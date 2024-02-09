#include "Window.hpp"
#include <Vendor/Tracy/tracy/TracyVulkan.hpp>
#include <queue>
#include <thread>

namespace Jkr {

struct CommandBuffers2 {
    std::array<VulkanCommandBuffer, 2> mCmdBuffers;
    CommandBuffers2(const VulkanDevice& inDev, const VulkanCommandPool& inPool)
        : mCmdBuffers {
            VulkanCommandBuffer(inDev, inPool, VulkanCommandBuffer::Type::Secondary),
            VulkanCommandBuffer(inDev, inPool, VulkanCommandBuffer::Type::Secondary)
        }
    {
    }
};

struct ThreadData {
    ThreadData(const Instance& inInstance, ui inNumCmdBuffers)
        : mCommandPool(inInstance.GetDevice(), inInstance.GetQueueContext())
    {
        for (int i = 0; i < inNumCmdBuffers; i++) {
            mCommandBuffers.emplace_back(mu<CommandBuffers2>(inInstance.GetDevice(), mCommandPool));
        }
    }

    VulkanCommandPool mCommandPool;
    v<up<CommandBuffers2>> mCommandBuffers;
};

class WindowMulT : public Window {
public:
    WindowMulT(const Instance& inInstance, const sv inTitle, ui inHeight, ui inWidth, ui inNumThreads, std::span<ui> inPerThreadBuffers, optref<ksai::ThreadPool> inPool = std::nullopt)
        : Window(inInstance, inTitle, inHeight, inWidth)
        , mThreadPool(inPool)
        , mBackgroundCommandPool(inInstance.GetDevice(), inInstance.GetQueueContext())
        , mUICommandPool(inInstance.GetDevice(), inInstance.GetQueueContext())
        , mSecondaryCommandBuffersBackground {
            VulkanCommandBuffer(inInstance.GetDevice(), mBackgroundCommandPool, VulkanCommandBuffer::Type::Secondary),
            VulkanCommandBuffer(inInstance.GetDevice(), mBackgroundCommandPool, VulkanCommandBuffer::Type::Secondary)
        }
        , mSecondaryCommandBuffersUI { VulkanCommandBuffer(inInstance.GetDevice(), mUICommandPool, VulkanCommandBuffer::Type::Secondary), VulkanCommandBuffer(inInstance.GetDevice(), mUICommandPool, VulkanCommandBuffer::Type::Secondary) }
    {
        for (int i = 0; i < inNumThreads; i++) {
            mThreadData.emplace_back(MakeUp<ThreadData>(mInstance, inPerThreadBuffers[i]));
        }
    }

    ~WindowMulT()
    {
    }
    virtual void Refresh() override;
    virtual void Draw(float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 0.1f, float d = 1.0f) override;
    const VulkanCommandBuffer& GetSecondaryCmdBufferBackground() const override { return mSecondaryCommandBuffersBackground[mCurrentFrame]; }
    const VulkanCommandBuffer& GetSecondaryCmdBufferUI() const override { return mSecondaryCommandBuffersUI[mCurrentFrame]; }
    const VulkanCommandBuffer& GetUtilCommandBuffer() const override { return mCommandBuffers[mCurrentFrame]; }
    const std::array<VulkanCommandBuffer, 2U>& GetCommandBuffers(ParameterContext inParameter, int inThreadId = 0, int inObjId = 0) const override
    {
        switch (inParameter) {
        case Jkr::Window::None:
            return mCommandBuffers;
        case Jkr::Window::UI:
            return mSecondaryCommandBuffersUI;
        case Jkr::Window::Background:
            return mSecondaryCommandBuffersBackground;
        default:
            return mThreadData[inThreadId]->mCommandBuffers[inObjId]->mCmdBuffers;
        }
    }

private:
    optref<ksai::ThreadPool> mThreadPool;
    VulkanCommandPool mBackgroundCommandPool;
    VulkanCommandPool mUICommandPool;
    std::array<VulkanCommandBuffer, mMaxFramesInFlight> mSecondaryCommandBuffersBackground;
    std::array<VulkanCommandBuffer, mMaxFramesInFlight> mSecondaryCommandBuffersUI;
    v<up<VulkanCommandBuffer>> mSecondaryCommandBuffers;
    v<up<ThreadData>> mThreadData;

    void CmdBackground();
    void CmdUI();
public:
    void SetBackgroundCallBack(std::function<void(void*)> inFunction) { mBackgroundCallback = inFunction; }
    void SetDedicated3DCallback(std::function<void(void*)> inFunction) { mDedicated3DCallback = inFunction; }

private:
    std::function<void(void*)> mBackgroundCallback = [](void*) {};
    std::function<void(void*)> mDedicated3DCallback = [](void*) {};
};

} // namespace Jkr

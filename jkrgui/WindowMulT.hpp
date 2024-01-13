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

        //mctx_ui1 = TracyVkContext(inInstance.GetPhysicalDevice().GetPhysicalDeviceHandle(), inInstance.GetDevice().GetDeviceHandle(), inInstance.GetGraphicsQueue().GetQueueHandle(), GetCommandBuffers(Jkr::Window::ParameterContext::UI)[0].GetCommandBufferHandle());
        //mctx_ui2 = TracyVkContext(inInstance.GetPhysicalDevice().GetPhysicalDeviceHandle(), inInstance.GetDevice().GetDeviceHandle(), inInstance.GetGraphicsQueue().GetQueueHandle(), GetCommandBuffers(Jkr::Window::ParameterContext::UI)[1].GetCommandBufferHandle());
        //mctx_bg1 = TracyVkContext(inInstance.GetPhysicalDevice().GetPhysicalDeviceHandle(), inInstance.GetDevice().GetDeviceHandle(), inInstance.GetGraphicsQueue().GetQueueHandle(), GetCommandBuffers(Jkr::Window::ParameterContext::Background)[0].GetCommandBufferHandle());
        //mctx_bg2 = TracyVkContext(inInstance.GetPhysicalDevice().GetPhysicalDeviceHandle(), inInstance.GetDevice().GetDeviceHandle(), inInstance.GetGraphicsQueue().GetQueueHandle(), GetCommandBuffers(Jkr::Window::ParameterContext::Background)[1].GetCommandBufferHandle());
        //mctx_main1 = TracyVkContext(inInstance.GetPhysicalDevice().GetPhysicalDeviceHandle(), inInstance.GetDevice().GetDeviceHandle(), inInstance.GetGraphicsQueue().GetQueueHandle(), GetCommandBuffers(Jkr::Window::ParameterContext::None)[0].GetCommandBufferHandle());
        //mctx_main2 = TracyVkContext(inInstance.GetPhysicalDevice().GetPhysicalDeviceHandle(), inInstance.GetDevice().GetDeviceHandle(), inInstance.GetGraphicsQueue().GetQueueHandle(), GetCommandBuffers(Jkr::Window::ParameterContext::None)[1].GetCommandBufferHandle());
        //TracyVkContextName(mctx_ui1, "ui1", 5);
        //TracyVkContextName(mctx_ui2, "ui2", 5);
        //TracyVkContextName(mctx_bg1, "bg1", 5);
        //TracyVkContextName(mctx_bg2, "bg2", 5);
        //TracyVkContextName(mctx_main1, "mn1", 5);
        //TracyVkContextName(mctx_main2, "mn2", 5);
    }

    ~WindowMulT()
    {
        //TracyVkDestroy(mctx_ui1);
        //TracyVkDestroy(mctx_ui2);
        //TracyVkDestroy(mctx_bg1);
        //TracyVkDestroy(mctx_bg2);
        //TracyVkDestroy(mctx_main1);
        //TracyVkDestroy(mctx_main2);
    }
    virtual void Draw(float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 0.1f, float d = 1.0f) override;
    const VulkanCommandBuffer& GetSecondaryCmdBufferBackground() const override { return mSecondaryCommandBuffersBackground[mCurrentFrame]; }
    const VulkanCommandBuffer& GetSecondaryCmdBufferUI() const override { return mSecondaryCommandBuffersUI[mCurrentFrame]; }
    const VulkanCommandBuffer& GetUtilCommandBuffer() const override { return mCommandBuffers[mCurrentFrame]; }
    const std::array<VulkanCommandBuffer, 2U>& GetCommandBuffers(ParameterContext inParameter, int inThreadId = 0, int inObjId = 0) const override
    {
        switch (inParameter) {
        case Jkr::Window::None:
            return mCommandBuffers;
            break;
        case Jkr::Window::UI:
            return mSecondaryCommandBuffersUI;
            break;
        case Jkr::Window::Background:
            return mSecondaryCommandBuffersBackground;
            break;
        default:
            return mThreadData[inThreadId]->mCommandBuffers[inObjId]->mCmdBuffers;
            break;
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

public:
    void SetBackgroundCallback(std::function<void(void*)> inFunction) { mBackgroundCallback = inFunction; }
    void SetDedicated3DCallback(std::function<void(void*)> inFunction) { mDedicated3DCallback = inFunction; }
    void CmdBackground();
    void CmdUI();

private:
    std::function<void(void*)> mBackgroundCallback = [](void*) {};
    std::function<void(void*)> mDedicated3DCallback = [](void*) {};

public:
    /* Performance Profiling */
    //tracy::VkCtx* mctx_ui1;
    //tracy::VkCtx* mctx_ui2;
    //tracy::VkCtx* mctx_bg1;
    //tracy::VkCtx* mctx_bg2;
    //tracy::VkCtx* mctx_main1;
    //tracy::VkCtx* mctx_main2;
};

} // namespace Jkr

#include "Window.hpp"
#include <queue>
#include <thread>

namespace Jkr {

struct ThreadData {
    ThreadData(const Instance& inInstance, ui inNumCmdBuffers)
        : mCommandPool(inInstance.GetDevice(), inInstance.GetQueueContext())
    {
        for (int i = 0; i < inNumCmdBuffers; i++) {
            mCommandBufferPerObj.emplace_back(
                MakeUp<VulkanCommandBuffer>(inInstance.GetDevice(),
                    inInstance.GetCommandPool(),
                    VulkanCommandBuffer::Type::Secondary));
        }
    }

    VulkanCommandPool mCommandPool;
    v<up<VulkanCommandBuffer>> mCommandBufferPerObj;
};

class WindowMulT : public Window {
public:
    WindowMulT(const Instance& inInstance, const sv inTitle, ui inHeight, ui inWidth, ui inNumThreads, std::span<ui> inPerThreadBuffers)
        : Window(inInstance, inTitle, inHeight, inWidth)
        , mSecondaryDispatchCommandBuffer(inInstance.GetDevice(), Window::GetCommandPool(), VulkanCommandBuffer::Type::Secondary)
        , mSecondaryCommandBufferBackground {
            VulkanCommandBuffer(inInstance.GetDevice(), Window::GetCommandPool(), VulkanCommandBuffer::Type::Secondary),
            VulkanCommandBuffer(inInstance.GetDevice(), Window::GetCommandPool(), VulkanCommandBuffer::Type::Secondary)
        }
        , mSecondaryCommandBuffersUI { VulkanCommandBuffer(inInstance.GetDevice(), Window::GetCommandPool(), VulkanCommandBuffer::Type::Secondary), VulkanCommandBuffer(inInstance.GetDevice(), Window::GetCommandPool(), VulkanCommandBuffer::Type::Secondary) }
    {
        for (int i = 0; i < inNumThreads; i++) {
            mThreadData.emplace_back(MakeUp<ThreadData>(mInstance, inPerThreadBuffers[i]));
        }
    }

    virtual void Draw(float r = 0.1f, float g = 0.1f, float b = 0.1f, float a = 0.1f, float d = 1.0f) override;
    const VulkanCommandBuffer& GetSecondaryCmdBufferBackground() const override { return mSecondaryCommandBufferBackground[mCurrentFrame]; }
    const VulkanCommandBuffer& GetSecondaryCmdBufferUI() const override { return mSecondaryCommandBuffersUI[mCurrentFrame]; }
    const VulkanCommandBuffer& GetSecondaryCmdBufferInThread(ui inThreadID, ui inObjId) const override { return *mThreadData[inThreadID]->mCommandBufferPerObj[inObjId]; }
    const VulkanCommandBuffer& GetUtilCommandBuffer() const override { return mCommandBuffers[mCurrentFrame]; }
    const std::array<VulkanCommandBuffer, 2U>& GetCommandBuffers() const override { return mSecondaryCommandBuffersUI; }

private:
    VulkanCommandBuffer mSecondaryDispatchCommandBuffer;
    std::array<VulkanCommandBuffer, mMaxFramesInFlight> mSecondaryCommandBufferBackground;
    std::array<VulkanCommandBuffer, mMaxFramesInFlight> mSecondaryCommandBuffersUI;
    v<up<VulkanCommandBuffer>> mSecondaryCommandBuffers;
    v<up<ThreadData>> mThreadData;

public:
    void SetBackgroundCallback(std::function<void(void*)> inFunction) { mBackgroundCallback = inFunction; }
    void SetDedicated3DCallback(std::function<void(void*)> inFunction) { mDedicated3DCallback = inFunction; }

private:
    std::function<void(void*)> mBackgroundCallback = [](void*) {};
    std::function<void(void*)> mDedicated3DCallback = [](void*) {};
};

} // namespace Jkr

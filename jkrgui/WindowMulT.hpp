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
    // virtual const std::array<VulkanCommandBuffer, 2U>& GetCommandBuffers() const override { return mSecondaryCommandBuffersUI; }
    GETTER& GetSecondaryCmdBufferBackground() { return mSecondaryCommandBufferBackground; }
    GETTER& GetSecondaryCmdBufferUI() { return mSecondaryCommandBuffersUI; }
    GETTER& GetSecondaryCmdBufferInThread(ui inThreadID) { return mThreadData[inThreadID]; }

private:
    VulkanCommandBuffer mSecondaryDispatchCommandBuffer;
    std::array<VulkanCommandBuffer, mMaxFramesInFlight> mSecondaryCommandBufferBackground;
    std::array<VulkanCommandBuffer, mMaxFramesInFlight> mSecondaryCommandBuffersUI;
    v<up<VulkanCommandBuffer>> mSecondaryCommandBuffers;
    v<up<ThreadData>> mThreadData;
};

} // namespace Jkr

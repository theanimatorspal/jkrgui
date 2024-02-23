#include "Window.hpp"

namespace Jkr {

    struct CommandBuffers2 {
        std::array<VulkanCommandBuffer, 2> mCmdBuffers;
        CommandBuffers2(const VulkanDevice& inDev, const VulkanCommandPool& inPool)
            : mCmdBuffers{
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
}

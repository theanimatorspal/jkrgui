#pragma once
#include "VulkanCommandBuffer.hpp"
#include "Window_base.hpp"

namespace Jkr {

template <ui MaxFramesInFlight> struct ThreadCommandBuffer {
    ThreadCommandBuffer(Instance &inInstance)
        : mCommandPool(inInstance.GetDevice(), inInstance.GetQueueContext()),
          mCommandBuffers(
               {VulkanCommandBuffer(
                     inInstance.GetDevice(), mCommandPool, VulkanCommandBuffer::Type::Secondary),
                VulkanCommandBuffer(
                     inInstance.GetDevice(), mCommandPool, VulkanCommandBuffer::Type::Secondary)}) {
    }

    VulkanCommandPool mCommandPool;
    std::array<VulkanCommandBuffer, MaxFramesInFlight> mCommandBuffers;
};
} // namespace Jkr

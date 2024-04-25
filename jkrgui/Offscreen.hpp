#pragma once
#include <Global/Standards.hpp>
#include <VulkanFrameBuffer.hpp>
#include <VulkanImageContext.hpp>

namespace Jkr {
using namespace ksai;
struct OffscreenPass {
    using FrameBufferType = VulkanFrameBuffer<1, VulkanImage<ImageContext::DepthImage>>;

    FrameBufferType mFrameBuffer;
};
} // namespace Jkr
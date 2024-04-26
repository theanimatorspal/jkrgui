#pragma once
#include "PainterParameter.hpp"
#include "PainterParameter_base.hpp"

namespace Jkr {
using namespace ksai;
struct ShadowPass {
    using FrameBufferType = VulkanFrameBuffer<1, VulkanImageVMA<ImageContext::DepthImage>>;
    using DepthImageType  = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformImage>;
    using RenderPassType  = VulkanRenderPass<RenderPassContext::Shadow>;
    GETTER& GetRenderPass() { return *mRenderpass; }
    GETTER& GetFrameBuffer() { return *mFrameBuffer; }
    GETTER& GetDepthImagePainterParameter() { return *mImage; }

    ShadowPass(const Instance& inInstance, ui inWidth, ui inHeight);

    private:
    const Instance& mInstance;
    Up<DepthImageType> mImage;
    Up<RenderPassType> mRenderpass;
    Up<FrameBufferType> mFrameBuffer;
};

} // namespace Jkr
#pragma once
#include "PainterParameter.hpp"
#include "PainterParameter_base.hpp"

namespace Jkr {
using namespace ksai;
struct ShadowPass {
    using FrameBufferType = VulkanFrameBuffer<1, VulkanImageVMA>;
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

struct DeferredPass {
    using FrameBufferType =
         VulkanFrameBuffer<4, VulkanImageVMA, VulkanImageVMA, VulkanImageVMA, VulkanImageVMA>;
    using ImageType      = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformImage>;
    using RenderPassType = VulkanRenderPass<RenderPassContext::Deferred>;
    GETTER& GetRenderPass() { return *mRenderPass; }
    GETTER& GetFrameBuffer() { return *mFrameBuffer; }

    DeferredPass(const Instance& inInstance, ui inWidth, ui inHeight);

    private:
    const Instance& mInstance;
    Up<ImageType> mDepthImage;
    Up<ImageType> mPositionImage;
    Up<ImageType> mNormalImage;
    Up<ImageType> mAlbedoImage;
    Up<RenderPassType> mRenderPass;
    Up<FrameBufferType> mFrameBuffer;
    Up<VulkanSampler> mSampler;
};

} // namespace Jkr
#pragma once
#include "PainterParameter.hpp"
#include "PainterParameter_base.hpp"

namespace Jkr {
class PainterCache;
class Painter;
namespace Renderer::_3D {
class Simple3D;
class World3D;
} // namespace Renderer::_3D
class Window;

using namespace ksai;
struct ShadowPass {
    using FrameBufferType = VulkanFrameBuffer<1, VulkanImageVMA>;
    using DepthImageType  = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformImage>;
    using RenderPassType  = VulkanRenderPass<RenderPassContext::Shadow>;
    GETTER &GetRenderPass() { return *mRenderpass; }
    GETTER &GetFrameBuffer() { return *mFrameBuffer; }
    GETTER &GetDepthImagePainterParameter() { return *mImage; }

    ShadowPass(const Instance &inInstance, ui inWidth, ui inHeight);

    private:
    const Instance &mInstance;
    Up<DepthImageType> mImage;
    Up<RenderPassType> mRenderpass;
    Up<FrameBufferType> mFrameBuffer;
};

struct DeferredPass {
    using FrameBufferType           = VulkanFrameBuffer<4, VulkanImageVMA, VulkanImageVMA, VulkanImageVMA, VulkanImageVMA>;
    using ImageType                 = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformImage>;
    using RenderPassType            = VulkanRenderPass<RenderPassContext::Deferred>;
    using CompositionRenderPassType = VulkanRenderPass<RenderPassContext::SingleColorAttachment>;
    GETTER &GetRenderPass() { return *mDeferredRenderPass; }
    GETTER &GetShadowRenderPass() { return *mShadowRenderPass; }
    GETTER &GetFrameBuffer() { return *mFrameBuffer; }
    void BeginShadowPass(Window &inWindow);
    void EndShadowPass(Window &inWindow);
    void BeginDeferred(Window &inWindow, const glm::vec4 &inColor);
    void EndDeferred(Window &inWindow);

    DeferredPass(const Instance &inInstance,
                 ui inWidth,
                 ui inHeight,
                 Renderer::_3D::Simple3D &inCompositionSimple3D,
                 Renderer::_3D::Simple3D &inShadowSimple3D,
                 Renderer::_3D::World3D &inWorld3D,
                 int inFramesInFlight);

    private:
    const Instance &mInstance;
    Up<ImageType> mDepthImage;
    Up<ImageType> mPositionImage;
    Up<ImageType> mNormalImage;
    Up<ImageType> mAlbedoImage;
    Up<RenderPassType> mDeferredRenderPass;
    Up<FrameBufferType> mFrameBuffer;
    Up<VulkanSampler> mSampler;

    VulkanDescriptorSet mCompositionDescriptorSet;
    Up<CompositionRenderPassType> mCompositionRenderPassType;
    Up<PainterCache> mCompositionPainterCache;
    Up<Painter> mCompositionPainter;
    Up<ImageType> mCompositionImage;
    v<VulkanCommandBuffer> mCompositionCommandBuffers;

    using ShadowRenderPassType  = VulkanRenderPass<RenderPassContext::Shadow>;
    using ShadowFrameBufferType = VulkanFrameBuffer<1, vk::ImageView>;
    Up<ImageType> mShadowMap;
    v<up<ShadowFrameBufferType>> mShadowFrameBuffers;
    Up<ShadowRenderPassType> mShadowRenderPass;
};

} // namespace Jkr
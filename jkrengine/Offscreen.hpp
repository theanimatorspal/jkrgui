#pragma once
#include "PainterParameter.hpp"
#include "PainterParameter_base.hpp"

constexpr int CASCADE_COUNT = 4;

namespace Jkr {
class PainterCache;
class Painter;
class Window;

namespace Renderer::_3D {
class Simple3D;
class World3D;
class Uniform3D;
class Camera3D;
} // namespace Renderer::_3D

using namespace ksai;
struct ShadowPass {
    using FrameBufferType = VulkanFrameBuffer;
    using DepthImageType  = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformImage>;
    using ImageType       = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformImage>;
    using RenderPassType  = VulkanRenderPass<RenderPassContext::Shadow>;
    struct Cascade {
        VulkanFrameBuffer mFrameBuffer;
        vk::ImageView mImageView;
        float mSplitDepth;
        glm::mat4 mViewProjMatrix;
    };
    GETTER &GetCascades() { return mCascades; }
    GETTER &GetRenderPass() { return *mCascadedRenderpass; }
    GETTER &GetFrameBuffer(int i) { return mCascades[i].mFrameBuffer; }
    GETTER &GetDepthImagePainterParameter() { return *mImage; }
    GETTER &GetCascadeSplits() { return mCascadeSplits; }

    ShadowPass(Instance &inInstance, ui inWidth, ui inHeight);
    void Update(Renderer::_3D::Camera3D &inCamera, glm::vec3 inLightPos);

    private:
    Instance &mInstance;
    Up<DepthImageType> mImage;
    Up<FrameBufferType> mFrameBuffer;
    std::array<Cascade, CASCADE_COUNT> mCascades;
    Up<RenderPassType> mCascadedRenderpass;

    float mCascadeSplits[CASCADE_COUNT];
    float mCascadeSplitLambda = 0.95f;
};

struct DeferredPass {
    using FrameBufferType = VulkanFrameBuffer;
    using RenderPassType  = VulkanRenderPass<RenderPassContext::Deferred>;
    using ImageType       = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformImage>;
    using CompositionRenderPassType  = VulkanRenderPass<RenderPassContext::SingleColorAttachment>;
    using CompositionFrameBufferType = VulkanFrameBuffer;
    using ShadowRenderPassType       = VulkanRenderPass<RenderPassContext::Shadow>;
    using ShadowFrameBufferType      = VulkanFrameBuffer;
    GETTER &GetCompositionRenderPass() { return *mCompositionRenderPass; }
    GETTER &GetRenderPass() { return *mDeferredRenderPass; }
    GETTER &GetFrameBuffer() { return *mFrameBuffer; }
    GETTER &GetDeferredCompositionImage() { return *mCompositionImage; }

    void BeginDeferred(Window &inWindow, float r, float g, float b, float a, float d);
    void EndDeferred(Window &inWindow);

    /// @brief This function expects at least one uniform3d created
    ///        in the world, (with World::AddUniform3D(), and built with
    ///        Uniform3D::Build()) in the inWorld3D parameter
    void ExecuteDeferredComposition(Window &inWindow,
                                    Renderer::_3D::Simple3D &inCompositionSimple3D,
                                    Renderer::_3D::World3D &inWorld3D);

    DeferredPass(Instance &inInstance, ui inWidth, ui inHeight, int inFramesInFlight);
    /// @todo Name this correctly, this function should be "Build", and the Build function
    ///         Should be named "Prepare".
    void Prepare(Renderer::_3D::Simple3D &inCompositionSimple3D, Renderer::_3D::World3D &inWorld3D);

    private:
    Instance &mInstance;
    Up<ImageType> mDepthImage;
    Up<ImageType> mPositionImage;
    Up<ImageType> mNormalImage;
    Up<ImageType> mAlbedoImage;
    Up<RenderPassType> mDeferredRenderPass;
    Up<FrameBufferType> mFrameBuffer;
    Up<VulkanSampler> mSampler;

    Up<Renderer::_3D::Uniform3D> mCompositionUniform3D;
    Up<CompositionRenderPassType> mCompositionRenderPass;
    Up<CompositionFrameBufferType> mCompositionFrameBuffer;
    Up<ImageType> mCompositionImage;

    glm::uvec2 mDimension{};
};

struct ArbritaryPass {
    using FrameBufferType = VulkanFrameBuffer;
    using RenderPassType  = VulkanRenderPass<RenderPassContext::SingleColorAttachment>;
    using ImageType       = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformImage>;

    ArbritaryPass(Instance &inInstance, int inWidth, int inHeight);
    Instance &mInstance;
    Up<FrameBufferType> mFrameBuffer;
    Up<RenderPassType> mRenderpass;
    Up<ImageType> mImage;
};

} // namespace Jkr
#include "Offscreen.hpp"
#include "Painter.hpp"
#include "PainterCache.hpp"
#include "Renderers/ThreeD/Simple3D.hpp"
#include "Renderers/ThreeD/Uniform3D.hpp"
#include "Renderers/ThreeD/World3D.hpp"

using namespace Jkr;

ShadowPass::ShadowPass(const Instance &inInstance, ui inWidth, ui inHeight)
    : mInstance(inInstance) {
    mImage = mu<DepthImageType>(inInstance);
    mImage->SetupDepth(inWidth, inHeight);
    mRenderpass = mu<RenderPassType>(inInstance.GetDevice(), mImage->GetDepthImage());
    mFrameBuffer =
         mu<FrameBufferType>(inInstance.GetDevice(), *mRenderpass, mImage->GetDepthImage());
}

Jkr::DeferredPass::DeferredPass(const Instance &inInstance,
                                ui inWidth,
                                ui inHeight,
                                int inFramesInFlight)
    : mInstance(inInstance), mDimension(inWidth, inHeight) {

    // Deferred stuff
    {
        mPositionImage                   = mu<ImageType>(inInstance);
        mNormalImage                     = mu<ImageType>(inInstance);
        mAlbedoImage                     = mu<ImageType>(inInstance);
        mDepthImage                      = mu<ImageType>(inInstance);
        mPositionImage->mUniformImagePtr = mu<VulkanImageVMA>(
             inInstance.GetVMA(),
             mInstance.GetDevice(),
             inWidth,
             inHeight,
             ksai::ImageContext::Default,
             4,
             1,
             1,
             1,
             vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
             vk::ImageLayout::eGeneral,
             vk::Format::eR16G16B16A16Sfloat);
        mNormalImage->mUniformImagePtr = mu<VulkanImageVMA>(
             inInstance.GetVMA(),
             mInstance.GetDevice(),
             inWidth,
             inHeight,
             ksai::ImageContext::Default,
             4,
             1,
             1,
             1,
             vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
             vk::ImageLayout::eGeneral,
             vk::Format::eR16G16B16A16Sfloat);
        mAlbedoImage->mUniformImagePtr = mu<VulkanImageVMA>(
             inInstance.GetVMA(),
             mInstance.GetDevice(),
             inWidth,
             inHeight,
             ksai::ImageContext::Default,
             4,
             1,
             1,
             1,
             vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled,
             vk::ImageLayout::eGeneral,
             vk::Format::eR8G8B8A8Unorm);
        mDepthImage->mUniformImagePtr = mu<VulkanImageVMA>(
             inInstance.GetVMA(),
             mInstance.GetDevice(),
             inWidth,
             inHeight,
             ksai::ImageContext::DepthImage,
             4,
             1,
             1,
             1,
             vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eDepthStencilAttachment,
             vk::ImageLayout::eGeneral,
             vk::Format::eD16Unorm);
        mDeferredRenderPass = mu<RenderPassType>(mInstance.GetDevice(),
                                                 *mPositionImage->mUniformImagePtr,
                                                 *mNormalImage->mUniformImagePtr,
                                                 *mAlbedoImage->mUniformImagePtr,
                                                 *mDepthImage->mUniformImagePtr);
        mFrameBuffer        = mu<FrameBufferType>(mInstance.GetDevice(),
                                           *mDeferredRenderPass,
                                           *mPositionImage->mUniformImagePtr,
                                           *mNormalImage->mUniformImagePtr,
                                           *mAlbedoImage->mUniformImagePtr,
                                           *mDepthImage->mUniformImagePtr);
        auto CreateSampler  = [&]() {
            return mv(mu<VulkanSampler>(inInstance.GetDevice(),
                                        vk::SamplerCreateInfo()
                                             .setMagFilter(vk::Filter::eNearest)
                                             .setMinFilter(vk::Filter::eNearest)
                                             .setMipmapMode(vk::SamplerMipmapMode::eLinear)
                                             .setAddressModeU(vk::SamplerAddressMode::eClampToEdge)
                                             .setAddressModeV(vk::SamplerAddressMode::eClampToEdge)
                                             .setAddressModeW(vk::SamplerAddressMode::eClampToEdge)
                                             .setMipLodBias(0.0f)
                                             .setMaxAnisotropy(1.0f)
                                             .setMinLod(0.0f)
                                             .setMaxLod(1.0f)
                                             .setBorderColor(vk::BorderColor::eFloatOpaqueWhite)));
        };

        mPositionImage->mSampler            = mv(CreateSampler());
        mNormalImage->mSampler              = mv(CreateSampler());
        mAlbedoImage->mSampler              = mv(CreateSampler());

        mCompositionImage                   = mu<ImageType>(mInstance);
        mCompositionImage->mUniformImagePtr = mu<VulkanImageVMA>(
             inInstance.GetVMA(),
             mInstance.GetDevice(),
             inWidth,
             inHeight,
             ksai::ImageContext::Default,
             4,
             1,
             1,
             1,
             vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eSampled |
                  vk::ImageUsageFlagBits::eTransferSrc,
             vk::ImageLayout::eGeneral,
             vk::Format::eR8G8B8A8Unorm);
        mCompositionRenderPass = mu<CompositionRenderPassType>(
             mInstance.GetDevice(), mCompositionImage->GetUniformImage());
        mCompositionFrameBuffer = mu<CompositionFrameBufferType>(
             mInstance.GetDevice(), *mCompositionRenderPass, *mCompositionImage->mUniformImagePtr);
    }

    // Deferred Shadows
    {
        mShadowMap                   = mu<ImageType>(mInstance);
        mShadowMap->mUniformImagePtr = mu<VulkanImageVMA>(
             inInstance.GetVMA(),
             mInstance.GetDevice(),
             std::max(inWidth, inHeight),
             std::max(inWidth, inHeight),
             ksai::ImageContext::DepthImage,
             4,
             kstd::LightCount,
             1,
             1,
             vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eDepthStencilAttachment,
             vk::ImageLayout::eGeneral,
             vk::Format::eD16Unorm);
        mShadowRenderPass =
             mu<ShadowRenderPassType>(mInstance.GetDevice(), *mShadowMap->mUniformImagePtr);
        mShadowMap->mUniformImagePtr->BuildImageViewsByLayers();
        auto &ShadowImageViews = mShadowMap->mUniformImagePtr->GetImageViews();
        for (int i = 0; i < ShadowImageViews.size(); i++) {
            mShadowFrameBuffers.emplace_back(mu<ShadowFrameBufferType>(
                 mInstance.GetDevice(),
                 *mShadowRenderPass,
                 vk::Extent2D{std::max(inWidth, inHeight), std::max(inWidth, inHeight)},
                 ShadowImageViews[i]));
        }
    }
}

static void Barrier(auto &inImage, auto &inCmd) {
    inImage->mUniformImagePtr->CmdTransitionImageLayout(inCmd,
                                                        vk::ImageLayout::eShaderReadOnlyOptimal,
                                                        vk::ImageLayout::eShaderReadOnlyOptimal,
                                                        vk::PipelineStageFlagBits::eFragmentShader,
                                                        vk::PipelineStageFlagBits::eFragmentShader,
                                                        vk::AccessFlagBits::eShaderWrite,
                                                        vk::AccessFlagBits::eShaderRead);
}

void DeferredPass::BeginDeferred(Window &inWindow, float r, float g, float b, float a, float d) {
    auto &cmd =
         inWindow.GetCommandBuffers(Window::ParameterContext::None)[inWindow.GetCurrentFrame()];
    auto &cmdh = cmd.GetCommandBufferHandle();
    cmd.BeginRenderPass(*mDeferredRenderPass,
                        vk::Extent2D{mDimension.x, mDimension.y},
                        *mFrameBuffer,
                        {r, g, b, a, d},
                        4);
}
void DeferredPass::EndDeferred(Window &inWindow) {
    auto &cmd =
         inWindow.GetCommandBuffers(Window::ParameterContext::None)[inWindow.GetCurrentFrame()];
    cmd.EndRenderPass();
}

void DeferredPass::ExecuteDeferredComposition(Window &inWindow,
                                              Renderer::_3D::Simple3D &inCompositionSimple3D,
                                              Renderer::_3D::World3D &inWorld3D) {
    constexpr auto Set          = 1;
    constexpr auto SetWorldInfo = 0;
    auto &cmd =
         inWindow.GetCommandBuffers(Window::ParameterContext::None)[inWindow.GetCurrentFrame()];
    auto &cmdh = cmd.GetCommandBufferHandle();
    Barrier(mPositionImage, cmd);
    Barrier(mNormalImage, cmd);
    Barrier(mAlbedoImage, cmd);
    inWorld3D.UpdateWorldInfoToUniform3D(*mCompositionUniform3D);

    cmd.BeginRenderPass(*mCompositionRenderPass,
                        vk::Extent2D{mDimension.x, mDimension.y},
                        *mCompositionFrameBuffer,
                        {1.0f, 1.0f, 1.f, 1.f, 1.f});
    mCompositionUniform3D->Bind(
         inWindow, inCompositionSimple3D, SetWorldInfo, Window::ParameterContext::None);
    mCompositionUniform3D->Bind(
         inWindow, inCompositionSimple3D, Set, Window::ParameterContext::None);
    inCompositionSimple3D.BindByCommandBuffer(cmd);
    cmdh.draw(3, 1, 0, 0);
    cmd.EndRenderPass();
    //    TransitionImage(mDepthImage, cmd);
}

void DeferredPass::Prepare(Renderer::_3D::Simple3D &inCompositionSimple3D,
                           Renderer::_3D::World3D &inWorld3D) {
    constexpr auto Set    = 1;
    mCompositionUniform3D = mu<Renderer::_3D::Uniform3D>(mInstance);
    mCompositionUniform3D->Build(inCompositionSimple3D);
    inWorld3D.AddWorldInfoToUniform3DEXT(*mCompositionUniform3D);

    mPositionImage->mUniformImagePtr->GetImagePropertyRef().mCurrentImageLayout =
         vk::ImageLayout::eShaderReadOnlyOptimal;
    mNormalImage->mUniformImagePtr->GetImagePropertyRef().mCurrentImageLayout =
         vk::ImageLayout::eShaderReadOnlyOptimal;
    mAlbedoImage->mUniformImagePtr->GetImagePropertyRef().mCurrentImageLayout =
         vk::ImageLayout::eShaderReadOnlyOptimal;

    mPositionImage->Register(0, 3, 0, mCompositionUniform3D->GetVulkanDescriptorSet(), Set);
    mNormalImage->Register(0, 4, 0, mCompositionUniform3D->GetVulkanDescriptorSet(), Set);
    mAlbedoImage->Register(0, 5, 0, mCompositionUniform3D->GetVulkanDescriptorSet(), Set);
}
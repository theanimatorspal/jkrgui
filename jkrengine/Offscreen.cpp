#include "Offscreen.hpp"
#include "Painter.hpp"
#include "PainterCache.hpp"
#include "Renderers/ThreeD/Simple3D.hpp"
#include "Renderers/ThreeD/Uniform3D.hpp"
#include "Renderers/ThreeD/World3D.hpp"

using namespace Jkr;

ShadowPass::ShadowPass(Instance &inInstance, ui inWidth, ui inHeight) : mInstance(inInstance) {
    ///@note new one from here
    mImage                   = mu<ImageType>(inInstance);
    mImage->mUniformImagePtr = mu<VulkanImageVMA>();
    auto &CascadeImage       = *mImage->mUniformImagePtr;
    mImage->mUniformImagePtr->Init(
         {&inInstance.GetVMA(),
          &inInstance.GetDevice(),
          inWidth,
          inHeight,
          ImageContext::DepthImage,
          4,
          CASCADE_COUNT,
          1,
          1,
          false,
          vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eDepthStencilAttachment,
          std::nullopt,
          std::nullopt,
          vk::ImageViewType::e2DArray});

    CascadeImage.BuildImageViewsByLayers();
    auto ImageViews     = CascadeImage.GetImageViews();
    mCascadedRenderpass = mu<RenderPassType>(inInstance.GetDevice(), CascadeImage);

    for (int i = 0; auto &Cascade : mCascades) {
        Cascade.mFrameBuffer.SetExtent(vk::Extent2D{inWidth, inHeight});
        Cascade.mFrameBuffer.Init(inInstance.GetDevice(), *mCascadedRenderpass, ImageViews[i]);
        ++i;
    }
}

///@note from Sascha willems' code
///@note i.e from https://johanmedestrom.wordpress.com/2016/03/18/opengl-cascaded-shadow-maps/
void Jkr::ShadowPass::Update(Renderer::_3D::Camera3D &inCamera, glm::vec3 inLightPos) {
    float nearClip  = inCamera.GetNearZ();
    float farClip   = inCamera.GetFarZ();
    float clipRange = farClip - nearClip;

    float minZ      = nearClip;
    float maxZ      = nearClip + clipRange;

    float range     = maxZ - minZ;
    float ratio     = maxZ / minZ;

    for (uint32_t i = 0; i < CASCADE_COUNT; ++i) {
        float p           = (i + 1) / static_cast<float>(CASCADE_COUNT);
        float log         = minZ * std::pow(ratio, p);
        float uniform     = minZ + range * p;
        float d           = mCascadeSplitLambda * (log - uniform) + uniform;
        mCascadeSplits[i] = (d - nearClip) / clipRange;
    }

    float lastSplitDist = 0.0f;
    for (uint32_t i = 0; i < CASCADE_COUNT; ++i) {
        float splitDist             = mCascadeSplits[i];
        glm::vec3 frustumCorners[8] = {
             glm::vec3(-1.0f, 1.0f, 0.0f),
             glm::vec3(1.0f, 1.0f, 0.0f),
             glm::vec3(1.0f, -1.0f, 0.0f),
             glm::vec3(-1.0f, -1.0f, 0.0f),
             glm::vec3(-1.0f, 1.0f, 1.0f),
             glm::vec3(1.0f, 1.0f, 1.0f),
             glm::vec3(1.0f, -1.0f, 1.0f),
             glm::vec3(-1.0f, -1.0f, 1.0f),
        };

        glm::mat4 invCam = glm::inverse(inCamera.GetProjection() * inCamera.GetView());
        for (uint32_t j = 0; j < 8; ++j) {
            glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[j], 1.0f);
            frustumCorners[j]   = invCorner / invCorner.w;
        }

        for (uint32_t j = 0; j < 4; ++j) {
            glm::vec3 dist        = frustumCorners[j + 4] - frustumCorners[j];
            frustumCorners[j + 4] = frustumCorners[j] + (dist * splitDist);
            frustumCorners[j]     = frustumCorners[j] + (dist * lastSplitDist);
        }

        ///@note Get Frustum Center
        glm::vec3 frustumCenter = glm::vec3(0.0f);
        for (uint32_t j = 0; j < 8; ++j) {
            frustumCenter += frustumCorners[j];
        }

        frustumCenter /= 8.0f;

        float radius = 0.0f;
        for (uint32_t j = 0; j < 8; ++j) {
            float distance = glm::length(frustumCorners[j] - frustumCenter);
            radius         = glm::max(radius, distance);
        }
        radius                    = std::ceil(radius * 16.0f) / 16.0f;

        glm::vec3 maxExtents      = glm::vec3(radius);
        glm::vec3 minExtents      = -maxExtents;

        glm::vec3 lightDir        = normalize(-inLightPos);
        glm::mat4 lightViewMatrix = glm::lookAt(
             frustumCenter - lightDir * -minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 lightOrthoMatrix   = glm::ortho(minExtents.x,
                                                maxExtents.x,
                                                minExtents.y,
                                                maxExtents.y,
                                                0.0f,
                                                maxExtents.z - minExtents.z);
        mCascades[i].mSplitDepth     = (inCamera.GetNearZ() + splitDist * clipRange) * -1.0f;
        mCascades[i].mViewProjMatrix = lightOrthoMatrix * lightViewMatrix;
        lastSplitDist                = mCascadeSplits[i];
    }
}

Jkr::DeferredPass::DeferredPass(Instance &inInstance, ui inWidth, ui inHeight, int inFramesInFlight)
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
    inWorld3D.UpdateWorldInfoToUniform3D(*mCompositionUniform3D, inWindow);

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

ArbritaryPass::ArbritaryPass(Instance &inInstance, int inWidth, int inHeight)
    : mInstance(inInstance) {
    mImage                   = MakeUp<ImageType>(mInstance);
    mImage->mUniformImagePtr = mu<VulkanImageVMA>(inInstance.GetVMA(),
                                                  mInstance.GetDevice(),
                                                  inWidth,
                                                  inHeight,
                                                  ksai::ImageContext::Default,
                                                  4,
                                                  1,
                                                  1,
                                                  1,
                                                  vk::ImageUsageFlagBits::eColorAttachment |
                                                       vk::ImageUsageFlagBits::eSampled |
                                                       vk::ImageUsageFlagBits::eTransferSrc,
                                                  vk::ImageLayout::eGeneral,
                                                  vk::Format::eR8G8B8A8Unorm);
    mRenderpass              = mu<RenderPassType>(mInstance.GetDevice(), mImage->GetUniformImage());
    mFrameBuffer =
         mu<FrameBufferType>(mInstance.GetDevice(), *mRenderpass, *mImage->mUniformImagePtr);
}
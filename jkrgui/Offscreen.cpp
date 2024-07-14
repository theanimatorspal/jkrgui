#include "Offscreen.hpp"
#include "Painter.hpp"
#include "PainterCache.hpp"
#include "Renderers/ThreeD/Simple3D.hpp"
#include "Renderers/ThreeD/World3D.hpp"

using namespace Jkr;

ShadowPass::ShadowPass(const Instance &inInstance, ui inWidth, ui inHeight) : mInstance(inInstance) {
    mImage = mu<DepthImageType>(inInstance);
    mImage->SetupDepth(inWidth, inHeight);
    mRenderpass  = mu<RenderPassType>(inInstance.GetDevice(), mImage->GetDepthImage());
    mFrameBuffer = mu<FrameBufferType>(inInstance.GetDevice(), *mRenderpass, mImage->GetDepthImage());
}

Jkr::DeferredPass::DeferredPass(const Instance &inInstance,
                                ui inWidth,
                                ui inHeight,
                                Renderer::_3D::Simple3D &inCompositionSimple3D,
                                Renderer::_3D::Simple3D &inShadowSimple3D,
                                Renderer::_3D::World3D &inWorld3D,
                                int inFramesInFlight)
    : mInstance(inInstance) {

    // Deferred stuff
    {
        mPositionImage = mu<ImageType>(inInstance);
        mNormalImage   = mu<ImageType>(inInstance);
        mAlbedoImage   = mu<ImageType>(inInstance);
        mDepthImage    = mu<ImageType>(inInstance);
        mPositionImage->mUniformImagePtr =
             mu<VulkanImageVMA>(inInstance.GetVMA(),
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
        mNormalImage->mUniformImagePtr =
             mu<VulkanImageVMA>(inInstance.GetVMA(),
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
        mAlbedoImage->mUniformImagePtr =
             mu<VulkanImageVMA>(inInstance.GetVMA(),
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
                                vk::Format::eR8G8B8Unorm);
        mDepthImage->mUniformImagePtr =
             mu<VulkanImageVMA>(inInstance.GetVMA(),
                                mInstance.GetDevice(),
                                inWidth,
                                inHeight,
                                ksai::ImageContext::Default,
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

        mCompositionDescriptorSet.Init({&mInstance.GetDevice(),
                                        &mInstance.GetDescriptorPool(),
                                        &inCompositionSimple3D.GetPainterCache().GetVertexFragmentDescriptorSetLayout()});
        mPositionImage->mSampler = mv(CreateSampler());
        mNormalImage->mSampler   = mv(CreateSampler());
        mAlbedoImage->mSampler   = mv(CreateSampler());

        constexpr auto Set       = 1;
        mPositionImage->Register(0, 0, 0, mCompositionDescriptorSet, Set);
        mNormalImage->Register(0, 1, 0, mCompositionDescriptorSet, Set);
        mAlbedoImage->Register(0, 2, 0, mCompositionDescriptorSet, Set);

        // Build Commmand Buffers
        {
            mCompositionCommandBuffers.resize(inFramesInFlight);
            for (auto &cmdbuf : mCompositionCommandBuffers) {
                cmdbuf.Init({&mInstance.GetDevice(), &mInstance.GetCommandPool()});
                cmdbuf.Begin();
                cmdbuf.BeginRenderPass(*mDeferredRenderPass, vk::Extent2D{inWidth, inHeight}, *mFrameBuffer, {1, 1, 1, 1, 1});
                cmdbuf.GetCommandBufferHandle().setViewport(0, vk::Viewport{0, 0, (float)inWidth, (float)inHeight, 0.0f, 1.0f});
                cmdbuf.GetCommandBufferHandle().setScissor(0, vk::Rect2D{{}, vk::Extent2D{inWidth, inHeight}});
                mCompositionDescriptorSet.Bind(vk::PipelineBindPoint::eGraphics,
                                               cmdbuf,
                                               inCompositionSimple3D.GetPainterCache().GetVertexFragmentPipelineLayout());
                inCompositionSimple3D.BindByCommandBuffer(cmdbuf);
                cmdbuf.GetCommandBufferHandle().draw(3, 1, 0, 0);
                cmdbuf.End();
            }
        }
    }

    // Deferred Shadows
    {
        mShadowMap->mUniformImagePtr =
             mu<VulkanImageVMA>(inInstance.GetVMA(),
                                mInstance.GetDevice(),
                                std::max(inWidth, inHeight),
                                std::max(inWidth, inHeight),
                                ksai::ImageContext::Default,
                                4,
                                kstd::LightCount,
                                1,
                                1,
                                vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eDepthStencilAttachment,
                                vk::ImageLayout::eGeneral,
                                vk::Format::eD16Unorm);
        mShadowRenderPass = mu<ShadowRenderPassType>(mInstance.GetDevice(), *mShadowMap->mUniformImagePtr);
        mShadowMap->mUniformImagePtr->BuildImageViewsByLayers();
        auto &ShadowImageViews = mShadowMap->mUniformImagePtr->GetImageViews();
        for (int i = 0; i < ShadowImageViews.size(); i++) {
            mShadowFrameBuffers.emplace_back(
                 mu<ShadowFrameBufferType>(mInstance.GetDevice(), *mShadowRenderPass, ShadowImageViews[i]));
        }
    }
}

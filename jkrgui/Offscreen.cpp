#include "Offscreen.hpp"
using namespace Jkr;

ShadowPass::ShadowPass(const Instance& inInstance, ui inWidth, ui inHeight)
    : mInstance(inInstance) {
    mImage = mu<DepthImageType>(inInstance);
    mImage->SetupDepth(inWidth, inHeight);
    mRenderpass = mu<RenderPassType>(inInstance.GetDevice(), mImage->GetDepthImage());
    mFrameBuffer =
         mu<FrameBufferType>(inInstance.GetDevice(), *mRenderpass, mImage->GetDepthImage());
}

DeferredPass::DeferredPass(const Instance& inInstance, ui inWidth, ui inHeight)
    : mInstance(inInstance) {
    mPositionImage                   = mu<ImageType>(inInstance);
    mNormalImage                     = mu<ImageType>(inInstance);
    mAlbedoImage                     = mu<ImageType>(inInstance);
    mDepthImage                      = mu<ImageType>(inInstance);
    mPositionImage->mUniformImagePtr = mu<VulkanImageVMA>(inInstance.GetVMA(),
                                                          mInstance.GetDevice(),
                                                          inWidth,
                                                          inHeight,
                                                          ksai::ImageContext::Default,
                                                          4,
                                                          1,
                                                          1,
                                                          1,
                                                          vk::ImageUsageFlagBits::eColorAttachment |
                                                               vk::ImageUsageFlagBits::eSampled,
                                                          vk::ImageLayout::eGeneral);
    mNormalImage->mUniformImagePtr   = mu<VulkanImageVMA>(inInstance.GetVMA(),
                                                        mInstance.GetDevice(),
                                                        inWidth,
                                                        inHeight,
                                                        ksai::ImageContext::Default,
                                                        4,
                                                        1,
                                                        1,
                                                        1,
                                                        vk::ImageUsageFlagBits::eColorAttachment |
                                                             vk::ImageUsageFlagBits::eSampled,
                                                        vk::ImageLayout::eGeneral);

    mAlbedoImage->mUniformImagePtr   = mu<VulkanImageVMA>(inInstance.GetVMA(),
                                                        mInstance.GetDevice(),
                                                        inWidth,
                                                        inHeight,
                                                        ksai::ImageContext::Default,
                                                        4,
                                                        1,
                                                        1,
                                                        1,
                                                        vk::ImageUsageFlagBits::eColorAttachment |
                                                             vk::ImageUsageFlagBits::eSampled,
                                                        vk::ImageLayout::eGeneral);

    mDepthImage->mUniformImagePtr    = mu<VulkanImageVMA>(
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
              vk::ImageUsageFlagBits::eDepthStencilAttachment,
         vk::ImageLayout::eGeneral,
         vk::Format::eD16Unorm);
    mRenderPass  = mu<RenderPassType>(mInstance.GetDevice(),
                                     *mPositionImage->mUniformImagePtr,
                                     *mNormalImage->mUniformImagePtr,
                                     *mAlbedoImage->mUniformImagePtr,
                                     *mDepthImage->mUniformImagePtr);
    mFrameBuffer = mu<FrameBufferType>(mInstance.GetDevice(),
                                       *mRenderPass,
                                       *mPositionImage->mUniformImagePtr,
                                       *mNormalImage->mUniformImagePtr,
                                       *mAlbedoImage->mUniformImagePtr,
                                       *mDepthImage->mUniformImagePtr);
    mSampler     = mu<VulkanSampler>(inInstance.GetDevice(),
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
                                      .setBorderColor(vk::BorderColor::eFloatOpaqueWhite));
}
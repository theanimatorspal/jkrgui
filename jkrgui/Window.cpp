#include "Window.hpp"

Jkr::Window::Window(const Instance& inInstance, std::string_view inTitle, int inHeight, int inWidth)
    : SDLWindow(inTitle, inHeight, inWidth)
    , mInstance(inInstance)
    , mSurface(mInstance.GetVulkanInstance(), mSDLWindowPtr)
    , mSwapChain(mInstance.GetDevice(),
          mInstance.GetQueueContext(),
          mSurface.ProcessCurrentSurfaceConditions(mInstance.GetPhysicalDevice())
              .ProcessCurrentSurfaceExtents(mInstance.GetPhysicalDevice()))
    , mColorImageRenderTarget(mInstance.GetDevice(), mSurface, 4)
    , mDepthImage(mInstance.GetDevice(), mSurface, 4) // Here
    , mRenderPass(mInstance.GetDevice(), mSurface, mColorImageRenderTarget, mDepthImage, vk::SampleCountFlagBits::e4)
    , mSwapChainImages { mSwapChain.GetVulkanImages(mInstance.GetDevice(), mSurface) }
    , mImageAvailableSemaphores { VulkanSemaphore(mInstance.GetDevice()),
        VulkanSemaphore(mInstance.GetDevice()) }
    , mRenderFinishedSemaphores { VulkanSemaphore(mInstance.GetDevice()),
        VulkanSemaphore(mInstance.GetDevice()) }
    , mFences { VulkanFence(mInstance.GetDevice()), VulkanFence(mInstance.GetDevice()) }
    , mCommandPool(inInstance.GetDevice(), inInstance.GetQueueContext())
    , mCommandBuffers { VulkanCommandBuffer(inInstance.GetDevice(), mCommandPool),
        VulkanCommandBuffer(inInstance.GetDevice(), mCommandPool) }
{
    ksai_print("SwapChainImagesSize: %d", mSwapChainImages.size());
    for (int i = 0; i < mSwapChainImages.size(); i++) {
        mFrameBuffers.emplace_back(MakeUp<FrameBufferType>(inInstance.GetDevice(), mRenderPass, mColorImageRenderTarget, mDepthImage, mSwapChainImages[i]));
    }
}

void Jkr::Window::Draw(float r, float g, float b, float a, float d)
{
    mUpdateFunction(mData);
    std::array<float, 5>
        ClearValues = { r, g, b, a, d };
    mFences[mCurrentFrame].Wait();
    std::pair<uint32_t, uint32_t> SwapChainResult = mSwapChain.AcquireNextImage(mImageAvailableSemaphores[mCurrentFrame]);
    mFences[mCurrentFrame].Reset();
    auto& CommandBuffers = this->GetCommandBuffers(ParameterContext::None);
    if (not mSwapChain.ImageIsNotOptimal(SwapChainResult)) {
        mAcquiredImageIndex = SwapChainResult.second;
        ksai_print("Acquired image Index: %d", mAcquiredImageIndex);
        CommandBuffers[mCurrentFrame].Reset();
        CommandBuffers[mCurrentFrame].Begin();

        mComputeDispatchFunction(mData);

        CommandBuffers[mCurrentFrame].BeginRenderPass(
            mRenderPass,
            mSurface,
            *mFrameBuffers[mAcquiredImageIndex], // यो स्थानमा जहिल्यै झुक्किन्छ । फ्रेम बफर एउटा हुने हो ।  "Acquired Image Index" अनुसार ।
            ClearValues);

        mDrawFunction(mData);

        CommandBuffers[mCurrentFrame].EndRenderPass();
        mPostRenderingFunction(mData);
        CommandBuffers[mCurrentFrame].End();

        mInstance.GetGraphicsQueue().Submit<SubmitContext::ColorAttachment>(
            mImageAvailableSemaphores[mCurrentFrame],
            mRenderFinishedSemaphores[mCurrentFrame],
            mFences[mCurrentFrame],
            CommandBuffers[mCurrentFrame]);
        uint32_t Result = mInstance.GetGraphicsQueue().Present<mMaxFramesInFlight, SubmitContext::ColorAttachment>(
            mSwapChain,
            mRenderFinishedSemaphores[mCurrentFrame],
            mAcquiredImageIndex);
        if (mSwapChain.ImageIsNotOptimal(Result)) {
            Refresh();
        } else {
            mCurrentFrame = (mCurrentFrame + 1) % mMaxFramesInFlight;
        }
    } else {
        ksai_print("Refresh");
        Refresh();
    }
}


void Jkr::Window::Refresh()
{
    mSurface.ProcessCurrentSurfaceConditions(mInstance.GetPhysicalDevice());
    mSwapChain = VulkanSwapChain<mMaxFramesInFlight>(mInstance.GetDevice(), mInstance.GetQueueContext(), mSurface);
    mSwapChainImages = mSwapChain.GetVulkanImages(mInstance.GetDevice(), mSurface);
    mDepthImage = VulkanImage<ImageContext::DepthImage>(mInstance.GetDevice(), mSurface);
    for (int i = 0; i < mSwapChainImages.size(); i++) {
        mFrameBuffers[i] = MakeUp<FrameBufferType>(mInstance.GetDevice(), mRenderPass, mColorImageRenderTarget, mDepthImage, mSwapChainImages[i]);
    }
}

void Jkr::Window::CmdCopySwapChainImageToBufferPostRendering(VulkanBufferBase& inbuffer)
{
	// TODO This has to be tested
    auto& CommandBuffer = this->GetCommandBuffers(ParameterContext::None)[mCurrentFrame];
	auto& Cmd = CommandBuffer.GetCommandBufferHandle();
	auto& srcImageHandle = mSwapChainImages[mCurrentFrame].GetImageHandle();
	auto srcImageProp = mSwapChainImages[mCurrentFrame].GetImageProperty();
	auto& srcVulkanImage = mSwapChainImages[mCurrentFrame];
	auto srcImageExtent = srcVulkanImage.GetImageExtent();
    vk::ImageSubresourceLayers Layer(srcImageProp.mImageAspect, 0, 0, srcImageProp.mArrayLayers);
	vk::BufferImageCopy CopyRegions(0, 0, 0, Layer, vk::Offset3D{}, vk::Extent3D(srcImageExtent, 1));

	srcVulkanImage.CmdTransitionImageLayout(CommandBuffer,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eTransferSrcOptimal,
		vk::PipelineStageFlagBits::eLateFragmentTests,
		vk::PipelineStageFlagBits::eTransfer,
		vk::AccessFlagBits::eNone,
		vk::AccessFlagBits::eMemoryRead);

	Cmd.copyImageToBuffer(srcImageHandle, vk::ImageLayout::eTransferSrcOptimal, inbuffer.GetBufferHandle(), CopyRegions);

	srcVulkanImage.CmdTransitionImageLayout(CommandBuffer,
		vk::ImageLayout::eTransferSrcOptimal,
		vk::ImageLayout::ePresentSrcKHR,
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eLateFragmentTests,
		vk::AccessFlagBits::eMemoryRead,
		vk::AccessFlagBits::eMemoryRead
	);
}

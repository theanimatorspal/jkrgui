#include "Shape.hpp"
#include "VulkanImage.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include <glm/ext/matrix_clip_space.hpp>

Jkr::Renderer::Shape::Shape(const Instance& inInstance, Window& inCompatibleWindow, std::unordered_map<FillType, Up<PainterCache>>& inPainterCaches) : mInstance(inInstance), mPainterCaches(inPainterCaches)
{
	mPainters[FillType::Fill] = MakeUp<Painter>(inInstance, inCompatibleWindow, *inPainterCaches[FillType::Fill]);

#ifndef JKR_USE_VARIABLE_DES_INDEXING
	mPainters[FillType::Image] = MakeUp<Painter>(inInstance, inCompatibleWindow, *inPainterCaches[FillType::Image]);
#else
	mPainters[FillType::Image] = MakeUp<Painter>(inInstance, inCompatibleWindow, *inPainterCaches[FillType::Image], JKR_VAR_DES_2D_IMG_COUNT);
	mVarDesVulkanDescriptorSet = MakeUp<VulkanDescriptorSet>(mInstance.GetDevice(), mInstance.GetDescriptorPool(), mPainterCaches[FillType::Image]->GetVertexFragmentDescriptorSetLayout(), JKR_VAR_DES_2D_IMG_COUNT);
#endif

	mPainters[FillType::ContinousLine] = MakeUp<Painter>(inInstance, inCompatibleWindow, *inPainterCaches[FillType::ContinousLine]);
#ifndef JKR_NO_STAGING_BUFFERS
	rb::CreateStagingBuffers(
		inInstance,
		sb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
		sb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold)
	);
#endif
	mPrimitive = MakeUp<Primitive>(inInstance, sb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold), sb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
}

void Jkr::Renderer::Shape::Add(Jkr::Generator& inShape, int inX, int inY, uint32_t inZ, uint32_t& outId)
{
	CheckAndResize(inShape);
	sb::Add(inShape, inX, inY, inZ, outId);
	const auto OffsetId = (uint32_t)outId;
#ifndef JKR_NO_STAGING_BUFFERS
	rb::CopyToStagingBuffers(
		sb::GetVertexBufferData(),
		sb::GetIndexBufferData(),
		sb::VertexCountToBytes(sb::GetVertexOffsetAbsolute(OffsetId)),
		sb::IndexCountToBytes(sb::GetIndexOffsetAbsolute(OffsetId)),
		sb::VertexCountToBytes(inShape.GetVertexCount()),
		sb::IndexCountToBytes(inShape.GetIndexCount())
	);
	rb::RegisterBufferCopyRegionToPrimitiveFromStaging(
		sb::VertexCountToBytes(sb::GetVertexOffsetAbsolute(OffsetId)),
		sb::IndexCountToBytes(sb::GetIndexOffsetAbsolute(OffsetId)),
		sb::VertexCountToBytes(inShape.GetVertexCount()),
		sb::IndexCountToBytes(inShape.GetIndexCount())
	);
#else
	rb::CopyToPrimitive(*mPrimitive,
		sb::GetVertexBufferData(),
		sb::GetIndexBufferData(),
		sb::VertexCountToBytes(sb::GetVertexOffsetAbsolute(OffsetId)),
		sb::IndexCountToBytes(sb::GetIndexOffsetAbsolute(OffsetId)),
		sb::VertexCountToBytes(inShape.GetVertexCount()),
		sb::IndexCountToBytes(inShape.GetIndexCount())
	);
#endif
}

void Jkr::Renderer::Shape::AddImage(const std::string_view inFileName, uint32_t& outIndex)
{
#ifndef JKR_USE_VARIABLE_DES_INDEXING
	Up<VulkanDescriptorSet> Desset = MakeUp<VulkanDescriptorSet>(mInstance.GetDevice(), mInstance.GetDescriptorPool(), mPainterCaches[FillType::Image]->GetVertexFragmentDescriptorSetLayout());
	Up<ImageType> Image = MakeUp<ImageType>(mInstance);
	Image->Setup(inFileName);
	Image->Register(
		0, 0, 0, *Desset
	);
	outIndex = mImages.size();
	mImages.push_back(std::move(Image));
	mVulkanPerImageDescriptorSets.push_back(std::move(Desset));
#else 
	Up<ImageType> Image = MakeUp<ImageType>(mInstance);
	Image->Setup(inFileName);
	uint32_t CurrentIndex = mImages.size();
	Image->Register(
		0, 0, CurrentIndex, *mVarDesVulkanDescriptorSet
	);
	outIndex = mImages.size();
	mImages.push_back(std::move(Image));
#endif
}

void Jkr::Renderer::Shape::AddImage(uint32_t inWidth, uint32_t inHeight, uint32_t &outIndex)
{
#ifndef JKR_USE_VARIABLE_DES_INDEXING
    Up<VulkanDescriptorSet> Desset = MakeUp<VulkanDescriptorSet>(
        mInstance.GetDevice(),
        mInstance.GetDescriptorPool(),
        mPainterCaches[FillType::Image]->GetVertexFragmentDescriptorSetLayout());
    Up<ImageType> Image = MakeUp<ImageType>(mInstance);
    std::vector<uint8_t> image;
    image.resize(inWidth * inHeight * 4);
    Image->Setup(reinterpret_cast<void **>(&image.data()), inWidth, inHeight, 4);
    Image->Register(0, 0, 0, *Desset);
    outIndex = mImages.size();
    mImages.push_back(std::move(Image));
    mVulkanPerImageDescriptorSets.push_back(std::move(Desset));
#else
    Up<ImageType> Image = MakeUp<ImageType>(mInstance);
    std::vector<uint8_t> image;
    image.resize(inWidth * inHeight * 4);
    void *data = image.data();
    Image->Setup(&data, inWidth, inHeight, 4);
    uint32_t CurrentIndex = mImages.size();
    Image->Register(0, 0, CurrentIndex, *mVarDesVulkanDescriptorSet);
    outIndex = mImages.size();
    mImages.push_back(std::move(Image));
#endif
}

void Jkr::Renderer::Shape::CopyToImage(uint32_t inId,
                                       uint32_t inWidth,
                                       uint32_t inHeight,
                                       Jkr::Renderer::CustomImagePainter &inPainter)
{
    mImagesToBeCopiedIds.push_back(inId);
    mImagesToBeCopiedFrom.push_back(inPainter.GetImagePtr());

#ifdef JKR_USE_VARIABLE_DES_INDEXING
    mImageCopyCommands.push_back([&](const ksai::VulkanCommandBuffer &inCmd,
                                     ksai::VulkanImageBase &inSrcImage,
                                     ksai::VulkanImageBase &inDstImage) {
        auto srcImgProp = inSrcImage.GetImageProperty();
        auto dstImgProp = inDstImage.GetImageProperty();
        vk::ImageSubresourceLayers SrcSubresource(srcImgProp.mImageAspect,
                                                  0,
                                                  0,
                                                  srcImgProp.mArrayLayers);
        vk::ImageSubresourceLayers DstSubresource(dstImgProp.mImageAspect,
                                                  0,
                                                  0,
                                                  dstImgProp.mArrayLayers);
        vk::ImageCopy ImageCopy(SrcSubresource,
                                vk::Offset3D(0, 0, 0),
                                DstSubresource,
                                vk::Offset3D(0, 0, 0),
                                vk::Extent3D(srcImgProp.mExtent, 1));

        vk::ImageSubresourceRange src_SubRange(SrcSubresource.aspectMask,
                                               0,
                                               srcImgProp.mMipLevels,
                                               0,
                                               srcImgProp.mArrayLayers);

        /* Source Image From General To Src Optimal  */
        vk::ImageMemoryBarrier src_Barrier(vk::AccessFlagBits::eMemoryWrite,
                                           vk::AccessFlagBits::eMemoryRead,
                                           vk::ImageLayout::eGeneral,
                                           vk::ImageLayout::eTransferSrcOptimal,
                                           VK_QUEUE_FAMILY_IGNORED,
                                           VK_QUEUE_FAMILY_IGNORED,
                                           inSrcImage.GetImageHandle(),
                                           src_SubRange);
        inCmd.GetCommandBufferHandle().pipelineBarrier(vk::PipelineStageFlagBits::eComputeShader,
                                                       vk::PipelineStageFlagBits::eTransfer,
                                                       vk::DependencyFlagBits::eByRegion,
                                                       {},
                                                       {},
                                                       src_Barrier);

        /* Destination Image From General To Dst Optimal  */
        inDstImage.CmdTransitionImageLayout(inCmd,
                                            vk::ImageLayout::eGeneral,
                                            vk::ImageLayout::eTransferDstOptimal,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            vk::AccessFlagBits::eNone,
                                            vk::AccessFlagBits::eMemoryWrite);

        inCmd.GetCommandBufferHandle().copyImage(inSrcImage.GetImageHandle(),
                                                 vk::ImageLayout::eTransferSrcOptimal,
                                                 inDstImage.GetImageHandle(),
                                                 vk::ImageLayout::eTransferDstOptimal,
                                                 ImageCopy);

        /* Destination Image From Dst Optimal To General  */
        vk::ImageSubresourceRange dst_SubRange(DstSubresource.aspectMask,
                                               0,
                                               dstImgProp.mMipLevels,
                                               0,
                                               dstImgProp.mArrayLayers);
        vk::ImageMemoryBarrier dst_Barrier(vk::AccessFlagBits::eMemoryWrite,
                                           vk::AccessFlagBits::eMemoryRead,
                                           vk::ImageLayout::eTransferDstOptimal,
                                           vk::ImageLayout::eGeneral,
                                           VK_QUEUE_FAMILY_IGNORED,
                                           VK_QUEUE_FAMILY_IGNORED,
                                           inDstImage.GetImageHandle(),
                                           dst_SubRange);
        inCmd.GetCommandBufferHandle().pipelineBarrier(vk::PipelineStageFlagBits::eTransfer,
                                                       vk::PipelineStageFlagBits::eFragmentShader,
                                                       vk::DependencyFlagBits::eByRegion,
                                                       {},
                                                       {},
                                                       dst_Barrier);

        /* Source Image From Src Optimal To General  */
        inSrcImage.CmdTransitionImageLayout(inCmd,
                                            vk::ImageLayout::eTransferSrcOptimal,
                                            vk::ImageLayout::eGeneral,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            vk::AccessFlagBits::eMemoryRead,
                                            vk::AccessFlagBits::eNone);
    });

#else
/*TODO: Yet To implement */
#endif
}

void Jkr::Renderer::Shape::Update(uint32_t inId, Jkr::Generator& inShape, int inX, int inY, uint32_t inZ)
{
	auto OffsetId = inId;
	sb::Update(inShape, inId, inX, inY, inZ);
#ifndef JKR_NO_STAGING_BUFFERS
	rb::CopyToStagingBuffers(
		sb::GetVertexBufferData(),
		sb::GetIndexBufferData(),
		sb::VertexCountToBytes(sb::GetVertexOffsetAbsolute(OffsetId)),
		sb::IndexCountToBytes(sb::GetIndexOffsetAbsolute(OffsetId)),
		sb::VertexCountToBytes(inShape.GetVertexCount()),
		sb::IndexCountToBytes(inShape.GetIndexCount())
	);
	rb::RegisterBufferCopyRegionToPrimitiveFromStaging(
		sb::VertexCountToBytes(sb::GetVertexOffsetAbsolute(OffsetId)),
		sb::IndexCountToBytes(sb::GetIndexOffsetAbsolute(OffsetId)),
		sb::VertexCountToBytes(inShape.GetVertexCount()),
		sb::IndexCountToBytes(inShape.GetIndexCount())
	);
#else
	rb::CopyToPrimitive(*mPrimitive,
		sb::GetVertexBufferData(),
		sb::GetIndexBufferData(),
		sb::VertexCountToBytes(sb::GetVertexOffsetAbsolute(OffsetId)),
		sb::IndexCountToBytes(sb::GetIndexOffsetAbsolute(OffsetId)),
		sb::VertexCountToBytes(inShape.GetVertexCount()),
		sb::IndexCountToBytes(inShape.GetIndexCount())
	);
#endif
}

void Jkr::Renderer::Shape::Dispatch(Window& inWindow)
{
#ifndef JKR_NO_STAGING_BUFFERS
	if (!rb::IsCopyRegionsEmpty())
	{
		rb::CmdCopyToPrimitiveFromStagingBuffer(
			mInstance,
			*mPrimitive,
			inWindow,
			sb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
			sb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold)
		);
	}
#endif

    for (uint32_t i = 0; i < mImagesToBeCopiedIds.size(); i++) {
        auto &func = mImageCopyCommands[i];
        auto id = mImagesToBeCopiedIds[i];
        auto &srcImage = mImagesToBeCopiedFrom[i];

        const ksai::VulkanCommandBuffer &cmd = inWindow
                                                   .GetCommandBuffers()[inWindow.GetCurrentFrame()];
        func(cmd, *srcImage, mImages[id]->GetUniformImage());
    }
    if (mImagesToBeCopiedIds.size() != 0) {
        mImagesToBeCopiedFrom.clear();
        mImageCopyCommands.clear();
        mImagesToBeCopiedIds.clear();
    }
}

void Jkr::Renderer::Shape::BindFillMode(FillType inFillType, Window& inWindow)
{
#ifdef JKR_USE_VARIABLE_DES_INDEXING
    auto &Cmd = inWindow.GetCommandBuffers()[inWindow.GetCurrentFrame()];
    Cmd.GetCommandBufferHandle().bindDescriptorSets(
		vk::PipelineBindPoint::eGraphics,
		mPainterCaches[FillType::Image]->GetVertexFragmentPipelineLayout().GetPipelineLayoutHandle(),
		0,
		mVarDesVulkanDescriptorSet->GetDescriptorSetHandle(),
		{}
	);
#endif
	mPainters[inFillType]->BindDrawParamtersPipelineOnly_EXT<PushConstant>(*mPrimitive, inWindow);
	mCurrentFillMode = inFillType;
}

void Jkr::Renderer::Shape::BindImage(Window& inWindow, uint32_t inImageId)
{
	// TODO Remove
	if (inImageId != -1)
	{
#ifndef JKR_USE_VARIABLE_DES_INDEXING
		auto& Cmd = mInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()];
		Cmd.GetCommandBufferHandle().bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			mPainterCaches[FillType::Image]->GetVertexFragmentPipelineLayout().GetPipelineLayoutHandle(),
			0,
			mVulkanPerImageDescriptorSets[inImageId]->GetDescriptorSetHandle(),
			{}
		);
#else
		mCurrentImageIndex = inImageId;
#endif
	}
}


void Jkr::Renderer::Shape::BindShapes(Window& inWindow)
{
	Painter::BindDrawParamtersVertexAndIndexBuffersOnly_EXT<PushConstant>(mInstance, *mPrimitive, inWindow);
}

void Jkr::Renderer::Shape::Draw(Window& inWindow, glm::vec4 inColor, uint32_t inWindowW, uint32_t inWindowH, uint32_t inStartShapeId, uint32_t inEndShapeId, glm::mat4 inMatrix)
{
	glm::mat4 Matrix = glm::ortho(
		0.0f,
		static_cast<float>(inWindowW),
		0.0f,
		static_cast<float>(inWindowH),
		100.0f,
		-100.0f
	) * inMatrix;
	PushConstant Push;
	Push.mColor = inColor;
	Push.mMatrix = Matrix;

#ifdef JKR_USE_VARIABLE_DES_INDEXING
	Push.mParams.r = mCurrentImageIndex;
#endif


	mPainters[mCurrentFillMode]->Draw_EXT<PushConstant>(
		*mPrimitive,
		Push,
		inWindow,
		sb::GetEndIndexOffsetAbsolute(inEndShapeId) - sb::GetIndexOffsetAbsolute(inStartShapeId),//TODO
		1,
		sb::GetIndexOffsetAbsolute(inStartShapeId),
		0
	);
}

void Jkr::Renderer::Shape::CheckAndResize(const Jkr::Generator& inShape)
{
	bool ResizeRequired = false;
	while (true)
	{
		bool ResizeRequiredi =
			(inShape.GetIndexCount() + sb::GetCurrentIndexOffset() >= mTotalNoOfIndicesRendererCanHold) ||
			(inShape.GetVertexCount() + sb::GetCurrentVertexOffset() >= mTotalNoOfVerticesRendererCanHold);
		if (ResizeRequiredi)
		{
			mTotalNoOfVerticesRendererCanHold *= rb::RendererCapacityResizeFactor;
			mTotalNoOfIndicesRendererCanHold *= rb::RendererCapacityResizeFactor;
			ResizeRequired = true;
		}
		else {
			break;
		}
	}

	if (ResizeRequired)
	{
		sb::Resize(mTotalNoOfVerticesRendererCanHold, mTotalNoOfIndicesRendererCanHold);
		mPrimitive.reset();
		mPrimitive = MakeUp<Primitive>(
			mInstance,
			sb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
			sb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold)
		);
#ifndef JKR_NO_STAGING_BUFFERS
		rb::ResizeStagingBuffer(
			mInstance,
			sb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
			sb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold)
		);
		rb::CopyToStagingBuffers(
			sb::GetVertexBufferData(),
			sb::GetIndexBufferData(),
			sb::VertexCountToBytes(0),
			sb::IndexCountToBytes(0),
			sb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
			sb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold)
		);
		rb::RegisterBufferCopyRegionToPrimitiveFromStaging(
			sb::VertexCountToBytes(0),
			sb::IndexCountToBytes(0),
			sb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
			sb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold)
		);
#else
		rb::CopyToPrimitive(*mPrimitive,
			sb::GetVertexBufferData(),
			sb::GetIndexBufferData(),
			sb::VertexCountToBytes(0),
			sb::IndexCountToBytes(0),
			sb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
			sb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold)
		);
#endif
	}
}

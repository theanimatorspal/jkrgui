#include "Shape.hpp"
#include <glm/ext/matrix_clip_space.hpp>

Jkr::Renderer::Shape::Shape(const Instance& inInstance, Window& inCompatibleWindow, std::unordered_map<FillType, Up<PainterCache>>& inPainterCaches) : mInstance(inInstance), mPainterCaches(inPainterCaches)
{
	mPainters[FillType::Fill] = MakeUp<Painter>(inInstance, inCompatibleWindow, *inPainterCaches[FillType::Fill]);
	mPainters[FillType::Image] = MakeUp<Painter>(inInstance, inCompatibleWindow, *inPainterCaches[FillType::Image]);
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

void Jkr::Renderer::Shape::Add(Jkr::Generator& inShape, uint32_t inX, uint32_t inY, uint32_t inZ, uint32_t& outId)
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
	Up<VulkanDescriptorSet> Desset = MakeUp<VulkanDescriptorSet>(mInstance.GetDevice(), mInstance.GetDescriptorPool(), mPainterCaches[FillType::Image]->GetVertexFragmentDescriptorSetLayout());
	Up<ImageType> Image = MakeUp<ImageType>(mInstance);
	Image->Setup(inFileName);
	Image->Register(
		0, 0, 0, *Desset
	);
	outIndex = mImages.size();
	mImages.push_back(std::move(Image));
	mVulkanPerImageDescriptorSets.push_back(std::move(Desset));
}

void Jkr::Renderer::Shape::Update(uint32_t inId, Jkr::Generator& inShape, uint32_t inX, uint32_t inY, uint32_t inZ)
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
#else

#endif
}

void Jkr::Renderer::Shape::BindFillMode(FillType inFillType, Window& inWindow)
{
	mPainters[inFillType]->BindDrawParamtersPipelineOnly_EXT<PushConstant>(*mPrimitive, inWindow);
	mCurrentFillMode = inFillType;
}

void Jkr::Renderer::Shape::BindImage(Window& inWindow, uint32_t inImageId)
{
	if (inImageId != -1)
	{
		auto& Cmd = mInstance.GetCommandBuffers()[inWindow.GetCurrentFrame()];
		Cmd.GetCommandBufferHandle().bindDescriptorSets(
			vk::PipelineBindPoint::eGraphics,
			mPainterCaches[FillType::Image]->GetVertexFragmentPipelineLayout().GetPipelineLayoutHandle(),
			0,
			mVulkanPerImageDescriptorSets[inImageId]->GetDescriptorSetHandle(),
			{}
		);
	}
}


void Jkr::Renderer::Shape::BindShapes(FillType inFillType, Window& inWindow)
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
	bool ResizeRequired =
		(inShape.GetIndexCount() + sb::GetCurrentIndexOffset() >= mTotalNoOfIndicesRendererCanHold) ||
		(inShape.GetVertexCount() + sb::GetCurrentVertexOffset() >= mTotalNoOfVerticesRendererCanHold);
	if (ResizeRequired)
	{
		mTotalNoOfVerticesRendererCanHold *= rb::RendererCapacityResizeFactor;
		mTotalNoOfIndicesRendererCanHold *= rb::RendererCapacityResizeFactor;
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

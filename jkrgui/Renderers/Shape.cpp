#include "Shape.hpp"
#include <glm/ext/matrix_clip_space.hpp>

Jkr::Renderer::Shape::Shape(const Instance& inInstance, Window& inCompatibleWindow, std::unordered_map<FillType, Up<PainterCache>>& inPainterCaches) : mInstance(inInstance), mPainterCaches(inPainterCaches)
{
	mPainters[FillType::Fill] = MakeUp<Painter>(inInstance, inCompatibleWindow, *inPainterCaches[FillType::Fill]);
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

void Jkr::Renderer::Shape::DrawInit(Window& inWindow, FillType inFillType)
{
	mPainters[inFillType]->BindDrawParamters_EXT<PushConstant>(*mPrimitive, inWindow);
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

	mPainters[FillType::Fill]->Draw_EXT<PushConstant>(
		*mPrimitive,
		Push,
		inWindow,
		sb::GetEndIndexOffsetAbsolute(inEndShapeId),//TODO
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
		mTotalNoOfVerticesRendererCanHold *= 2;
		mTotalNoOfIndicesRendererCanHold *= 2;
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
			sb::LineCountToVertexBytes(0),
			sb::LineCountToIndexBytes(0),
			sb::LineCountToVertexBytes(mTotalNoOfLinesRendererCanHold),
			sb::LineCountToIndexBytes(mTotalNoOfLinesRendererCanHold)
		);
#endif
	}
}

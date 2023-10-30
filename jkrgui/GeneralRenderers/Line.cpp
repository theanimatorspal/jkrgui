#include "Line.hpp"

Jkr::Renderer::Line::Line(const Instance& inInstance, GUIWindow& inCompatibleWindow, GUIPainterCache& inPainterCache)
	: mInstance(inInstance)
{
	mPainter = MakeUp<GUIPainter>(inInstance, inCompatibleWindow, inPainterCache);
	rb::CreateStagingBuffers(
		inInstance,
		lb::LineCountToVertexBytes(mTotalNoOfLinesRendererCanHold),
		lb::LineCountToIndexBytes(mTotalNoOfLinesRendererCanHold)
	);
	mPrimitive = MakeUp<GUIPrimitive>(
		inInstance,
		lb::LineCountToVertexBytes(mTotalNoOfLinesRendererCanHold),
		lb::LineCountToIndexBytes(mTotalNoOfLinesRendererCanHold)
	);
}

void Jkr::Renderer::Line::AddLine(glm::vec2 inFirstPoint, glm::vec2 inSecondPoint, float inDepthValue, uint32_t& outId)
{
	CheckAndResize(mInstance, 1 + GetCurrentLineOffset());
	lb::AddLine(inFirstPoint, inSecondPoint, inDepthValue);
	rb::CopyToStagingBuffers(
		lb::GetVertexBufferData(),
		lb::GetIndexBufferData(),
		lb::LineCountToVertexBytes(lb::GetCurrentLineOffset()),
		lb::LineCountToIndexBytes(lb::GetCurrentLineOffset()),
		lb::LineCountToVertexBytes(1),
		lb::LineCountToIndexBytes(1)
	);
	rb::RegisterBufferCopyRegionToPrimitiveFromStagingFirstFrameOnly(
		lb::LineCountToVertexBytes(lb::GetCurrentLineOffset()),
		lb::LineCountToIndexBytes(lb::GetCurrentLineOffset()),
		lb::LineCountToVertexBytes(1),
		lb::LineCountToIndexBytes(1)
	);
	outId = lb::GetCurrentLineOffset();
}

void Jkr::Renderer::Line::UpdateLine(uint32_t inId, glm::vec2 inFirstPoint, glm::vec2 inSecondPoint, float inDepthValue)
{
	auto LineOffset = inId;
	lb::UpdateLine(inId, inFirstPoint, inSecondPoint, inDepthValue);
	rb::CopyToStagingBuffers(
		lb::GetVertexBufferData(),
		lb::GetIndexBufferData(),
		lb::LineCountToVertexBytes(LineOffset),
		lb::LineCountToIndexBytes(LineOffset),
		lb::LineCountToVertexBytes(1),
		lb::LineCountToIndexBytes(1)
	);
	rb::RegisterBufferCopyRegionToPrimitiveFromStagingFirstFrameOnly(
		lb::LineCountToVertexBytes(LineOffset),
		lb::LineCountToIndexBytes(LineOffset),
		lb::LineCountToVertexBytes(1),
		lb::LineCountToIndexBytes(1)
	);
}

void Jkr::Renderer::Line::Dispatch(GUIWindow& inWindow)
{
	if (!rb::IsSingleTimeCopyRegionsEmpty())
	{
		rb::CmdCopyToPrimitiveFromStagingBufferSingleTime(
			mInstance,
			*mPrimitive,
			inWindow,
			lb::LineCountToVertexBytes(mTotalNoOfLinesRendererCanHold),
			lb::LineCountToIndexBytes(mTotalNoOfLinesRendererCanHold)
		);
	}
}

void Jkr::Renderer::Line::DrawInit(GUIWindow& inWindow)
{
	mPainter->BindDrawParamters_EXT<PushConstant>(*mPrimitive, inWindow);
}

void Jkr::Renderer::Line::DrawBatched(GUIWindow& inWindow, glm::vec4 inColor, uint32_t inWindowW, uint32_t inWindowH, uint32_t inLineOffset, uint32_t inNoOfLines)
{
	glm::mat4 Matrix = glm::ortho(
		0.0f,
		static_cast<float>(inWindowW),
		0.0f,
		static_cast<float>(inWindowH),
		100.0f,
		-100.0f
	);
	PushConstant Push;
	Push.mColor = inColor;
	Push.mMatrix = Matrix;

	mPainter->Draw_EXT<PushConstant>(
			*mPrimitive,
			Push,
			inWindow,
			2 * inNoOfLines,
			1,
			inLineOffset,
			0
		);
}

void Jkr::Renderer::Line::DrawSingle(GUIWindow& inWindow, glm::vec4 inColor, uint32_t inWindowW, uint32_t inWindowH, uint32_t inId)
{
	glm::mat4 Matrix = glm::ortho(
		0.0f,
		static_cast<float>(inWindowW),
		0.0f,
		static_cast<float>(inWindowH),
		100.0f,
		-100.0f
	);
	PushConstant Push;
	Push.mColor = inColor;
	Push.mMatrix = Matrix;

	mPainter->Draw_EXT<PushConstant>(
			*mPrimitive,
			Push,
			inWindow,
			2,
			1,
			inId * 2,
			0
		);

}

void Jkr::Renderer::Line::CheckAndResize(const Instance& inInstance, uint32_t inNewSizeNeeded)
{
	if (inNewSizeNeeded >= mTotalNoOfLinesRendererCanHold)
	{
		mTotalNoOfLinesRendererCanHold *= 2;
		mPrimitive.reset();
		mPrimitive = MakeUp<GUIPrimitive>(
			inInstance,
			lb::LineCountToVertexBytes(mTotalNoOfLinesRendererCanHold),
			lb::LineCountToIndexBytes(mTotalNoOfLinesRendererCanHold)
		);
		rb::ResizeStagingBuffer(
			inInstance,
			lb::LineCountToVertexBytes(mTotalNoOfLinesRendererCanHold),
			lb::LineCountToIndexBytes(mTotalNoOfLinesRendererCanHold)
		);
		rb::CopyToStagingBuffers(
			lb::GetVertexBufferData(),
			lb::GetIndexBufferData(),
			lb::LineCountToVertexBytes(0),
			lb::LineCountToIndexBytes(0),
			lb::LineCountToVertexBytes(mTotalNoOfLinesRendererCanHold),
			lb::LineCountToIndexBytes(mTotalNoOfLinesRendererCanHold)
		);
		rb::RegisterBufferCopyRegionToPrimitiveFromStagingFirstFrameOnly(
			lb::LineCountToVertexBytes(0),
			lb::LineCountToIndexBytes(0),
			lb::LineCountToVertexBytes(mTotalNoOfLinesRendererCanHold),
			lb::LineCountToIndexBytes(mTotalNoOfLinesRendererCanHold)
		);

	}
}


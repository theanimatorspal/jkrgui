#include "FastText.hpp"

using namespace Jkr::Renderer;
using fb = FastText_base;
using rb = Renderer_base;

FastText::FastText(const Instance& inInstance, const Window& inCompatibleWindow, const PainterCache& inCache)
	: FastText_base("res/ksai_fast_text/Font32.fnt"), mInstance(inInstance)
{
	mImage = MakeUp<ImageType>(inInstance);
	mImage->Setup("res/ksai_fast_text/Font32.png");
	mPainter = MakeUp<Painter>(inInstance, inCompatibleWindow, inCache);
	mPainter->RegisterPainterParameter(*mImage, 0, 0, 0);

#ifndef JKR_NO_STAGING_BUFFERS
	rb::CreateStagingBuffers(
		inInstance,
		fb::CharCountToVertexBytes(mTotalNoOfCharsRendererCanHold),
		fb::CharCountToIndexBytes(mTotalNoOfCharsRendererCanHold)
	);
#endif

	mPrimitive = MakeUp<Primitive>(
		inInstance,
		fb::CharCountToVertexBytes(mTotalNoOfCharsRendererCanHold),
		fb::CharCountToIndexBytes(mTotalNoOfCharsRendererCanHold)
	);
}

fb::TextDimensions Jkr::Renderer::FastText::AddText(const std::string_view inText, uint32_t inX, uint32_t inY, uint32_t inDepthValue, uint32_t& outId)
{
	auto NoOfNewCharsNeeded = strlen(inText.data());
	CheckAndResize(NoOfNewCharsNeeded);
	auto TextDimension = fb::AddText(inText, inX, inY, inDepthValue, outId);
#ifndef JKR_NO_STAGING_BUFFERS
	rb::CopyToStagingBuffers(
		fb::GetVertexBufferData(),
		fb::GetIndexBufferData(),
		fb::CharCountToVertexBytes(fb::GetCurrentCharOffset(inText)),
		fb::CharCountToIndexBytes(fb::GetCurrentCharOffset(inText)),
		fb::CharCountToVertexBytes(1 * inText.size()),
		fb::CharCountToIndexBytes(1 * inText.size())
	);

	rb::RegisterBufferCopyRegionToPrimitiveFromStaging(
		fb::CharCountToVertexBytes(fb::GetCurrentCharOffset(inText)),
		fb::CharCountToIndexBytes(fb::GetCurrentCharOffset(inText)),
		fb::CharCountToVertexBytes(1 * inText.size()),
		fb::CharCountToIndexBytes(1 * inText.size())
	);
#else
	rb::CopyToPrimitive(*mPrimitive,
		fb::GetVertexBufferData(),
		fb::GetIndexBufferData(),
		fb::CharCountToVertexBytes(fb::GetCurrentCharOffset(inText)),
		fb::CharCountToIndexBytes(fb::GetCurrentCharOffset(inText)),
		fb::CharCountToVertexBytes(1 * inText.size()),
		fb::CharCountToIndexBytes(1 * inText.size())
	);
#endif

	return TextDimension;
}

fb::TextDimensions Jkr::Renderer::FastText::UpdateText(uint32_t inId, const std::string_view inText, uint32_t inX, uint32_t inY, uint32_t inDepthValue)
{
	auto Offset = inId;
	auto TextDimension = fb::UpdateText(inId, inText, inX, inY, inDepthValue);
#ifndef JKR_NO_STAGING_BUFFERS
	rb::CopyToStagingBuffers(
		fb::GetVertexBufferData(),
		fb::GetIndexBufferData(),
		fb::CharCountToVertexBytes(Offset),
		fb::CharCountToIndexBytes(Offset),
		fb::CharCountToVertexBytes(inText.size()),
		fb::CharCountToIndexBytes(inText.size())
	);
	rb::RegisterBufferCopyRegionToPrimitiveFromStaging(
		fb::CharCountToVertexBytes(Offset),
		fb::CharCountToIndexBytes(Offset),
		fb::CharCountToVertexBytes(inText.size()),
		fb::CharCountToIndexBytes(inText.size())
	);
#else
	rb::CopyToPrimitive(*mPrimitive,
		fb::GetVertexBufferData(),
		fb::GetIndexBufferData(),
		fb::CharCountToVertexBytes(Offset),
		fb::CharCountToIndexBytes(Offset),
		fb::CharCountToVertexBytes(inText.size()),
		fb::CharCountToIndexBytes(inText.size())
	);
#endif
	return TextDimension;
}

void Jkr::Renderer::FastText::Dispatch(Window& inWindow)
{
#ifndef JKR_NO_STAGING_BUFFERS
	if (!rb::IsCopyRegionsEmpty())
	{
		rb::CmdCopyToPrimitiveFromStagingBuffer(
			mInstance,
			*mPrimitive,
			inWindow,
			fb::CharCountToVertexBytes(mTotalNoOfCharsRendererCanHold),
			fb::CharCountToIndexBytes(mTotalNoOfCharsRendererCanHold)
		);
	}
#endif
}

void Jkr::Renderer::FastText::Bind(Window& inWindow)
{
	mPainter->BindDrawParamters_EXT<PushConstant>(*mPrimitive, inWindow);
}

void Jkr::Renderer::FastText::Draw(Window& inWindow, glm::vec4 inColor, uint32_t inWindowW, uint32_t inWindowH, uint32_t inStartTextId, uint32_t inNoOfChars, glm::mat4 inMatrix)
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

	mPainter->Draw_EXT<PushConstant>(
		*mPrimitive,
		Push,
		inWindow,
		6 * inNoOfChars,
		1,
		inStartTextId * 6,
		0
	);
}

void Jkr::Renderer::FastText::CheckAndResize(size_t inNewSize)
{

	bool ResizeRequired = false;
	while (true)
	{
		bool ResizeRequiredi = (inNewSize + GetCurrentCharOffsetAbsolute() >= mTotalNoOfCharsRendererCanHold)
			|| inNewSize >= mTotalNoOfCharsRendererCanHold;
		if (ResizeRequiredi)
		{
			mTotalNoOfCharsRendererCanHold *= rb::RendererCapacityResizeFactor;
			ResizeRequired = true;
		}
		else {
			break;
		}
	}

	if (ResizeRequired)
	{
		mTotalNoOfCharsRendererCanHold *= rb::RendererCapacityResizeFactor;
		fb::Resize(mTotalNoOfCharsRendererCanHold);
		mPrimitive.reset();
		mPrimitive = MakeUp<Primitive>(
			mInstance,
			fb::CharCountToVertexBytes(mTotalNoOfCharsRendererCanHold),
			fb::CharCountToIndexBytes(mTotalNoOfCharsRendererCanHold)
		);
#ifndef JKR_NO_STAGING_BUFFERS
		rb::ResizeStagingBuffer(
			mInstance,
			fb::CharCountToVertexBytes(mTotalNoOfCharsRendererCanHold),
			fb::CharCountToIndexBytes(mTotalNoOfCharsRendererCanHold)
		);
		rb::CopyToStagingBuffers(
			fb::GetVertexBufferData(),
			fb::GetIndexBufferData(),
			fb::CharCountToVertexBytes(0),
			fb::CharCountToIndexBytes(0),
			fb::CharCountToVertexBytes(mTotalNoOfCharsRendererCanHold),
			fb::CharCountToIndexBytes(mTotalNoOfCharsRendererCanHold)
		);
		rb::RegisterBufferCopyRegionToPrimitiveFromStaging(
			fb::CharCountToVertexBytes(0),
			fb::CharCountToIndexBytes(0),
			fb::CharCountToVertexBytes(mTotalNoOfCharsRendererCanHold),
			fb::CharCountToIndexBytes(mTotalNoOfCharsRendererCanHold)
		);
#else
		rb::CopyToPrimitive(*mPrimitive,
			fb::GetVertexBufferData(),
			fb::GetIndexBufferData(),
			fb::CharCountToVertexBytes(0),
			fb::CharCountToIndexBytes(0),
			fb::CharCountToVertexBytes(mTotalNoOfCharsRendererCanHold),
			fb::CharCountToIndexBytes(mTotalNoOfCharsRendererCanHold)
		);
#endif
}
}

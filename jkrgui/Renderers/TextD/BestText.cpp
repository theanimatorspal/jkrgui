#include "BestText.hpp"

Jkr::Renderer::BestText::BestText ( const Instance& inInstance, const Window& inCompatibleWindow, const PainterCache& inCache )
	: mInstance ( inInstance )
{
	mPainter = MakeUp<Painter> ( inInstance, inCompatibleWindow, inCache, 5000 );

#ifndef JKR_NO_STAGING_BUFFERS
	rb::CreateStagingBuffers (
		inInstance,
		bb::CharCountToVertexBytes ( mTotalNoOfCharsRendererCanHold ),
		bb::CharCountToIndexBytes ( mTotalNoOfCharsRendererCanHold )
	);
#endif

	mPrimitive = MakeUp<Primitive> (
		inInstance,
		bb::CharCountToVertexBytes ( mTotalNoOfCharsRendererCanHold ),
		bb::CharCountToIndexBytes ( mTotalNoOfCharsRendererCanHold )
	);
}

Jkr::Renderer::BestText_base::TextDimensions Jkr::Renderer::BestText::AddText ( const std::string_view inText, uint32_t inX, uint32_t inY, uint32_t inDepthValue, uint32_t& outId )
{
	std::vector<uint32_t> CodePoints;
	uint32_t len;
	auto TextDimension = bb::AddText ( inText, mCurrentFontFaceId, inDepthValue, CodePoints, outId );

	uint32_t VertexStartingIndex = outId;
	for (int i = 0; i < CodePoints.size ( ); i++)
	{
		uint32_t id;
		if (mCodepointKeyTextureMap.contains ( CodePoints[i] ))
		{
			id = mCodepointKeyTextureMap[CodePoints[i]];
		}
		else {
			Up<ImageType> Image = MakeUp<ImageType> ( mInstance );
			uint32_t width, height, channel_count;
			uint8_t* ImageData = bb::GetGlyphTextureData ( mCurrentFontFaceId, CodePoints[i], width, height, channel_count );
			mCodepointKeyTextureMap[CodePoints[i]] = mGlyphImages.size ( );
			id = mGlyphImages.size ( );
			mPainter->RegisterPainterParameter ( *Image, 0, 1, mGlyphImages.size ( ) );
			Image->Setup ( reinterpret_cast<void**>(&ImageData), width, height, channel_count );
			mGlyphImages.push_back ( std::move ( Image ) );
		}
		bb::FillTextureIndexDataInVertexBufferAt ( id, VertexStartingIndex++ );
		bb::FillTextureIndexDataInVertexBufferAt ( id, VertexStartingIndex++ );
		bb::FillTextureIndexDataInVertexBufferAt ( id, VertexStartingIndex++ );
		bb::FillTextureIndexDataInVertexBufferAt ( id, VertexStartingIndex++ );
	}


	auto CurrentCharacterOffset = outId;
#ifndef JKR_NO_STAGING_BUFFERS
	rb::CopyToStagingBuffers (
		bb::GetVertexBufferData ( ),
		bb::GetIndexBufferData ( ),
		bb::CharCountToVertexBytes ( CurrentCharacterOffset ),
		bb::CharCountToIndexBytes ( CurrentCharacterOffset ),
		bb::CharCountToVertexBytes ( len ),
		bb::CharCountToIndexBytes ( len )
	);

	rb::RegisterBufferCopyRegionToPrimitiveFromStaging (
		bb::CharCountToVertexBytes ( CurrentCharacterOffset ),
		bb::CharCountToIndexBytes ( CurrentCharacterOffset ),
		bb::CharCountToVertexBytes ( len ),
		bb::CharCountToIndexBytes ( len )
	);
#else
	rb::CopyToPrimitive ( *mPrimitive,
								 bb::GetVertexBufferData ( ),
								 bb::GetIndexBufferData ( ),
								 bb::CharCountToVertexBytes ( CurrentCharacterOffset ),
								 bb::CharCountToIndexBytes ( CurrentCharacterOffset ),
								 bb::CharCountToVertexBytes ( len ),
								 bb::CharCountToIndexBytes ( len )
	);
#endif
	return TextDimension;
}

Jkr::Renderer::BestText_base::TextDimensions Jkr::Renderer::BestText::UpdateText ( uint32_t inId, const std::string_view inText, uint32_t inX, uint32_t inY, uint32_t inDepthValue )
{
	std::vector<uint32_t> CodePoints;
	CodePoints.reserve ( rb::InitialRendererElementArraySize * 10 );
	uint32_t len;
	auto TextDimension = bb::UpdateText ( inId, inText, mCurrentFontFaceId, inDepthValue, CodePoints );

	uint32_t VertexStartingIndex = inId;
	for (int i = 0; i < CodePoints.size ( ); i++)
	{
		uint32_t id;
		if (mCodepointKeyTextureMap.contains ( CodePoints[i] ))
		{
			id = mCodepointKeyTextureMap[CodePoints[i]];
		}
		else {
			Up<ImageType> Image = MakeUp<ImageType> ( mInstance );
			uint32_t width, height, channel_count;
			uint8_t* ImageData = bb::GetGlyphTextureData ( mCurrentFontFaceId, CodePoints[i], width, height, channel_count );
			mCodepointKeyTextureMap[CodePoints[i]] = mGlyphImages.size ( );
			id = mGlyphImages.size ( );
			mPainter->RegisterPainterParameter ( *Image, 0, 1, mGlyphImages.size ( ) );
			Image->Setup ( reinterpret_cast<void**>(&ImageData), width, height, channel_count );
			mGlyphImages.push_back ( std::move ( Image ) );
		}
		bb::FillTextureIndexDataInVertexBufferAt ( id, VertexStartingIndex++ );
		bb::FillTextureIndexDataInVertexBufferAt ( id, VertexStartingIndex++ );
		bb::FillTextureIndexDataInVertexBufferAt ( id, VertexStartingIndex++ );
		bb::FillTextureIndexDataInVertexBufferAt ( id, VertexStartingIndex++ );
	}

	auto Offset = inId;

#ifndef JKR_NO_STAGING_BUFFERS
	rb::CopyToStagingBuffers (
		bb::GetVertexBufferData ( ),
		bb::GetIndexBufferData ( ),
		bb::CharCountToVertexBytes ( Offset ),
		bb::CharCountToIndexBytes ( Offset ),
		bb::CharCountToVertexBytes ( len ),
		bb::CharCountToIndexBytes ( len )
	);
	rb::RegisterBufferCopyRegionToPrimitiveFromStaging (
		bb::CharCountToVertexBytes ( Offset ),
		bb::CharCountToIndexBytes ( Offset ),
		bb::CharCountToVertexBytes ( len ),
		bb::CharCountToIndexBytes ( len )
	);
#else
	rb::CopyToPrimitive ( *mPrimitive,
								 bb::GetVertexBufferData ( ),
								 bb::GetIndexBufferData ( ),
								 bb::CharCountToVertexBytes ( Offset ),
								 bb::CharCountToIndexBytes ( Offset ),
								 bb::CharCountToVertexBytes ( len ),
								 bb::CharCountToIndexBytes ( len )
	);
#endif


	return TextDimension;
}

void Jkr::Renderer::BestText::SetCurrentFontFace ( uint32_t inFontFaceId )
{
	mCurrentFontFaceId = inFontFaceId;
}

void Jkr::Renderer::BestText::Dispatch ( Window& inWindow )
{
#ifndef JKR_NO_STAGING_BUFFERS
	if (!rb::IsCopyRegionsEmpty ( ))
	{
		rb::CmdCopyToPrimitiveFromStagingBuffer (
			mInstance,
			*mPrimitive,
			inWindow,
			bb::CharCountToVertexBytes ( mTotalNoOfCharsRendererCanHold ),
			bb::CharCountToIndexBytes ( mTotalNoOfCharsRendererCanHold )
		);
	}
#endif
}

void Jkr::Renderer::BestText::Bind ( Window& inWindow )
{
	mPainter->BindDrawParamters_EXT<PushConstant> ( *mPrimitive, inWindow );
}

void Jkr::Renderer::BestText::Draw ( Window& inWindow, glm::vec4 inColor, uint32_t inWindowW, uint32_t inWindowH, uint32_t inStartTextId, uint32_t inNoOfChars, glm::mat4 inMatrix )
{
	glm::mat4 Matrix = glm::ortho (
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

	mPainter->Draw_EXT<PushConstant> (
		*mPrimitive,
		Push,
		inWindow,
		6 * inNoOfChars,
		1,
		inStartTextId * 6,
		0
	);
}


void Jkr::Renderer::BestText::CheckAndResize(size_t inNewSize)
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
		bb::Resize(mTotalNoOfCharsRendererCanHold);
		mPrimitive.reset();
		mPrimitive = MakeUp<Primitive>(
			mInstance,
			bb::CharCountToVertexBytes(mTotalNoOfCharsRendererCanHold),
			bb::CharCountToIndexBytes(mTotalNoOfCharsRendererCanHold)
		);
#ifndef JKR_NO_STAGING_BUFFERS
		rb::ResizeStagingBuffer(
			mInstance,
			bb::CharCountToVertexBytes(mTotalNoOfCharsRendererCanHold),
			bb::CharCountToIndexBytes(mTotalNoOfCharsRendererCanHold)
		);
		rb::CopyToStagingBuffers(
			bb::GetVertexBufferData(),
			bb::GetIndexBufferData(),
			bb::CharCountToVertexBytes(0),
			bb::CharCountToIndexBytes(0),
			bb::CharCountToVertexBytes(mTotalNoOfCharsRendererCanHold),
			bb::CharCountToIndexBytes(mTotalNoOfCharsRendererCanHold)
		);
		rb::RegisterBufferCopyRegionToPrimitiveFromStaging(
			bb::CharCountToVertexBytes(0),
			bb::CharCountToIndexBytes(0),
			bb::CharCountToVertexBytes(mTotalNoOfCharsRendererCanHold),
			bb::CharCountToIndexBytes(mTotalNoOfCharsRendererCanHold)
		);
#else
		rb::CopyToPrimitive(*mPrimitive,
			bb::GetVertexBufferData(),
			bb::GetIndexBufferData(),
			bb::CharCountToVertexBytes(0),
			bb::CharCountToIndexBytes(0),
			bb::CharCountToVertexBytes(mTotalNoOfCharsRendererCanHold),
			bb::CharCountToIndexBytes(mTotalNoOfCharsRendererCanHold)
		);
#endif
}
}

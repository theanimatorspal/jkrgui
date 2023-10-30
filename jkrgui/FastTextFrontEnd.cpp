#include "FastTextFrontEnd.hpp"

Jkr::Font::FastTextRenderer::FastTextRenderer(const Instance& inInstance, const GUIWindow& inWindow, const GUIPainterCache& inCache, std::string inFolderPath)
	: mInstance(inInstance), mWindow(inWindow)
{
	using namespace ksai::Font;
	mFastTexts = MakeUp<FastText>(inFolderPath + ".fnt");
	mAtlases = MakeUp<AtlasType>(mInstance);
	mAtlases->Setup(inFolderPath + ".png");

	time.reset();
	mPainter = MakeUp<GUIPainter>(mInstance, inWindow, inCache);

	time.reset();
	mPainter->RegisterPainterParameter(*mAtlases, 0, 0, 0);

	mT_Vertices.resize(4 * mTotalNoOfCharsRendererCanHoldAtOnce, kstd::Vertex{});
	mT_Indices.resize(6 * mTotalNoOfCharsRendererCanHoldAtOnce, 0);

	mPrimitive = MakeUp<Jkr::GUIPrimitive>(mInstance, mT_Vertices, mT_Indices);
	mStagingVertexBuffer = MakeUp<StagingBuffer>(mInstance.GetVMA(), mInstance.GetDevice(), sizeOfVertices * mTotalNoOfCharsStagingBufferCanHold);
	mStagingVertexBuffer->MapMemoryRegion(&mVertexStagingBufferMemoryMapPtr);
	mStagingIndexBuffer = MakeUp<StagingBuffer>(mInstance.GetVMA(), mInstance.GetDevice(), sizeOfIndices * mTotalNoOfCharsStagingBufferCanHold);
	mStagingIndexBuffer->MapMemoryRegion(&mIndexStagingBufferMemoryMapPtr);
}

void Jkr::Font::FastTextRenderer::Dispatch()
{
	if (mNoOfCharsToCopyInStagingBuffer != 0)
	{
		CopyToMainPrimitiveFromStagingBuffer(
			mNoOfCharactersDrawn,
			mNoOfCharactersDrawn,
			mNoOfCharsToCopyInStagingBuffer
		);

		//mNoOfCharactersDrawn += mNoOfCharsToCopyInStagingBuffer;
		//mNoOfCharsToCopyInStagingBuffer = 0;
	}

	for (const auto& Range : mDynamicTextCharacterIndexRanges)
	{
		mVertexCopyRegions.push_back(vk::BufferCopy(Range.mStart * sizeOfVertices, Range.mStart * sizeOfVertices, Range.mSize * sizeOfVertices));
		mIndexCopyRegions.push_back(vk::BufferCopy(Range.mStart * sizeOfIndices, Range.mStart * sizeOfIndices, Range.mSize * sizeOfIndices));
	}

   mInstance.GetCommandBuffers()[mWindow.GetCurrentFrame()].GetCommandBufferHandle().copyBuffer(
		mStagingVertexBuffer->GetBufferHandle(),
		mPrimitive->GetVertexBufferPtr()->GetBufferHandle(),
		mVertexCopyRegions
	);

	mPrimitive->GetVertexBufferPtr()->SetBarrier(
		mInstance.GetCommandBuffers()[mWindow.GetCurrentFrame()],
		0,
		mTotalNoOfCharsStagingBufferCanHold * sizeOfVertices,
		vk::AccessFlagBits::eTransferWrite,
		vk::AccessFlagBits::eVertexAttributeRead,
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eVertexInput
	);

   mInstance.GetCommandBuffers()[mWindow.GetCurrentFrame()].GetCommandBufferHandle().copyBuffer(
		mStagingIndexBuffer->GetBufferHandle(),
		mPrimitive->GetIndexBufferPtr()->GetBufferHandle(),
		mIndexCopyRegions
	);

	mPrimitive->GetIndexBufferPtr()->SetBarrier(
		mInstance.GetCommandBuffers()[mWindow.GetCurrentFrame()],
		0,
		mTotalNoOfCharsStagingBufferCanHold * sizeOfIndices,
		vk::AccessFlagBits::eTransferWrite,
		vk::AccessFlagBits::eVertexAttributeRead,
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eVertexInput
	);
	mVertexCopyRegions.clear();
	mIndexCopyRegions.clear();

}

void Jkr::Font::FastTextRenderer::CheckResize(int inNewCharactersRequired)
{
	uint32_t ActualResizeFactor = inNewCharactersRequired;
	bool ThereAreMoreStagingChars = (mNoOfCharsToCopyInStagingBuffer + inNewCharactersRequired > mTotalNoOfCharsRendererCanHoldAtOnce);
	bool MoreStringCharsRequired = (inNewCharactersRequired > mTotalNoOfCharsRendererCanHoldAtOnce);
	bool SizeIsInsufficient =  ThereAreMoreStagingChars || MoreStringCharsRequired;
	if (SizeIsInsufficient)
	{
		ActualResizeFactor = MoreStringCharsRequired ? inNewCharactersRequired * 2 / mTotalNoOfCharsRendererCanHoldAtOnce : 2;

		mStagingIndexBuffer.reset();
		mStagingVertexBuffer.reset();
		mPrimitive.reset();
		mVertexStagingBufferMemoryMapPtr = nullptr;
		mIndexStagingBufferMemoryMapPtr = nullptr;

		mTotalNoOfCharsStagingBufferCanHold *= ActualResizeFactor;
		mTotalNoOfCharsRendererCanHoldAtOnce *= ActualResizeFactor;
		mT_Vertices.resize(mT_Vertices.size() * ActualResizeFactor, kstd::Vertex{});
		mT_Indices.resize(mT_Indices.size() * ActualResizeFactor, 0);
		mPrimitive = MakeUp<Jkr::GUIPrimitive>(mInstance, mT_Vertices, mT_Indices);

		mStagingVertexBuffer = MakeUp<StagingBuffer>(mInstance.GetVMA(), mInstance.GetDevice(), sizeOfVertices * mTotalNoOfCharsStagingBufferCanHold);
		mStagingVertexBuffer->MapMemoryRegion(&mVertexStagingBufferMemoryMapPtr);
		mStagingIndexBuffer = MakeUp<StagingBuffer>(mInstance.GetVMA(), mInstance.GetDevice(), sizeOfIndices * mTotalNoOfCharsStagingBufferCanHold);
		mStagingIndexBuffer->MapMemoryRegion(&mIndexStagingBufferMemoryMapPtr);

		std::memcpy(
			(char*)mVertexStagingBufferMemoryMapPtr,
			mT_Vertices.data(),
			mNoOfCharsToCopyInStagingBuffer * sizeOfVertices
		);
		mStagingVertexBuffer->FlushMemoryRanges(0, mNoOfCharsToCopyInStagingBuffer * sizeOfVertices);
		std::memcpy(
			(char*)mIndexStagingBufferMemoryMapPtr,
			mT_Indices.data(),
			mNoOfCharsToCopyInStagingBuffer * sizeOfIndices
		);
		mStagingIndexBuffer->FlushMemoryRanges(0, mNoOfCharsToCopyInStagingBuffer * sizeOfIndices);
	}
}

void Jkr::Font::FastTextRenderer::RegisterStaticText(const std::string inString, kstd::ui32 inX, kstd::ui32 inY, FeatureFlags inFlags, kstd::ui32 inDepthValue)
{
	CheckResize(inString.size());
	auto Features = mFastTexts->GetTextFeatures(inString.c_str(), 1/inFlags.inSizeFactor);
	kstd::ui32 x = 0;
	kstd::ui32 y = 0;
	if (inFlags.h == HorizontalAlignment::Left) x = inX;
	else if (inFlags.h == HorizontalAlignment::Right) x = inX - Features.mWidth;
	else if (inFlags.h == HorizontalAlignment::Center) x = inX - Features.mWidth / 2.0f;
	if (inFlags.v == VerticalAlignment::Bottom) y = inY;
	else if (inFlags.v == VerticalAlignment::Top) y = inY + Features.mHeight;
	else if (inFlags.v == VerticalAlignment::Center) y = inY + Features.mHeight / 2.0f;

	mFastTexts->GenerateQuadsAt(inString.c_str(), mT_Vertices, mT_Indices, x, y, 1/inFlags.inSizeFactor, mNoOfCharsToCopyInStagingBuffer, inDepthValue);

	std::memcpy(
		(char*)mVertexStagingBufferMemoryMapPtr + sizeOfVertices * mNoOfCharsToCopyInStagingBuffer,
		(char*)mT_Vertices.data() + sizeOfVertices * mNoOfCharsToCopyInStagingBuffer,
		inString.size() * sizeOfVertices
	);
	mStagingVertexBuffer->FlushMemoryRanges(sizeOfVertices * mNoOfCharsToCopyInStagingBuffer, inString.size() * sizeOfVertices);

	std::memcpy(
		(char*)mIndexStagingBufferMemoryMapPtr + sizeOfIndices * mNoOfCharsToCopyInStagingBuffer,
		(char*)mT_Indices.data() + sizeOfIndices * mNoOfCharsToCopyInStagingBuffer,
		inString.size() * sizeOfIndices
	);
	mStagingIndexBuffer->FlushMemoryRanges(sizeOfIndices * mNoOfCharsToCopyInStagingBuffer, inString.size() * sizeOfIndices);

	mNoOfCharsToCopyInStagingBuffer += inString.size();
}

void Jkr::Font::FastTextRenderer::RegisterDynamicText(kstd::ui32 inCharCount, kstd::ui32& outId)
{
	CheckResize(inCharCount);
	mDynamicTextCharacterIndexRanges.push_back(
		CharMemoryRange{ .mStart = mNoOfCharsToCopyInStagingBuffer, .mSize = inCharCount }
	);
	mNoOfCharsToCopyInStagingBuffer += inCharCount;
	outId = mDynamicTextCharacterIndexRanges.size() - 1;

	mVertexCopyRegions.reserve(mDynamicTextCharacterIndexRanges.size());
	mIndexCopyRegions.reserve(mDynamicTextCharacterIndexRanges.size());
}

void Jkr::Font::FastTextRenderer::UpdateDynamicText(kstd::ui32 inId, const char* inString, kstd::ui32 inX, kstd::ui32 inY, FeatureFlags inFlags, kstd::ui32 inDepthValue)
{

	if (mDynamicTextCharacterIndexRanges.size() > inId)
	{

		auto Features = mFastTexts->GetTextFeatures(inString, 1/inFlags.inSizeFactor);
		kstd::ui32 x = 0;
		kstd::ui32 y = 0;
		if (inFlags.h == HorizontalAlignment::Left) x = inX;
		else if (inFlags.h == HorizontalAlignment::Right) x = inX - Features.mWidth;
		else if (inFlags.h == HorizontalAlignment::Center) x = inX - Features.mWidth / 2.0f;
		if (inFlags.v == VerticalAlignment::Bottom) y = inY;
		else if (inFlags.v == VerticalAlignment::Top) y = inY + Features.mHeight;
		else if (inFlags.v == VerticalAlignment::Center) y = inY + Features.mHeight / 2.0f;


		mFastTexts->GenerateQuadsAt(inString, mT_Vertices, mT_Indices, x, y, 1/inFlags.inSizeFactor, mDynamicTextCharacterIndexRanges[inId].mStart, inDepthValue);


		std::memcpy(
			(char*)mVertexStagingBufferMemoryMapPtr + sizeOfVertices * mDynamicTextCharacterIndexRanges[inId].mStart,
			(char*)mT_Vertices.data() + sizeOfVertices * mDynamicTextCharacterIndexRanges[inId].mStart,
			mDynamicTextCharacterIndexRanges[inId].mSize * sizeOfVertices
		);
		mStagingVertexBuffer->FlushMemoryRanges(
			sizeOfVertices * mDynamicTextCharacterIndexRanges[inId].mStart,
			mDynamicTextCharacterIndexRanges[inId].mSize * sizeOfVertices
		);
		std::memcpy(
			(char*)mIndexStagingBufferMemoryMapPtr + sizeOfIndices * mDynamicTextCharacterIndexRanges[inId].mStart,
			(char*)mT_Indices.data() + sizeOfIndices * mDynamicTextCharacterIndexRanges[inId].mStart,
			mDynamicTextCharacterIndexRanges[inId].mSize * sizeOfIndices
		);
		mStagingIndexBuffer->FlushMemoryRanges(
			sizeOfIndices * mDynamicTextCharacterIndexRanges[inId].mStart,
			mDynamicTextCharacterIndexRanges[inId].mSize * sizeOfIndices
		);
	}

}

void Jkr::Font::FastTextRenderer::CopyToMainPrimitiveFromStagingBuffer(kstd::ui32 InFromBufferOffsetIndex, kstd::ui32 InToBufferOffsetIndex, kstd::ui32 inSizeToCopyIndex)
{
	mPrimitive->GetVertexBufferPtr()->CmdCopyFrom(
		mInstance.GetCommandBuffers()[mWindow.GetCurrentFrame()],
		*mStagingVertexBuffer,
		InFromBufferOffsetIndex * sizeOfVertices,
		InToBufferOffsetIndex * sizeOfVertices,
		inSizeToCopyIndex * sizeOfVertices
	);
	mPrimitive->GetVertexBufferPtr()->SetBarrier(
		mInstance.GetCommandBuffers()[mWindow.GetCurrentFrame()],
		InToBufferOffsetIndex * sizeOfVertices,
		inSizeToCopyIndex * sizeOfVertices,
		vk::AccessFlagBits::eTransferWrite,
		vk::AccessFlagBits::eVertexAttributeRead,
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eVertexInput
	);
	mPrimitive->GetIndexBufferPtr()->CmdCopyFrom(
		mInstance.GetCommandBuffers()[mWindow.GetCurrentFrame()],
		*mStagingIndexBuffer,
		InFromBufferOffsetIndex * sizeOfIndices,
		InToBufferOffsetIndex * sizeOfIndices,
		inSizeToCopyIndex * sizeOfIndices
	);
	mPrimitive->GetIndexBufferPtr()->SetBarrier(
		mInstance.GetCommandBuffers()[mWindow.GetCurrentFrame()],
		InToBufferOffsetIndex * sizeOfIndices,
		inSizeToCopyIndex * sizeOfIndices,
		vk::AccessFlagBits::eTransferWrite,
		vk::AccessFlagBits::eVertexAttributeRead,
		vk::PipelineStageFlagBits::eTransfer,
		vk::PipelineStageFlagBits::eVertexInput
	);
}


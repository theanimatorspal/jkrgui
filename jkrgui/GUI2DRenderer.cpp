#include "GUI2DRenderer.hpp"

Jkr::GUI2DRenderer::GUI2DRenderer(const Instance& inInstance, const GUIWindow& inWindow, const GUIPainterCache& inCache)
	: mInstance(inInstance), mWindow(inWindow)
{
	mPainter = MakeUp<Jkr::GUIPainter>(mInstance, mWindow, inCache, 1000);

	mT_Vertices.resize(4 * mTotalNoOfQuadsRendererCanHold, kstd::VertexEXT{});
	mT_Indices.resize(6 * mTotalNoOfQuadsRendererCanHold, 0);

	mPrimitive = MakeUp<Jkr::GUIPrimitive>(mInstance, mT_Vertices, mT_Indices, 0);

	mStagingVertexBuffer = MakeUp<StagingBuffer>(mInstance.GetVMA(), mInstance.GetDevice(), sizeOfVertices * mTotalNoOfQuadsStagingBufferCanHold);
	mStagingVertexBuffer->MapMemoryRegion(&mVertexStagingBufferMemoryMapPtr);
	mStagingIndexBuffer = MakeUp<StagingBuffer>(mInstance.GetVMA(), mInstance.GetDevice(), sizeOfIndices * mTotalNoOfQuadsStagingBufferCanHold);
	mStagingIndexBuffer->MapMemoryRegion(&mIndexStagingBufferMemoryMapPtr);

	std::memset(mVertexStagingBufferMemoryMapPtr, 0, sizeOfVertices * mTotalNoOfQuadsStagingBufferCanHold);
	std::memset(mIndexStagingBufferMemoryMapPtr, 0, sizeOfIndices * mTotalNoOfQuadsStagingBufferCanHold);

	kstd::ui32 i;
	RegisterStaticQuad(0, 0, 0, 0, i);
	/*
	TODO
	This Function Call is to be removed.
	The first 6 * 4 bytes of VertexBuffer in mPrimitive is corrupted by VkCmdCopyBuffer for an unknown reason, Therefore a quick Solution.
	*/
}

Jkr::GUI2DRenderer::GUI2DRenderer(const Instance& inInstance, const GUIWindow& inWindow, const GUIPainterCache& inCache, void* inExtensionForDrawCall)
	: mInstance(inInstance), mWindow(inWindow)
{
}

void Jkr::GUI2DRenderer::Draw(kstd::ui32 inWindowW, kstd::ui32 inWindowH, const GUIWindow& inWindow)
{
	glm::mat4 Matrix = glm::ortho(
		0.0f,
		static_cast<float>(inWindowW),
		0.0f,
		static_cast<float>(inWindowH),
		100.0f,
		-100.0f
	);

	Draw_WithSeparateDrawCallsPerQuad(Matrix, inWindow);
}

void Jkr::GUI2DRenderer::Draw_WithSeparateDrawCallsPerQuad(const glm::mat4& Matrix, const Jkr::GUIWindow& inWindow)
{
	FragmentPushConstantStruct Push;
	Push.mColor = glm::vec4(1.0f, 1.0f, 1.0, 1.0f);
	Push.mMatrix = Matrix;
	mPainter->BindDrawParamters_EXT<FragmentPushConstantStruct>(*mPrimitive, inWindow);
	for (int i = 0; i < mQuadsWithItsOwnTextureIndex.size(); i++)
	{
		Push.index = mQuadsWithItsOwnTextureIndex[i].mTextureIndex;
		const auto QIndex = mQuadsWithItsOwnTextureIndex[i].mQuadIndex;
		mPainter->Draw_EXT<FragmentPushConstantStruct>(*mPrimitive, Push, inWindow, 6, 1, QIndex * 6, 0);
	}
}

void Jkr::GUI2DRenderer::Draw(kstd::ui32 inWindowW, kstd::ui32 inWindowH)
{
	Draw(inWindowW, inWindowH, mWindow);
}

void Jkr::GUI2DRenderer::DrawQuadWithExistingTextureCoordinate(kstd::ui32 inId, glm::vec3 mColor, kstd::ui32 inWindowW, kstd::ui32 inWindowH, const GUIWindow& inWindow)
{
	glm::mat4 Matrix = glm::ortho(
		0.0f,
		static_cast<float>(inWindowW),
		0.0f,
		static_cast<float>(inWindowH),
		100.0f,
		-100.0f
	);
	FragmentPushConstantStruct Push;
	Push.mMatrix = Matrix;
	Push.mColor = glm::vec4(mColor, 0.0f);
	Push.index = mQuadsWithExistingTextureIndex[inId].mTextureIndex;
	const auto QIndex = mQuadsWithExistingTextureIndex[inId].mQuadIndex;
	mPainter->Draw_EXT<FragmentPushConstantStruct>(*mPrimitive, Push, inWindow, 6, 1, QIndex * 6, 0);
}

void Jkr::GUI2DRenderer::CopyTheGeneratedQuadToStagingBuffer(const ksai::kstd::ui32 inOffset, const ksai::kstd::ui32 NoOfQuadsGenerated)
{
	std::memcpy(
		(char*)mVertexStagingBufferMemoryMapPtr + sizeOfVertices * inOffset,
		(char*)mT_Vertices.data() + sizeOfVertices * inOffset,
		sizeOfVertices * NoOfQuadsGenerated
	);
	mStagingVertexBuffer->FlushMemoryRanges(sizeOfVertices * inOffset, sizeOfVertices * 1);

	std::memcpy(
		(char*)mIndexStagingBufferMemoryMapPtr + sizeOfIndices * inOffset,
		(char*)mT_Indices.data() + sizeOfIndices * inOffset,
		sizeOfIndices * NoOfQuadsGenerated
	);
	mStagingIndexBuffer->FlushMemoryRanges(sizeOfIndices * inOffset, sizeOfIndices * 1);
}

void Jkr::GUI2DRenderer::GenerateTextureCoordinates(glm::vec2& inFirst, glm::vec2& inSecond, glm::vec2& inThird, glm::vec2& inFourth)
{
	inFirst = { 0, 1 };
	inSecond = { 0, 0 };
	inThird = { 1, 0 };
	inFourth = { 1, 1 };
}

void Jkr::GUI2DRenderer::Dispatch(float inTime)
{
	if (mNoOfQuadsToCopyInStagingBufferToMainBuffer != 0)
	{
		CopyToMainPrimitiveFromStagingBuffer(
			mNoOfQuadsDrawn,
			mNoOfQuadsDrawn,
			mNoOfQuadsToCopyInStagingBufferToMainBuffer
		);
	}

	if (mDynamicQuadIndexCopyRegions.size() != 0)
	{
		mInstance.GetCommandBuffers()[mWindow.GetCurrentFrame()].GetCommandBufferHandle().copyBuffer(
			mStagingVertexBuffer->GetBufferHandle(),
			mPrimitive->GetVertexBufferPtr()->GetBufferHandle(),
			mDynamicQuadVertexCopyRegions
		);

		mPrimitive->GetVertexBufferPtr()->SetBarrier(
			mInstance.GetCommandBuffers()[mWindow.GetCurrentFrame()],
			0,
			mTotalNoOfQuadsRendererCanHold * sizeOfVertices,
			vk::AccessFlagBits::eTransferWrite,
			vk::AccessFlagBits::eVertexAttributeRead,
			vk::PipelineStageFlagBits::eTransfer,
			vk::PipelineStageFlagBits::eVertexInput
		);

		mInstance.GetCommandBuffers()[mWindow.GetCurrentFrame()].GetCommandBufferHandle().copyBuffer(
			mStagingIndexBuffer->GetBufferHandle(),
			mPrimitive->GetIndexBufferPtr()->GetBufferHandle(),
			mDynamicQuadIndexCopyRegions
		);

		mPrimitive->GetIndexBufferPtr()->SetBarrier(
			mInstance.GetCommandBuffers()[mWindow.GetCurrentFrame()],
			0,
			mTotalNoOfQuadsRendererCanHold * sizeOfIndices,
			vk::AccessFlagBits::eTransferWrite,
			vk::AccessFlagBits::eVertexAttributeRead,
			vk::PipelineStageFlagBits::eTransfer,
			vk::PipelineStageFlagBits::eVertexInput
		);
	}

	static int i = 0;
	if (i == 0)
	{
		for (auto& u : mTextures)
		{
			u->GetStorageImage().CmdTransitionImageLayout(
				mInstance.GetCommandBuffers()[mWindow.GetCurrentFrame()],
				vk::ImageLayout::eUndefined,
				vk::ImageLayout::eGeneral,
				vk::PipelineStageFlagBits::eFragmentShader,
				vk::PipelineStageFlagBits::eBottomOfPipe,
				vk::AccessFlagBits::eNone,
				vk::AccessFlagBits::eNone
			);
		}

		ComputePushConstantStruct Push;

		Push.mPosDimen = glm::vec4(0, 0, 0, 0);
		Push.mColor = glm::vec4(0, 0, 0, 0);
		Push.mTextureCoordinateIndex = 0;

		mPainter->BindDispatchParameters_EXT<ComputePushConstantStruct>(*mPrimitive);
		for (int i = 0; i < mTextures.size(); i++)
		{
			mDrawFunctions[i](*this, i);
			Push.mTextureCoordinateIndex++;
		}
		i = 1;
	}
	/*
	Bind for dynamic Textures
	*/
	mPainter->BindDispatchParameters_EXT<ComputePushConstantStruct>(*mPrimitive);

}

void Jkr::GUI2DRenderer::RegisterStaticQuad(kstd::ui32 inX, kstd::ui32 inY, kstd::ui32 inW, kstd::ui32 inH, kstd::ui32& outTextureId, const std::function<void(GUI2DRenderer&, kstd::ui32)>& inDrawFunction, kstd::ui32 inDepthValue, kstd::ui32 inTextureWidth, kstd::ui32 inTextureHeight)
{
	if (mNoOfQuadsToCopyInStagingBufferToMainBuffer + 1 > mTotalNoOfQuadsRendererCanHold)
	{
		auto ResizeFactor = 2;
		CheckResize(ResizeFactor);
	}

	auto v_index = mNoOfQuadsToCopyInStagingBufferToMainBuffer * 4;
	auto i_index = mNoOfQuadsToCopyInStagingBufferToMainBuffer * 6;
	auto NoOfQuadsGenerated = GenerateTheVerticesAndIndices(inX, inW, inY, inH, v_index, i_index, inDepthValue);
	GenerateTextureCoordinates(mT_Vertices[v_index + 0].mTextureCoordinates, mT_Vertices[v_index + 1].mTextureCoordinates, mT_Vertices[v_index + 2].mTextureCoordinates, mT_Vertices[v_index + 3].mTextureCoordinates);
	CopyTheGeneratedQuadToStagingBuffer(mNoOfQuadsToCopyInStagingBufferToMainBuffer, NoOfQuadsGenerated);

	auto Image = MakeUp<ImageType>(mInstance);
	Image->Setup(inTextureWidth, inTextureHeight);
	mTextures.push_back(std::move(Image));
	const uint32_t CurrentTextureCoordinate = mTextures.size() - 1;
	mPainter->RegisterPainterParameter(*mTextures[CurrentTextureCoordinate], 0, mDstBindingForImages, CurrentTextureCoordinate);

	mQuadsWithItsOwnTextureIndex.push_back(
		QuadsWithTextureIndex{
		.mQuadIndex = mTextureCoordinateIndicesOfAllQuads.size(),
		.mTextureIndex = CurrentTextureCoordinate
		}
	);
	mTextureCoordinateIndicesOfAllQuads.push_back(CurrentTextureCoordinate);

	outTextureId = mNoOfQuadsToCopyInStagingBufferToMainBuffer; // TODO TextureIndex
	mNoOfQuadsToCopyInStagingBufferToMainBuffer += 1;
	mDrawFunctions.push_back(inDrawFunction);
}

void Jkr::GUI2DRenderer::RegisterStaticQuadWithExistingTexture(kstd::ui32& outIndex, kstd::ui32 inTextureId, kstd::ui32 inX, kstd::ui32 inY, kstd::ui32 inW, kstd::ui32 inH, kstd::ui32 inDepthValue)
{
	if (mNoOfQuadsToCopyInStagingBufferToMainBuffer + 1 > mTotalNoOfQuadsRendererCanHold)
	{
		auto ResizeFactor = 2;
		CheckResize(ResizeFactor);
	}
	auto v_index = mNoOfQuadsToCopyInStagingBufferToMainBuffer * 4;
	auto i_index = mNoOfQuadsToCopyInStagingBufferToMainBuffer * 6;
	auto NoOfQuadsGenerated = GenerateTheVerticesAndIndices(inX, inW, inY, inH, v_index, i_index, inDepthValue);
	GenerateTextureCoordinates(mT_Vertices[v_index + 0].mTextureCoordinates, mT_Vertices[v_index + 1].mTextureCoordinates, mT_Vertices[v_index + 2].mTextureCoordinates, mT_Vertices[v_index + 3].mTextureCoordinates);
	CopyTheGeneratedQuadToStagingBuffer(mNoOfQuadsToCopyInStagingBufferToMainBuffer, NoOfQuadsGenerated);

	outIndex = mQuadsWithExistingTextureIndex.size();
	mQuadsWithExistingTextureIndex.push_back(
		QuadsWithTextureIndex{
		.mTextureIndex = inTextureId,
		.mQuadIndex = mTextureCoordinateIndicesOfAllQuads.size()
		}
	);

	mTextureCoordinateIndicesOfAllQuads.push_back(inTextureId);

	mNoOfQuadsToCopyInStagingBufferToMainBuffer += 1;
}

void Jkr::GUI2DRenderer::RegisterDynamicQuad(kstd::ui32& outId, const std::function<void(GUI2DRenderer&, kstd::ui32)>& inDrawFunction, kstd::ui32 inTextureWidth, kstd::ui32 inTextureHeight, kstd::ui32 inDepthValue)
{
	RegisterStaticQuad(0, 0, 0, 0, outId, inDrawFunction, 0, inTextureWidth, inTextureHeight);
	auto Offset = outId;
	mDynamicQuadVertexCopyRegions.push_back(
		vk::BufferCopy(
			Offset * sizeOfVertices,
			Offset * sizeOfVertices,
			1 * sizeOfVertices
		)
	);
	mDynamicQuadIndexCopyRegions.push_back(
		vk::BufferCopy(
			Offset * sizeOfIndices,
			Offset * sizeOfIndices,
			1 * sizeOfIndices
		)
	);
}

void Jkr::GUI2DRenderer::RegisterDynamicQuadWithExistingTexture(kstd::ui32& outIndex, kstd::ui32 inTextureId, kstd::ui32 inX, kstd::ui32 inY, kstd::ui32 inW, kstd::ui32 inH, kstd::ui32 inDepthValue)
{
	RegisterStaticQuadWithExistingTexture(outIndex, inTextureId, inX, inY, inW, inH, inDepthValue);
	auto Offset = mQuadsWithExistingTextureIndex[outIndex].mQuadIndex;
	mDynamicQuadVertexCopyRegions.push_back(
		vk::BufferCopy(
			Offset * sizeOfVertices,
			Offset * sizeOfVertices,
			1 * sizeOfVertices
		)
	);
	mDynamicQuadIndexCopyRegions.push_back(
		vk::BufferCopy(
			Offset * sizeOfIndices,
			Offset * sizeOfIndices,
			1 * sizeOfIndices
		)
	);
}

void Jkr::GUI2DRenderer::UpdateDynamicQuad(kstd::ui32 inId, kstd::ui32 inX, kstd::ui32 inY, kstd::ui32 inW, kstd::ui32 inH, kstd::ui32 inDepthValue)
{
	auto Offset = inId;
	auto v_index = Offset * 4;
	auto i_index = Offset * 6;
	auto NoOfQuadsGenerated = GenerateTheVerticesAndIndices(inX, inW, inY, inH, v_index, i_index, inDepthValue);
	GenerateTextureCoordinates(mT_Vertices[v_index + 0].mTextureCoordinates, mT_Vertices[v_index + 1].mTextureCoordinates, mT_Vertices[v_index + 2].mTextureCoordinates, mT_Vertices[v_index + 3].mTextureCoordinates);
	CopyTheGeneratedQuadToStagingBuffer(Offset, NoOfQuadsGenerated);
}

void Jkr::GUI2DRenderer::UpdateDynamicQuadWithExistingTexture(kstd::ui32 inId, kstd::ui32 inX, kstd::ui32 inY, kstd::ui32 inW, kstd::ui32 inH, kstd::ui32 inDepthValue)
{
	UpdateDynamicQuad(mQuadsWithExistingTextureIndex[inId].mQuadIndex, inX, inY, inW, inH, inDepthValue);
}

ksai::kstd::ui32 Jkr::GUI2DRenderer::GenerateTheVerticesAndIndices(const ksai::kstd::ui32 inX, const ksai::kstd::ui32 inW, const ksai::kstd::ui32 inY, const ksai::kstd::ui32 inH, unsigned int v_index, unsigned int i_index, ksai::kstd::ui32 inDepthValue)
{
	mT_Vertices.resize(mT_Vertices.size() + 4, kstd::VertexEXT{});
	mT_Indices.resize(mT_Indices.size() + 6, 0);

	auto x = inX;
	auto dx = inW;
	auto y = inY;
	auto dy = inH;
	auto image_id = mNoOfQuadsToCopyInStagingBufferToMainBuffer;

	mT_Vertices[v_index + 0] = kstd::VertexEXT{
		.mPosition = glm::vec3(x, y + dy, inDepthValue),
		.mTextureCoordinates = glm::vec2(0, 1),
		.mIvec2 = glm::ivec3(image_id, image_id, image_id)
	};

	mT_Vertices[v_index + 1] = kstd::VertexEXT{
		.mPosition = glm::vec3(x, y, inDepthValue),
		.mTextureCoordinates = glm::vec2(0, 0),
		.mIvec2 = glm::ivec3(image_id, image_id, image_id)
	};

	mT_Vertices[v_index + 2] = kstd::VertexEXT{
		.mPosition = glm::vec3(x + dx, y, inDepthValue),
		.mTextureCoordinates = glm::vec2(1, 0),
		.mIvec2 = glm::ivec3(image_id, image_id, image_id)
	};

	mT_Vertices[v_index + 3] = kstd::VertexEXT{
		.mPosition = glm::vec3(x + dx, y + dy, inDepthValue),
		.mTextureCoordinates = glm::vec2(1, 1),
		.mIvec2 = glm::ivec3(image_id, image_id, image_id)
	};

	mT_Indices[i_index + 0] = (0 + v_index);
	mT_Indices[i_index + 1] = (1 + v_index);
	mT_Indices[i_index + 2] = (2 + v_index);
	mT_Indices[i_index + 3] = (0 + v_index);
	mT_Indices[i_index + 4] = (2 + v_index);
	mT_Indices[i_index + 5] = (3 + v_index);

	GenerateTextureCoordinates(
		mT_Vertices[v_index + 0].mTextureCoordinates,
		mT_Vertices[v_index + 1].mTextureCoordinates,
		mT_Vertices[v_index + 2].mTextureCoordinates,
		mT_Vertices[v_index + 3].mTextureCoordinates
	);

	return 1;
}

void Jkr::GUI2DRenderer::CopyToMainPrimitiveFromStagingBuffer(kstd::ui32 InFromBufferOffsetIndex, kstd::ui32 InToBufferOffsetIndex, kstd::ui32 inSizeToCopyIndex)
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

void Jkr::GUI2DRenderer::CheckResize(kstd::ui32 inSize)
{
	mStagingIndexBuffer.reset();
	mStagingVertexBuffer.reset();
	mPrimitive.reset();
	mVertexStagingBufferMemoryMapPtr = nullptr;
	mIndexStagingBufferMemoryMapPtr = nullptr;

	mTotalNoOfQuadsStagingBufferCanHold *= inSize;
	mTotalNoOfQuadsRendererCanHold *= inSize;
	mT_Vertices.resize(mT_Vertices.size() * 2, kstd::VertexEXT{});
	mT_Indices.resize(mT_Indices.size() * 2, 0);
	mPrimitive = MakeUp<Jkr::GUIPrimitive>(mInstance, mT_Vertices, mT_Indices, 0);

	mStagingVertexBuffer = MakeUp<StagingBuffer>(mInstance.GetVMA(), mInstance.GetDevice(), sizeOfVertices * mTotalNoOfQuadsStagingBufferCanHold);
	mStagingVertexBuffer->MapMemoryRegion(&mVertexStagingBufferMemoryMapPtr);
	mStagingIndexBuffer = MakeUp<StagingBuffer>(mInstance.GetVMA(), mInstance.GetDevice(), sizeOfIndices * mTotalNoOfQuadsStagingBufferCanHold);
	mStagingIndexBuffer->MapMemoryRegion(&mIndexStagingBufferMemoryMapPtr);
	CopyTheGeneratedQuadToStagingBuffer(0, mTotalNoOfQuadsStagingBufferCanHold);
}

void Jkr::GUI2DRenderer::DrawToQuadTextureDispatch(kstd::ui32 inId, Shape inShape, glm::vec4 mPosDimen, glm::vec4 inColor, glm::vec4 inParam)
{
	ComputePushConstantStruct Push;

	Push.mPosDimen = mPosDimen;
	Push.mColor = inColor;
	Push.mTextureCoordinateIndex = inId;
	Push.mParam = inParam;
	Push.mShapeIndex = static_cast<kstd::ui32>(inShape);
	mPainter->Dispatch_EXT<ComputePushConstantStruct>(*mPrimitive, Push, 64, 64, 1);
}

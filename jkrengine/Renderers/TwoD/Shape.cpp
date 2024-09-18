#include "Shape.hpp"
#include "VulkanImage.hpp"
#include "vulkan/vulkan_core.h"
#include "vulkan/vulkan_enums.hpp"
#include <glm/ext/matrix_clip_space.hpp>

using namespace Jkr::Renderer;

Shape::Shape(const Instance &inInstance,
             Window_base &inCompatibleWindow,
             std::unordered_map<FillType, Up<PainterCache>> &inPainterCaches,
             ui inVarDesCount)
    : mInstance(inInstance), mPainterCaches(inPainterCaches) {
    mPainters[FillType::Fill] =
         MakeUp<Painter>(inInstance, inCompatibleWindow, *inPainterCaches[FillType::Fill]);

#ifndef JKR_USE_VARIABLE_DES_INDEXING
    mPainters[FillType::Image] =
         MakeUp<Painter>(inInstance, inCompatibleWindow, *inPainterCaches[FillType::Image]);
#else
    mPainters[FillType::Image] = MakeUp<Painter>(
         inInstance, inCompatibleWindow, *inPainterCaches[FillType::Image], inVarDesCount);
    mVarDesVulkanDescriptorSet = MakeUp<VulkanDescriptorSet>(
         mInstance.GetDevice(),
         mInstance.GetDescriptorPool(),
         mPainterCaches[FillType::Image]->GetVertexFragmentDescriptorSetLayout(),
         inVarDesCount);
#endif

    mPainters[FillType::ContinousLine] =
         MakeUp<Painter>(inInstance, inCompatibleWindow, *inPainterCaches[FillType::ContinousLine]);
#ifndef JKR_NO_STAGING_BUFFERS
    rb::CreateStagingBuffers(inInstance,
                             sb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
                             sb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
#endif
    mPrimitive = MakeUp<Primitive>(inInstance,
                                   sb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
                                   sb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
}

void Shape::Add(Jkr::Generator &inShape, float inX, float inY, float inZ, ui &outId) {
    CheckAndResize(inShape);
    sb::Add(inShape, inX, inY, inZ, outId);
    const auto OffsetId = (ui)outId;
#ifndef JKR_NO_STAGING_BUFFERS
    rb::CopyToStagingBuffers(sb::GetVertexBufferData(),
                             sb::GetIndexBufferData(),
                             sb::VertexCountToBytes(sb::GetVertexOffsetAbsolute(OffsetId)),
                             sb::IndexCountToBytes(sb::GetIndexOffsetAbsolute(OffsetId)),
                             sb::VertexCountToBytes(inShape.GetVertexCount()),
                             sb::IndexCountToBytes(inShape.GetIndexCount()));
    rb::RegisterBufferCopyRegionToPrimitiveFromStaging(
         sb::VertexCountToBytes(sb::GetVertexOffsetAbsolute(OffsetId)),
         sb::IndexCountToBytes(sb::GetIndexOffsetAbsolute(OffsetId)),
         sb::VertexCountToBytes(inShape.GetVertexCount()),
         sb::IndexCountToBytes(inShape.GetIndexCount()));
#else
    rb::CopyToPrimitive(*mPrimitive,
                        sb::GetVertexBufferData(),
                        sb::GetIndexBufferData(),
                        sb::VertexCountToBytes(sb::GetVertexOffsetAbsolute(OffsetId)),
                        sb::IndexCountToBytes(sb::GetIndexOffsetAbsolute(OffsetId)),
                        sb::VertexCountToBytes(inShape.GetVertexCount()),
                        sb::IndexCountToBytes(inShape.GetIndexCount()));
#endif
}

void Shape::AddImage(const std::string_view inFileName, ui &outIndex) {
#ifndef JKR_USE_VARIABLE_DES_INDEXING
    Up<VulkanDescriptorSet> Desset = MakeUp<VulkanDescriptorSet>(
         mInstance.GetDevice(),
         mInstance.GetDescriptorPool(),
         mPainterCaches[FillType::Image]->GetVertexFragmentDescriptorSetLayout());
    Up<ImageType> Image = MakeUp<ImageType>(mInstance);
    Image->Setup(inFileName);
    Image->Register(0, 0, 0, *Desset);
    outIndex = mImages.size();
    mImages.push_back(std::move(Image));
    mVulkanPerImageDescriptorSets.push_back(std::move(Desset));
#else
    Up<ImageType> Image = MakeUp<ImageType>(mInstance);
    Image->Setup(inFileName);
    ui CurrentIndex = mImages.size();
    Image->Register(0, 0, CurrentIndex, *mVarDesVulkanDescriptorSet);
    outIndex = mImages.size();
    mImages.push_back(std::move(Image));
#endif
}

void Shape::AddImage(ui inWidth, ui inHeight, ui &outIndex) {
#ifndef JKR_USE_VARIABLE_DES_INDEXING
    Up<VulkanDescriptorSet> Desset = MakeUp<VulkanDescriptorSet>(
         mInstance.GetDevice(),
         mInstance.GetDescriptorPool(),
         mPainterCaches[FillType::Image]->GetVertexFragmentDescriptorSetLayout());
    Up<ImageType> Image = MakeUp<ImageType>(mInstance);
    v<uint8_t> image;
    image.resize(inWidth * inHeight * 4);
    void *Data = image.data();
    Image->Setup(reinterpret_cast<void **>(&Data), inWidth, inHeight, 4);
    Image->Register(0, 0, 0, *Desset);
    outIndex = mImages.size();
    mImages.push_back(std::move(Image));
    mVulkanPerImageDescriptorSets.push_back(std::move(Desset));
#else
    Up<ImageType> Image = MakeUp<ImageType>(mInstance);
    v<uint8_t> image;
    image.resize(inWidth * inHeight * 4);
    void *data = image.data();
    Image->Setup(&data, inWidth, inHeight, 4);
    ui CurrentIndex = mImages.size();
    Image->Register(0, 0, CurrentIndex, *mVarDesVulkanDescriptorSet);
    outIndex = mImages.size();
    mImages.push_back(std::move(Image));
#endif
}

void Shape::AddImage(v<uc> &inImage, ui inWidth, ui inHeight, ui &outIndex) {
#ifndef JKR_USE_VARIABLE_DES_INDEXING
    Up<VulkanDescriptorSet> Desset = MakeUp<VulkanDescriptorSet>(
         mInstance.GetDevice(),
         mInstance.GetDescriptorPool(),
         mPainterCaches[FillType::Image]->GetVertexFragmentDescriptorSetLayout());
    Up<ImageType> Image = MakeUp<ImageType>(mInstance);
    void *data          = inImage.data();
    Image->Setup(reinterpret_cast<void **>(&data), inWidth, inHeight, 4);
    Image->Register(0, 0, 0, *Desset);
    outIndex = mImages.size();
    mImages.push_back(std::move(Image));
    mVulkanPerImageDescriptorSets.push_back(std::move(Desset));
#else
    Up<ImageType> Image = MakeUp<ImageType>(mInstance);
    void *data          = inImage.data();
    Image->Setup(&data, inWidth, inHeight, 4);
    ui CurrentIndex = mImages.size();
    Image->Register(0, 0, CurrentIndex, *mVarDesVulkanDescriptorSet);
    outIndex = mImages.size();
    mImages.push_back(std::move(Image));
#endif
}

void Shape::UpdateImage(ui inId, v<uc> &inImage, ui inWidth, ui inHeight) {
#ifndef JKR_USE_VARIABLE_DES_INDEXING
    Up<ImageType> Image     = MakeUp<ImageType>(mInstance);
    void *data              = inImage.data();
    Image->mUniformImagePtr = mu<VulkanImageVMA>(
         mInstance.GetVMA(), mInstance.GetDevice(), inWidth, inHeight, 4, ImageContext::Default);
    // Image->Setup(reinterpret_cast<void **>(&data), inWidth, inHeight, 4);
    Image->Register(0, 0, 0, *mVulkanPerImageDescriptorSets[inId]);
    mImages[inId] = std::move(Image);
#else
    Up<ImageType> Image = MakeUp<ImageType>(mInstance);
    void *data          = inImage.data();
    Image->Setup(&data, inWidth, inHeight, 4);
    Image->Register(0, 0, inId, *mVarDesVulkanDescriptorSet);
    mImages[inId] = std::move(Image);
#endif
}

void Shape::CopyFromImage(uint32_t inId, CustomPainterImage &inPainterImage) {
    CopyFromImage(inId,
                  inPainterImage.GetPainterParam().GetStorageImagePtr()->GetImageExtent().width,
                  inPainterImage.GetPainterParam().GetStorageImagePtr()->GetImageExtent().height,
                  inPainterImage);
}

void Shape::CopyFromImage(uint32_t inId,
                          uint32_t inWidth,
                          uint32_t inHeight,
                          CustomPainterImage &inPainterImage) {
    mImagesToBeCopiedIds.push_back(inId);
    mImagesToBeCopiedFrom.push_back(inPainterImage.GetPainterParam().GetStorageImagePtr());

    mImageCopyCommands.push_back(
         [&](const ksai::VulkanCommandBuffer &inCmd,
             ksai::VulkanImageBase &inDstImage, // TODO: improve this, here this is flipped
             ksai::VulkanImageBase &inSrcImage) {
             auto srcImgProp = inSrcImage.GetImageProperty();
             auto dstImgProp = inDstImage.GetImageProperty();
             vk::ImageSubresourceLayers SrcSubresource(
                  srcImgProp.mImageAspect, 0, 0, srcImgProp.mArrayLayers);
             vk::ImageSubresourceLayers DstSubresource(
                  dstImgProp.mImageAspect, 0, 0, dstImgProp.mArrayLayers);
             vk::ImageCopy ImageCopy(SrcSubresource,
                                     vk::Offset3D(0, 0, 0),
                                     DstSubresource,
                                     vk::Offset3D(0, 0, 0),
                                     vk::Extent3D(srcImgProp.mExtent, 1));

             // src is sampled image
             //  dst is compute image

             inSrcImage.CmdTransitionImageLayout(inCmd,
                                                 vk::ImageLayout::eGeneral,
                                                 vk::ImageLayout::eTransferSrcOptimal,
                                                 vk::PipelineStageFlagBits::eFragmentShader,
                                                 vk::PipelineStageFlagBits::eTransfer,
                                                 vk::AccessFlagBits::eShaderWrite,
                                                 vk::AccessFlagBits::eMemoryRead);

             /* Destination Image From General To Dst Optimal  */
             inDstImage.CmdTransitionImageLayout(inCmd,
                                                 vk::ImageLayout::eGeneral,
                                                 vk::ImageLayout::eTransferDstOptimal,
                                                 vk::PipelineStageFlagBits::eComputeShader,
                                                 vk::PipelineStageFlagBits::eTransfer,
                                                 vk::AccessFlagBits::eMemoryWrite,
                                                 vk::AccessFlagBits::eMemoryWrite |
                                                      vk::AccessFlagBits::eMemoryRead);

             inCmd.GetCommandBufferHandle().copyImage(inSrcImage.GetImageHandle(),
                                                      vk::ImageLayout::eTransferSrcOptimal,
                                                      inDstImage.GetImageHandle(),
                                                      vk::ImageLayout::eTransferDstOptimal,
                                                      ImageCopy);

             inDstImage.CmdTransitionImageLayout(inCmd,
                                                 vk::ImageLayout::eTransferDstOptimal,
                                                 vk::ImageLayout::eGeneral,
                                                 vk::PipelineStageFlagBits::eTransfer,
                                                 vk::PipelineStageFlagBits::eComputeShader,
                                                 vk::AccessFlagBits::eMemoryWrite,
                                                 vk::AccessFlagBits::eShaderRead |
                                                      vk::AccessFlagBits::eShaderWrite);

             /* Source Image From Src Optimal To General  */
             inSrcImage.CmdTransitionImageLayout(inCmd,
                                                 vk::ImageLayout::eTransferSrcOptimal,
                                                 vk::ImageLayout::eGeneral,
                                                 vk::PipelineStageFlagBits::eTransfer,
                                                 vk::PipelineStageFlagBits::eFragmentShader,
                                                 vk::AccessFlagBits::eMemoryRead,
                                                 vk::AccessFlagBits::eShaderRead);
         });
}

void Shape::CopyToImage(ui inId, ui inWidth, ui inHeight, CustomPainterImage &inPainterImage) {
    mImagesToBeCopiedIds.push_back(inId);
    mImagesToBeCopiedFrom.push_back(inPainterImage.GetPainterParam().GetStorageImagePtr());

    mImageCopyCommands.push_back([&](const ksai::VulkanCommandBuffer &inCmd,
                                     ksai::VulkanImageBase &inSrcImage,
                                     ksai::VulkanImageBase &inDstImage) {
        auto srcImgProp = inSrcImage.GetImageProperty();
        auto dstImgProp = inDstImage.GetImageProperty();
        vk::ImageSubresourceLayers SrcSubresource(
             srcImgProp.mImageAspect, 0, 0, srcImgProp.mArrayLayers);
        vk::ImageSubresourceLayers DstSubresource(
             dstImgProp.mImageAspect, 0, 0, dstImgProp.mArrayLayers);
        vk::ImageCopy ImageCopy(SrcSubresource,
                                vk::Offset3D(0, 0, 0),
                                DstSubresource,
                                vk::Offset3D(0, 0, 0),
                                vk::Extent3D(srcImgProp.mExtent, 1));

        inSrcImage.CmdTransitionImageLayout(inCmd,
                                            vk::ImageLayout::eGeneral,
                                            vk::ImageLayout::eTransferSrcOptimal,
                                            vk::PipelineStageFlagBits::eComputeShader,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            vk::AccessFlagBits::eMemoryWrite,
                                            vk::AccessFlagBits::eMemoryRead);

        /* Destination Image From General To Dst Optimal  */
        inDstImage.CmdTransitionImageLayout(inCmd,
                                            vk::ImageLayout::eGeneral,
                                            vk::ImageLayout::eTransferDstOptimal,
                                            vk::PipelineStageFlagBits::eFragmentShader,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            vk::AccessFlagBits::eShaderRead,
                                            vk::AccessFlagBits::eMemoryWrite);

        inCmd.GetCommandBufferHandle().copyImage(inSrcImage.GetImageHandle(),
                                                 vk::ImageLayout::eTransferSrcOptimal,
                                                 inDstImage.GetImageHandle(),
                                                 vk::ImageLayout::eTransferDstOptimal,
                                                 ImageCopy);

        inDstImage.CmdTransitionImageLayout(inCmd,
                                            vk::ImageLayout::eTransferDstOptimal,
                                            vk::ImageLayout::eGeneral,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            vk::PipelineStageFlagBits::eFragmentShader,
                                            vk::AccessFlagBits::eMemoryWrite,
                                            vk::AccessFlagBits::eShaderRead);

        /* Source Image From Src Optimal To General  */
        inSrcImage.CmdTransitionImageLayout(inCmd,
                                            vk::ImageLayout::eTransferSrcOptimal,
                                            vk::ImageLayout::eGeneral,
                                            vk::PipelineStageFlagBits::eTransfer,
                                            vk::PipelineStageFlagBits::eComputeShader,
                                            vk::AccessFlagBits::eMemoryRead,
                                            vk::AccessFlagBits::eMemoryWrite);
    });
}

void Shape::Update(ui inId, Jkr::Generator &inShape, float inX, float inY, float inZ) {
    auto OffsetId = inId;
    sb::Update(inShape, inId, inX, inY, inZ);
#ifndef JKR_NO_STAGING_BUFFERS
    rb::CopyToStagingBuffers(sb::GetVertexBufferData(),
                             sb::GetIndexBufferData(),
                             sb::VertexCountToBytes(sb::GetVertexOffsetAbsolute(OffsetId)),
                             sb::IndexCountToBytes(sb::GetIndexOffsetAbsolute(OffsetId)),
                             sb::VertexCountToBytes(inShape.GetVertexCount()),
                             sb::IndexCountToBytes(inShape.GetIndexCount()));
    rb::RegisterBufferCopyRegionToPrimitiveFromStaging(
         sb::VertexCountToBytes(sb::GetVertexOffsetAbsolute(OffsetId)),
         sb::IndexCountToBytes(sb::GetIndexOffsetAbsolute(OffsetId)),
         sb::VertexCountToBytes(inShape.GetVertexCount()),
         sb::IndexCountToBytes(inShape.GetIndexCount()));
#else
    rb::CopyToPrimitive(*mPrimitive,
                        sb::GetVertexBufferData(),
                        sb::GetIndexBufferData(),
                        sb::VertexCountToBytes(sb::GetVertexOffsetAbsolute(OffsetId)),
                        sb::IndexCountToBytes(sb::GetIndexOffsetAbsolute(OffsetId)),
                        sb::VertexCountToBytes(inShape.GetVertexCount()),
                        sb::IndexCountToBytes(inShape.GetIndexCount()));
#endif
}

void Shape::Dispatch(Window_base &inWindow, CmdParam inParam) {
#ifndef JKR_NO_STAGING_BUFFERS
    if (!rb::IsCopyRegionsEmpty()) {
        rb::CmdCopyToPrimitiveFromStagingBuffer(
             mInstance,
             *mPrimitive,
             inWindow,
             sb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
             sb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
    }
#endif

    for (ui i = 0; i < mImagesToBeCopiedIds.size(); i++) {
        auto &func     = mImageCopyCommands[i];
        auto id        = mImagesToBeCopiedIds[i];
        auto &srcImage = mImagesToBeCopiedFrom[i];

        const ksai::VulkanCommandBuffer &cmd =
             inWindow.GetCommandBuffers(inParam)[inWindow.GetCurrentFrame()];
        func(cmd, *srcImage, mImages[id]->GetUniformImage());
    }
    if (mImagesToBeCopiedIds.size() != 0) {
        mImagesToBeCopiedFrom.clear();
        mImageCopyCommands.clear();
        mImagesToBeCopiedIds.clear();
    }
}

void Shape::BindFillMode(FillType inFillType, Window_base &inWindow, CmdParam inParam) {
#ifdef JKR_USE_VARIABLE_DES_INDEXING
    auto &Cmd = inWindow.GetCommandBuffers(inParam)[inWindow.GetCurrentFrame()];
    Cmd.GetCommandBufferHandle().bindDescriptorSets(
         vk::PipelineBindPoint::eGraphics,
         mPainterCaches[FillType::Image]
              ->GetVertexFragmentPipelineLayout()
              .GetPipelineLayoutHandle(),
         0,
         mVarDesVulkanDescriptorSet->GetDescriptorSetHandle(),
         {});
#endif
    mPainters[inFillType]->BindDrawPipeline(*mPrimitive, inWindow);
    mCurrentFillMode = inFillType;
}

void Shape::BindImage(Window_base &inWindow, ui inImageId, CmdParam inParam) {
    // TODO Remove
    if (inImageId != -1) {
#ifndef JKR_USE_VARIABLE_DES_INDEXING
        auto &Cmd = inWindow.GetCommandBuffers(inParam)[inWindow.GetCurrentFrame()];
        Cmd.GetCommandBufferHandle().bindDescriptorSets(
             vk::PipelineBindPoint::eGraphics,
             mPainterCaches[FillType::Image]
                  ->GetVertexFragmentPipelineLayout()
                  .GetPipelineLayoutHandle(),
             0,
             mVulkanPerImageDescriptorSets[inImageId]->GetDescriptorSetHandle(),
             {});
#else
        mCurrentImageIndex = inImageId;
#endif
    }
}

void Shape::BindShapes(Window_base &inWindow, CmdParam inParam) {
    mPrimitive->Bind(mInstance, inWindow, inParam);
}

void Shape::Draw(Window_base &inWindow,
                 glm::vec4 inColor,
                 ui inWindowW,
                 ui inWindowH,
                 ui inStartShapeId,
                 ui inEndShapeId,
                 glm::mat4 inMatrix) {
    glm::mat4 Matrix = glm::ortho(0.0f,
                                  static_cast<float>(inWindowW),
                                  0.0f,
                                  static_cast<float>(inWindowH),
                                  100.0f,
                                  -100.0f) *
                       inMatrix;
    PushConstant Push;
    Push.mColor  = inColor;
    Push.mMatrix = Matrix;

#ifdef JKR_USE_VARIABLE_DES_INDEXING
    Push.mParams.r = mCurrentImageIndex;
#endif

    mPainters[mCurrentFillMode]->Draw_EXT<PushConstant>(
         Push,
         inWindow,
         sb::GetEndIndexOffsetAbsolute(inEndShapeId) -
              sb::GetIndexOffsetAbsolute(inStartShapeId), // TODO
         1,
         sb::GetIndexOffsetAbsolute(inStartShapeId),
         0);
}

void Shape::CheckAndResize(const Jkr::Generator &inShape) {
    bool ResizeRequired = false;
    while (true) {
        bool ResizeRequiredi = (inShape.GetIndexCount() + sb::GetCurrentIndexOffset() >=
                                mTotalNoOfIndicesRendererCanHold) ||
                               (inShape.GetVertexCount() + sb::GetCurrentVertexOffset() >=
                                mTotalNoOfVerticesRendererCanHold);
        if (ResizeRequiredi) {
            mTotalNoOfVerticesRendererCanHold *= rb::RendererCapacityResizeFactor;
            mTotalNoOfIndicesRendererCanHold *= rb::RendererCapacityResizeFactor;
            ResizeRequired = true;
        } else {
            break;
        }
    }

    if (ResizeRequired) {
        sb::Resize(mTotalNoOfVerticesRendererCanHold, mTotalNoOfIndicesRendererCanHold);
        mPrimitive.reset();
        mPrimitive = MakeUp<Primitive>(mInstance,
                                       sb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
                                       sb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
#ifndef JKR_NO_STAGING_BUFFERS
        rb::ResizeStagingBuffer(mInstance,
                                sb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
                                sb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
        rb::CopyToStagingBuffers(sb::GetVertexBufferData(),
                                 sb::GetIndexBufferData(),
                                 sb::VertexCountToBytes(0),
                                 sb::IndexCountToBytes(0),
                                 sb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
                                 sb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
        rb::RegisterBufferCopyRegionToPrimitiveFromStaging(
             sb::VertexCountToBytes(0),
             sb::IndexCountToBytes(0),
             sb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
             sb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
#else
        rb::CopyToPrimitive(*mPrimitive,
                            sb::GetVertexBufferData(),
                            sb::GetIndexBufferData(),
                            sb::VertexCountToBytes(0),
                            sb::IndexCountToBytes(0),
                            sb::VertexCountToBytes(mTotalNoOfVerticesRendererCanHold),
                            sb::IndexCountToBytes(mTotalNoOfIndicesRendererCanHold));
#endif
    }
}

ksai::ui Jkr::Renderer::Shape::AddEXT(Jkr::Generator &inShape, glm::vec3 inPosition) {
    ui i;
    Add(inShape, inPosition.x, inPosition.y, inPosition.z, i);
    return i;
}

void Jkr::Renderer::Shape::CopyToImage(uint32_t inId, CustomPainterImage &inPainterImage) {
    CopyToImage(inId,
                inPainterImage.GetPainterParam().GetStorageImagePtr()->GetImageExtent().width,
                inPainterImage.GetPainterParam().GetStorageImagePtr()->GetImageExtent().height,
                inPainterImage);
}

glm::vec2 Jkr::Renderer::Shape::GetImageSize(const sv inFileName) {
    int x;
    int y;
    int comp;
    stbi_info(inFileName.data(), &x, &y, &comp);
    return glm::vec2{x, y};
}

ksai::ui Jkr::Renderer::Shape::AddImageEXT(const std::string_view inFileName) {
    ui i;
    AddImage(inFileName, i);
    return i;
}

ksai::ui Jkr::Renderer::Shape::AddImageEXT(uint32_t inWidth, uint32_t inHeight) {
    ui i;
    AddImage(inWidth, inHeight, i);
    return i;
}

ksai::ui Jkr::Renderer::Shape::AddImageEXT(v<uc> inImage, ui inWidth, ui inHeight) {
    ui i;
    AddImage(inImage, inWidth, inHeight, i);
    return i;
}

void Jkr::Renderer::Shape::UpdateEXT(ui inId, Jkr::Generator &inShape, glm::vec3 inPosition) {
    Update(inId, inShape, inPosition.x, inPosition.y, inPosition.z);
}

void Jkr::Renderer::Shape::DrawEXT(Window_base &inWindow,
                                   glm::vec4 inColor,
                                   ui inStartShapeId,
                                   ui inEndShapeId,
                                   glm::mat4 inMatrix,
                                   CmdParam inParam) {
    PushConstant Push;
    Push.mColor  = inColor;
    Push.mMatrix = inMatrix;

#ifdef JKR_USE_VARIABLE_DES_INDEXING
    Push.mParams.r = mCurrentImageIndex;
#endif

    mPainters[mCurrentFillMode]->Draw_EXT<PushConstant>(
         Push,
         inWindow,
         sb::GetEndIndexOffsetAbsolute(inEndShapeId) -
              sb::GetIndexOffsetAbsolute(inStartShapeId), // TODO
         1,
         sb::GetIndexOffsetAbsolute(inStartShapeId),
         0,
         inParam);
}
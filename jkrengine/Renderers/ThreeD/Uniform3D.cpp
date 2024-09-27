#include "Uniform3D.hpp"
#include "Global/Standards.hpp"
#include "Renderers/ThreeD/glTF_Model.hpp"
#include "TestUtils.hpp"
#include "PBR.hpp"
#include "Uniform3D.hpp"
#include "World3D.hpp"

namespace Jkr::Renderer {

Up<VulkanImageVMA> PBR::GenerateBRDFLookupTable(Instance &inInstance,
                                                Window &inWindow,
                                                std::string_view inFileName,
                                                std::string_view inVertexShader,
                                                std::string_view inFragmentShader,
                                                std::string_view inComputeShader,
                                                bool inShouldLoad) {
    const int Dim                   = 512;
    Up<VulkanImageVMA> BRDFLUTImage = MakeUp<VulkanImageVMA>(
         inInstance.GetVMA(),
         inInstance.GetDevice(),
         Dim,
         Dim,
         ImageContext::Default,
         4,
         1,
         1,
         1,
         vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eColorAttachment);
    VulkanRenderPass<RenderPassContext::SingleColorAttachment> RenderPass(inInstance.GetDevice(),
                                                                          *BRDFLUTImage);
    VulkanFrameBuffer FrameBuffer(inInstance.GetDevice(), RenderPass, *BRDFLUTImage);

    _3D::Simple3D Simple3D(inInstance, inWindow);
    Simple3D.CompileWithCustomRenderPass(inInstance,
                                         inWindow,
                                         RenderPass,
                                         inFileName,
                                         inVertexShader,
                                         inFragmentShader,
                                         inComputeShader,
                                         inShouldLoad);

    VulkanCommandBuffer CommandBuffer(inInstance.GetDevice(), inInstance.GetCommandPool());

    CommandBuffer.Begin();
    CommandBuffer.BeginRenderPass(
         RenderPass, vk::Extent2D{Dim, Dim}, FrameBuffer, std::array<float, 5>{1, 1, 1, 1, 1});

    Simple3D.BindByCommandBuffer(CommandBuffer);
    CommandBuffer.GetCommandBufferHandle().draw(3, 1, 0, 0);

    CommandBuffer.EndRenderPass();

    BRDFLUTImage->CmdTransitionImageLayout(CommandBuffer,
                                           vk::ImageLayout::eShaderReadOnlyOptimal,
                                           vk::ImageLayout::eGeneral,
                                           vk::PipelineStageFlagBits::eAllCommands,
                                           vk::PipelineStageFlagBits::eBottomOfPipe,
                                           vk::AccessFlagBits::eNone,
                                           vk::AccessFlagBits::eNone);

    CommandBuffer.End();
    inInstance.GetGraphicsQueue().Submit<SubmitContext::SingleTime>(CommandBuffer);
    inInstance.GetGraphicsQueue().Wait();
    return mv(BRDFLUTImage);
}

Up<VulkanImageVMA> PBR::GenerateIrradianceCube(Instance &inInstance,
                                               Window &inWindow,
                                               _3D::Shape &inShape,
                                               int inSkyboxModelIndex,
                                               VulkanImageBase &inEnvironmentCubeMap,
                                               std::string_view inFileName,
                                               std::string_view inVertexShader,
                                               std::string_view inFragmentShader,
                                               std::string_view inComputeShader,
                                               bool inShouldLoad,
                                               _3D::World3D &inWorld3D) {
    const uint32_t Dim     = 64;
    const uint32_t NumMips = static_cast<uint32_t>(floor(log2(Dim))) + 1;
    auto IrradianceCubeMap = MakeUp<VulkanImageVMA>(inInstance.GetVMA(),
                                                    inInstance.GetDevice(),
                                                    Dim,
                                                    Dim,
                                                    ImageContext::CubeCompatible,
                                                    4,
                                                    6,
                                                    1,
                                                    NumMips,
                                                    vk::ImageUsageFlagBits::eTransferDst,
                                                    vk::ImageLayout::eUndefined);

    VulkanImageVMA OffscreenFBufferImage(inInstance.GetVMA(),
                                         inInstance.GetDevice(),
                                         Dim,
                                         Dim,
                                         ImageContext::Default,
                                         4,
                                         1,
                                         1,
                                         1,
                                         vk::ImageUsageFlagBits::eTransferSrc |
                                              vk::ImageUsageFlagBits::eColorAttachment,
                                         vk::ImageLayout::eUndefined);
    VulkanRenderPass<RenderPassContext::SingleColorAttachment> RenderPass(inInstance.GetDevice(),
                                                                          OffscreenFBufferImage);
    VulkanFrameBuffer FrameBuffer(inInstance.GetDevice(), RenderPass, OffscreenFBufferImage);

    VulkanCommandBuffer Cmd(inInstance.GetDevice(), inInstance.GetCommandPool());

    //     Cmd.Begin();
    //     OffscreenFBufferImage.CmdTransitionImageLayout(
    //          Cmd,
    //          OffscreenFBufferImage.GetCurrentImageLayout(),
    //          vk::ImageLayout::eColorAttachmentOptimal,
    //          vk::PipelineStageFlagBits::eAllCommands,
    //          vk::PipelineStageFlagBits::eAllCommands, // TODO Improve this
    //          vk::AccessFlagBits::eNone,
    //          vk::AccessFlagBits::eNone);
    //     Cmd.End();
    //     inInstance.GetGraphicsQueue().Submit<SubmitContext::SingleTime>(Cmd);
    //     inInstance.GetGraphicsQueue().Wait();

    _3D::Simple3D Simple3D(inInstance, inWindow);
    Simple3D.CompileWithCustomRenderPass(inInstance,
                                         inWindow,
                                         RenderPass,
                                         inFileName,
                                         inVertexShader,
                                         inFragmentShader,
                                         inComputeShader,
                                         inShouldLoad);

    VulkanDescriptorPool DescriptorPool(inInstance.GetDevice(), 10, 10);
    _3D::Uniform3D Uniform(inInstance);
    Uniform.Build(Simple3D, DescriptorPool);
    inWorld3D.AddWorldInfoToUniform3DEXT(Uniform);

    VulkanDescriptorUpdateHandler Handler(inInstance.GetDevice());
    VulkanSampler Sampler(
         inInstance.GetDevice(), ImageContext::Default, 0.0f, static_cast<float>(NumMips));

    Handler.RW(ImageContext::Default,
               Uniform.GetVulkanDescriptorSet(),
               inEnvironmentCubeMap,
               Sampler,
               kstd::BindingIndex::Uniform::CubeMapImage,
               0,
               1);

    using namespace glm;

    struct PushBlock {
        mat4 mvp;
        mat4 m2 = mat4(vec4(0), vec4(0), vec4(0), vec4(0));
    } PushBlock;

    // float DeltaPhi = (2.0f * float(M_PI)) / 180.0f;
    //  float DeltaTheta                = (0.5f * float(M_PI)) / 64.0f;
    float DeltaPhi                  = (2.0f * float(M_PI)) / 32.0f;
    float DeltaTheta                = (0.5f * float(M_PI)) / 16.0f;

    PushBlock.m2[0].x               = DeltaPhi;
    PushBlock.m2[0].y               = DeltaTheta;

    std::vector<glm::mat4> matrices = {
         // POSITIVE_X
         glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
                     glm::radians(180.0f),
                     glm::vec3(1.0f, 0.0f, 0.0f)),
         // NEGATIVE_X
         glm::rotate(
              glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
              glm::radians(180.0f),
              glm::vec3(1.0f, 0.0f, 0.0f)),

         // POSITIVE_Y
         glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
         // NEGATIVE_Y
         glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),

         // POSITIVE_Z
         glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
         // NEGATIVE_Z
         glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    };

    inWorld3D.UpdateWorldInfoToUniform3D(Uniform);

    Cmd.Begin();

    vk::Viewport Viewport((float)Dim, (float)Dim, 0.0f, 1.0f);
    vk::Rect2D Scissor(vk::Offset2D{0}, vk::Extent2D{Dim, Dim});

    auto Cmdh = Cmd.GetCommandBufferHandle();
    for (uint32_t m = 0; m < NumMips; m++) {
        for (uint32_t f = 0; f < 6; f++) {
            Viewport.width  = static_cast<float>(Dim * std::pow(0.5f, m));
            Viewport.height = static_cast<float>(Dim * std::pow(0.5f, m));
            Cmdh.setViewport(0, Viewport);
            Cmdh.setScissor(0, Scissor);
            Cmd.BeginRenderPass(RenderPass,
                                vk::Extent2D(Dim, Dim),
                                FrameBuffer,
                                std::array<float, 5>{0.0f, 0.0f, 0.2f, 0.0f});

            PushBlock.mvp = glm::perspective((float)(M_PI / 2.0), 1.0f, 0.1f, 512.0f) * matrices[f];
            inShape.GetPrimitive().GetVertexBufferPtr()->Bind<BufferContext::Vertex>(Cmd);
            inShape.GetPrimitive().GetIndexBufferPtr()->Bind<BufferContext::Index>(Cmd);
            Simple3D.BindByCommandBuffer(Cmd);

            Uniform.GetVulkanDescriptorSet().Bind(
                 vk::PipelineBindPoint::eGraphics,
                 Cmd,
                 Simple3D.GetPainterCache().GetVertexFragmentPipelineLayout(),
                 0);
            Uniform.GetVulkanDescriptorSet().Bind(
                 vk::PipelineBindPoint::eGraphics,
                 Cmd,
                 Simple3D.GetPainterCache().GetVertexFragmentPipelineLayout(),
                 1);
            Simple3D.DrawByCommandBuffer(
                 Cmd, inShape, PushBlock, 0, inShape.GetIndexCount(inSkyboxModelIndex), 1);
            Cmd.EndRenderPass();

            //   IrradianceCubeMap->GetImagePropertyRef().mInitialImageLayout =
            //        vk::ImageLayout::eUndefined;
            //   OffscreenFBufferImage.GetImagePropertyRef().mInitialImageLayout =
            //        vk::ImageLayout::eUndefined;

            IrradianceCubeMap->CmdCopyImageFromImageAfterStage(
                 Cmd,
                 inInstance.GetDevice(),
                 OffscreenFBufferImage,
                 vk::PipelineStageFlagBits::eFragmentShader,
                 vk::AccessFlagBits::eNone,
                 static_cast<int>(Viewport.width),
                 static_cast<int>(Viewport.height),
                 0,
                 0,
                 m,
                 f,
                 1,
                 vk::ImageLayout::eGeneral,
                 vk::ImageLayout::eGeneral,
                 vk::ImageLayout::eShaderReadOnlyOptimal);
        }
    }

    IrradianceCubeMap->CmdTransitionImageLayout(Cmd,
                                                IrradianceCubeMap->GetCurrentImageLayout(),
                                                vk::ImageLayout::eShaderReadOnlyOptimal,
                                                vk::PipelineStageFlagBits::eAllCommands,
                                                vk::PipelineStageFlagBits::eBottomOfPipe,
                                                vk::AccessFlagBits::eNone,
                                                vk::AccessFlagBits::eNone);

    //     IrradianceCubeMap->GetImagePropertyRef().mInitialImageLayout =
    //          vk::ImageLayout::eShaderReadOnlyOptimal;

    Cmd.End();
    inInstance.GetGraphicsQueue().Submit<SubmitContext::SingleTime>(Cmd);

    return mv(IrradianceCubeMap);
}

Up<VulkanImageVMA> PBR::GeneratePrefilteredCube(Instance &inInstance,
                                                Window &inWindow,
                                                _3D::Shape &inShape,
                                                int inSkyboxModelIndex,
                                                VulkanImageBase &inEnvironmentCubeMap,
                                                std::string_view inFileName,
                                                std::string_view inVertexShader,
                                                std::string_view inFragmentShader,
                                                std::string_view inComputeShader,
                                                bool inShouldLoad,
                                                _3D::World3D &inWorld3D) {
    const int32_t Dim            = 512;
    const uint32_t NumMips       = static_cast<uint32_t>(floor(log2(Dim))) + 1;
    auto PrefilteredCubeMapImage = MakeUp<VulkanImageVMA>(inInstance.GetVMA(),
                                                          inInstance.GetDevice(),
                                                          Dim,
                                                          Dim,
                                                          ImageContext::CubeCompatible,
                                                          4,
                                                          6,
                                                          1,
                                                          NumMips,
                                                          vk::ImageUsageFlagBits::eTransferDst,
                                                          vk::ImageLayout::eUndefined);

    VulkanImageVMA OffscreenImage(inInstance.GetVMA(),
                                  inInstance.GetDevice(),
                                  Dim,
                                  Dim,
                                  ImageContext::Default,
                                  4,
                                  1,
                                  1,
                                  1,
                                  vk::ImageUsageFlagBits::eColorAttachment |
                                       vk::ImageUsageFlagBits::eTransferSrc,
                                  vk::ImageLayout::eUndefined);
    VulkanRenderPass<RenderPassContext::SingleColorAttachment> RenderPass(inInstance.GetDevice(),
                                                                          OffscreenImage);
    VulkanFrameBuffer FrameBuffer(inInstance.GetDevice(), RenderPass, OffscreenImage);

    VulkanCommandBuffer Cmd(inInstance.GetDevice(), inInstance.GetCommandPool());

    Cmd.Begin();
    OffscreenImage.CmdTransitionImageLayout(
         Cmd,
         vk::ImageLayout::eUndefined,
         vk::ImageLayout::eColorAttachmentOptimal,
         vk::PipelineStageFlagBits::eAllCommands,
         vk::PipelineStageFlagBits::eBottomOfPipe, // TODO Improve this
         vk::AccessFlagBits::eNone,
         vk::AccessFlagBits::eNone);
    Cmd.End();
    inInstance.GetGraphicsQueue().Submit<SubmitContext::SingleTime>(Cmd);
    inInstance.GetGraphicsQueue().Wait();

    _3D::Simple3D Simple3D(inInstance, inWindow);
    Simple3D.CompileWithCustomRenderPass(inInstance,
                                         inWindow,
                                         RenderPass,
                                         inFileName,
                                         inVertexShader,
                                         inFragmentShader,
                                         inComputeShader,
                                         inShouldLoad);

    VulkanDescriptorPool DescriptorPool(inInstance.GetDevice(), 10, 10);
    _3D::Uniform3D Uniform(inInstance);
    Uniform.Build(Simple3D, DescriptorPool);
    inWorld3D.AddWorldInfoToUniform3DEXT(Uniform);

    VulkanDescriptorUpdateHandler Handler(inInstance.GetDevice());
    VulkanSampler Sampler(
         inInstance.GetDevice(), ImageContext::Default, 0.0f, static_cast<float>(NumMips));

    Handler.RW(ImageContext::Default,
               Uniform.GetVulkanDescriptorSet(),
               inEnvironmentCubeMap,
               Sampler,
               kstd::BindingIndex::Uniform::CubeMapImage,
               0,
               1);

    struct PushBlock {
        glm::mat4 mvp;
        glm::mat4 m2;
    } PushBlock;
    constexpr int NumSamples = 32;

    PushBlock.m2[0].y        = NumSamples;
    using namespace glm;

    std::vector<glm::mat4> Matrices = {
         // POSITIVE_X
         glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
                     glm::radians(180.0f),
                     glm::vec3(1.0f, 0.0f, 0.0f)),
         // NEGATIVE_X
         glm::rotate(
              glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)),
              glm::radians(180.0f),
              glm::vec3(1.0f, 0.0f, 0.0f)),

         // POSITIVE_Y
         glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
         // NEGATIVE_Y
         glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),

         // POSITIVE_Z
         glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
         // NEGATIVE_Z
         glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    };

    inWorld3D.UpdateWorldInfoToUniform3D(Uniform);

    Cmd.Begin();

    vk::Viewport Viewport((float)Dim, (float)Dim, 0.0f, 1.0f);
    vk::Rect2D Scissor(vk::Offset2D{0}, vk::Extent2D{Dim, Dim});

    auto Cmdh = Cmd.GetCommandBufferHandle();
    for (uint32_t m = 0; m < NumMips; m++) {
        float roughness   = (float)m / (float)(NumMips - 1);
        PushBlock.m2[0].x = roughness;
        for (uint32_t f = 0; f < 6; f++) {
            Viewport.width  = static_cast<float>(Dim * std::pow(0.5f, m));
            Viewport.height = static_cast<float>(Dim * std::pow(0.5f, m));
            Cmdh.setViewport(0, Viewport);
            Cmdh.setScissor(0, Scissor);
            PushBlock.mvp = glm::perspective((float)(M_PI / 2.0), 1.0f, 0.1f, 512.0f) * Matrices[f];

            Cmd.BeginRenderPass(RenderPass,
                                vk::Extent2D{Dim, Dim},
                                FrameBuffer,
                                std::array<float, 5>{0.0f, 0.0f, 0.2f, 0.0f, 1.0f});

            inShape.GetPrimitive().GetVertexBufferPtr()->Bind<BufferContext::Vertex>(Cmd);
            inShape.GetPrimitive().GetIndexBufferPtr()->Bind<BufferContext::Index>(Cmd);

            Simple3D.BindByCommandBuffer(Cmd);

            Uniform.GetVulkanDescriptorSet().Bind(
                 vk::PipelineBindPoint::eGraphics,
                 Cmd,
                 Simple3D.GetPainterCache().GetVertexFragmentPipelineLayout(),
                 0);

            Uniform.GetVulkanDescriptorSet().Bind(
                 vk::PipelineBindPoint::eGraphics,
                 Cmd,
                 Simple3D.GetPainterCache().GetVertexFragmentPipelineLayout(),
                 1);

            Simple3D.DrawByCommandBuffer(
                 Cmd, inShape, PushBlock, 0, inShape.GetIndexCount(inSkyboxModelIndex), 1);
            Cmd.EndRenderPass();

            PrefilteredCubeMapImage->CmdCopyImageFromImageAfterStage(
                 Cmd,
                 inInstance.GetDevice(),
                 OffscreenImage,
                 vk::PipelineStageFlagBits::eFragmentShader,
                 vk::AccessFlagBits::eNone,
                 static_cast<int>(Viewport.width),
                 static_cast<int>(Viewport.height),
                 0,
                 0,
                 m,
                 f,
                 1,
                 vk::ImageLayout::eGeneral,
                 vk::ImageLayout::eGeneral,
                 vk::ImageLayout::eShaderReadOnlyOptimal);
        }
    }

    PrefilteredCubeMapImage->CmdTransitionImageLayout(
         Cmd,
         PrefilteredCubeMapImage->GetCurrentImageLayout(),
         vk::ImageLayout::eShaderReadOnlyOptimal,
         vk::PipelineStageFlagBits::eAllCommands,
         vk::PipelineStageFlagBits::eAllCommands,
         vk::AccessFlagBits::eNone,
         vk::AccessFlagBits::eNone);

    // PrefilteredCubeMapImage->GetImagePropertyRef().mInitialImageLayout =
    //   vk::ImageLayout::eShaderReadOnlyOptimal;
    Cmd.End();
    inInstance.GetGraphicsQueue().Submit<SubmitContext::SingleTime>(Cmd);

    return mv(PrefilteredCubeMapImage);
}
//
} // namespace Jkr::Renderer

using namespace Jkr::Renderer::_3D;
using namespace Jkr;

void Uniform3D::AddTexture(int inDstBinding, s inFileName, ui inDestSet) {
    Up<ImageType> Image = MakeUp<ImageType>(mInstance);
    Image->Setup(inFileName);
    Image->Register(0, inDstBinding, 0, *mVulkanDescriptorSet, inDestSet);
    mImages[inDstBinding] = mv(Image);
}

void Uniform3D::AddTextureByVector(
     int inDstBinding, v<uc> &inImageVector, ui inWidth, ui inHeight, ui inDstSet) {
    Up<ImageType> Image = MakeUp<ImageType>(mInstance);
    void *data          = inImageVector.data();
    Image->Setup(&data, inWidth, inHeight, 4);
    Image->Register(0, inDstBinding, 0, *mVulkanDescriptorSet, inDstSet);
    mImages[inDstBinding] = mv(Image);
}

void Uniform3D::AddUniformBuffer(int inDstBinding, size_t inSize, ui inDstSet) {
    Up<UniformBufferType> Buffer = MakeUp<UniformBufferType>(mInstance);
    Buffer->Setup(inSize);
    Buffer->Register(0, inDstBinding, 0, *mVulkanDescriptorSet, inDstSet);
    mUniformBuffers[inDstBinding] = mv(Buffer);
}

void Uniform3D::AddStorageBuffer(int inDstBinding, size_t inSize, ui inDstSet) {
    Up<StorageBufferType> Buffer = MakeUp<StorageBufferType>(mInstance);
    Buffer->SetupCoherent(inSize);
    Buffer->RegisterCoherent(0, inDstBinding, 0, *mVulkanDescriptorSet, inDstSet);
    mStorageBuffers[inDstBinding] = mv(Buffer);
}
void Uniform3D::AddTextureToUniform3D(Uniform3D &inUniform3D, int inTextureId, ui inDstSet) {
    mImages[inTextureId]->Register(0, inTextureId, 0, *inUniform3D.mVulkanDescriptorSet, inDstSet);
    {};
}
void Uniform3D::AddUniformBufferToUniform3D(Uniform3D &inUniform3D, int inBufferId, ui inDstSet) {
    mUniformBuffers[inBufferId]->Register(
         0, inBufferId, 0, *inUniform3D.mVulkanDescriptorSet, inDstSet);
    {};
}
void Uniform3D::AddStorageBufferToUniform3D(Uniform3D &inUniform3D, int inStorageId, ui inDstSet) {
    mStorageBuffers[inStorageId]->RegisterCoherent(
         0, inStorageId, 0, *inUniform3D.mVulkanDescriptorSet, inDstSet);
    {};
}

void Uniform3D::AddSkyboxImage(SkyboxImageType &inType, int inDstBinding, ui inDstSet) {
    inType.Register(0, inDstBinding, 0, *mVulkanDescriptorSet, inDstSet);
}

/// @todo Move this function to Tools.hpp
void Uniform3D::AddTextureFromShapeImage(Jkr::Renderer::Shape &inShapeRenderer,
                                         int inShapeImageId,
                                         int inDstImageBinding,
                                         int inDstSet) {
    inShapeRenderer.GetImages()[inShapeImageId]->Register(
         0, inDstImageBinding, 0, *mVulkanDescriptorSet, inDstSet);
}

void Uniform3D::UpdateByGLTFAnimation(Renderer::_3D::glTF_Model &inModel) {
    inModel.UpdateAllJoints([&](v<glm::mat4> &inMatrices) {
        void *data = inMatrices.data();
        UpdateStorageBuffer(kstd::BindingIndex::Storage::JointMatrix,
                            &data,
                            inMatrices.size() * sizeof(glm::mat4));
    }

    );
}

Up<Uniform3D> Uniform3D::CreateByGLTFNodeIndex(Instance &inInstance,
                                               Simple3D &inSimple,
                                               Renderer::_3D::glTF_Model &inLoadedModel,
                                               ui inNodeIndex,
                                               bool inShouldSkin) {
    auto Uniform = MakeUp<Uniform3D>(inInstance);
    Uniform->Build(inSimple, inLoadedModel, inNodeIndex, inShouldSkin);
    return mv(Uniform);
};

void Uniform3D::Build(Simple3D &inSimple3D) {
    mVulkanDescriptorSet = MakeUp<VulkanDescriptorSet>(
         mInstance.GetDevice(),
         mInstance.GetDescriptorPool(),
         inSimple3D.GetPainterCache().GetVertexFragmentDescriptorSetLayout());
}

void Uniform3D::Build(Simple3D &inSimple3D, VulkanDescriptorPool &inPool) {
    mVulkanDescriptorSet = MakeUp<VulkanDescriptorSet>(
         mInstance.GetDevice(),
         inPool,
         inSimple3D.GetPainterCache().GetVertexFragmentDescriptorSetLayout());
}

void Uniform3D::Build(Simple3D &inSimple3D,
                      Renderer::_3D::glTF_Model &inModel,
                      ui inNodeIndex,
                      bool inShouldSkin,
                      bool inTangents) {
    if (not mVulkanDescriptorSet) {
        Build(inSimple3D);
    }

    v<kstd::JointInfluence> JointInfluence;
    v<kstd::Tangent> Tangents;
    for (auto &u : inModel.GetVerticesExtRef()) {
        JointInfluence.push_back(kstd::JointInfluence{.mJointIndices = u.mJointIndices,
                                                      .mJointWeights = u.mJointWeights});
        Tangents.push_back(kstd::Tangent{.mTangent = u.mTangent});
    }

    if (inTangents) {
        this->AddStorageBuffer(kstd::BindingIndex::Storage::VertexInfo,
                               Tangents.size() * sizeof(kstd::Tangent));
        void *data = Tangents.data();
        this->UpdateStorageBuffer(kstd::BindingIndex::Storage::VertexInfo,
                                  &data,
                                  Tangents.size() * sizeof(kstd::Tangent));
    }

    if (inShouldSkin) {
        this->AddStorageBuffer(kstd::BindingIndex::Storage::JointInfluence,
                               JointInfluence.size() * sizeof(kstd::JointInfluence));
        void *data = JointInfluence.data();
        this->UpdateStorageBuffer(kstd::BindingIndex::Storage::JointInfluence,
                                  &data,
                                  JointInfluence.size() * sizeof(kstd::JointInfluence));

        if (inModel.GetSkinsSize() != 0) {
            auto &Skins = inModel.GetSkinsRef()[0]; // TODO
                                                    // inSkinIndex
            auto &InverseBindMatrices = Skins.mInverseBindMatrices;
            void *Data                = InverseBindMatrices.data();
            this->AddStorageBuffer(kstd::BindingIndex::Storage::JointMatrix,
                                   InverseBindMatrices.size() * sizeof(glm::mat4));
        }
    }
}

void Uniform3D::AddBindingsToUniform3DGLTF(Uniform3D &modUniform3D,
                                           bool inShouldSkin,
                                           bool inShouldTexture,
                                           ui inSet) {
    if (inShouldSkin) {
        for (auto &u : mUniformBuffers) {
            AddUniformBufferToUniform3D(modUniform3D, u.first, inSet);
        }
        for (auto &s : mStorageBuffers) {
            AddStorageBufferToUniform3D(modUniform3D, s.first, inSet);
        }
    }
    if (inShouldTexture) {
        for (auto &t : mImages) {
            AddTextureToUniform3D(modUniform3D, t.first, inSet);
        }
    }
}

void Uniform3D::UpdateUniformBuffer(int inDstBinding, void **inData, size_t inSize) {
    void *memory = mUniformBuffers[inDstBinding]->GetUniformMappedMemoryRegion();
    memcpy(memory, *inData, inSize);
}

void Uniform3D::UpdateStorageBuffer(int inDstBinding, void **inData, size_t inSize) {
    void *memory = mStorageBuffers[inDstBinding]->GetStorageMappedMemoryRegion();
    memcpy(memory, *inData, inSize);
}

void Uniform3D::Bind(Window_base &inWindow,
                     Simple3D &inSimple3D,
                     int inSet,
                     Window_base::ParameterContext inParam) {
    const VulkanCommandBuffer &Cmd =
         inWindow.GetCommandBuffers(inParam)[inWindow.GetCurrentFrame()];
    mVulkanDescriptorSet->Bind(vk::PipelineBindPoint::eGraphics,
                               Cmd,
                               inSimple3D.GetPainterCache().GetVertexFragmentPipelineLayout(),
                               inSet);
}

// TODO Quite misleading
void Uniform3D::Build(Simple3D &inSimple3D,
                      Renderer::_3D::glTF_Model &inModel,
                      Renderer::_3D::glTF_Model::Primitive &inPrimitive) {
    if (not mVulkanDescriptorSet) {
        Build(inSimple3D);
    }
    BuildByMaterial(inSimple3D, inModel, inPrimitive.mMaterialIndex);
}

void Uniform3D::BuildByMaterial(Simple3D &inSimple3D,
                                Renderer::_3D::glTF_Model &inModel,
                                int inMaterialIndex) {
    if (not mVulkanDescriptorSet) {
        Build(inSimple3D);
    }
    ui BindingIndex    = kstd::BindingIndex::Uniform::Images;
    auto FillByTexture = [&](int inIndex) {
        auto &Image = inModel.GetImagesRef()[inModel.GetTexturesRef()[inIndex].mImageIndex];
        AddTextureByVector(BindingIndex, Image.mTextureImage, Image.mWidth, Image.mHeight);
        BindingIndex++;
    };

    auto &Material = inModel.GetMaterialsRef()[inMaterialIndex];
    if (Material.mBaseColorTextureIndex != -1) {
        FillByTexture(Material.mBaseColorTextureIndex);
    }
    if (Material.mMetallicRoughnessTextureIndex != -1) {
        FillByTexture(Material.mMetallicRoughnessTextureIndex);
    }
    if (Material.mNormalTextureIndex != -1) {
        FillByTexture(Material.mNormalTextureIndex);
    }
    if (Material.mOcclusionTextureIndex != -1) {
        FillByTexture(Material.mOcclusionTextureIndex);
    }
    if (Material.mEmissiveTextureIndex != -1) {
        FillByTexture(Material.mEmissiveTextureIndex);
    }
}

void Uniform3D::AddGenerateBRDFLookupTable(Instance &inInstance,
                                           Window &inWindow,
                                           std::string_view inFileName,
                                           std::string_view inVertexShader,
                                           std::string_view inFragmentShader,
                                           std::string_view inComputeShader,
                                           bool inShouldLoad,
                                           int inDstBinding,
                                           int inDstSet) {
    auto VMAimage                           = Renderer::PBR::GenerateBRDFLookupTable(inInstance,
                                                           inWindow,
                                                           inFileName,
                                                           inVertexShader,
                                                           inFragmentShader,
                                                           inComputeShader,
                                                           inShouldLoad);

    mImages[inDstBinding]                   = MakeUp<ImageType>(inInstance);
    mImages[inDstBinding]->mUniformImagePtr = mv(VMAimage);
    mImages[inDstBinding]->mSampler =
         MakeUp<VulkanSampler>(inInstance.GetDevice(), ImageContext::CubeCompatible);
    mImages[inDstBinding]->Register(0, inDstBinding, 0, *mVulkanDescriptorSet, inDstSet);
}

void Uniform3D::AddGenerateIrradianceCube(Instance &inInstance,
                                          Window &inWindow,
                                          Renderer::_3D::Shape &inShape,
                                          int inSkyboxModelIndex,
                                          VulkanImageBase &inEnvironmentCubeMap,
                                          Renderer::_3D::World3D &inWorld,
                                          std::string_view inFileName,
                                          std::string_view inVertexShader,
                                          std::string_view inFragmentShader,
                                          std::string_view inComputeShader,
                                          bool inShouldLoad,
                                          int inDstBinding,
                                          int inDstSet) {
    auto VMAimage                           = Renderer::PBR::GenerateIrradianceCube(inInstance,
                                                          inWindow,
                                                          inShape,
                                                          inSkyboxModelIndex,
                                                          inEnvironmentCubeMap,
                                                          inFileName,
                                                          inVertexShader,
                                                          inFragmentShader,
                                                          inComputeShader,
                                                          inShouldLoad,
                                                          inWorld);
    mImages[inDstBinding]                   = MakeUp<ImageType>(inInstance);
    mImages[inDstBinding]->mUniformImagePtr = mv(VMAimage);
    mImages[inDstBinding]->mSampler =
         MakeUp<VulkanSampler>(inInstance.GetDevice(), ImageContext::CubeCompatible);
    mImages[inDstBinding]->Register(0, inDstBinding, 0, *mVulkanDescriptorSet, inDstSet);
}

void Uniform3D::AddGeneratePrefilteredCube(Instance &inInstance,
                                           Window &inWindow,
                                           Renderer::_3D::Shape &inShape,
                                           int inSkyboxModelIndex,
                                           VulkanImageBase &inEnvironmentCubeMap,
                                           Renderer::_3D::World3D &inWorld,
                                           std::string_view inFileName,
                                           std::string_view inVertexShader,
                                           std::string_view inFragmentShader,
                                           std::string_view inComputeShader,
                                           bool inShouldLoad,
                                           int inDstBinding,
                                           int inDstSet) {
    auto VMAimage                           = Renderer::PBR::GeneratePrefilteredCube(inInstance,
                                                           inWindow,
                                                           inShape,
                                                           inSkyboxModelIndex,
                                                           inEnvironmentCubeMap,
                                                           inFileName,
                                                           inVertexShader,
                                                           inFragmentShader,
                                                           inComputeShader,
                                                           inShouldLoad,
                                                           inWorld);
    mImages[inDstBinding]                   = MakeUp<ImageType>(inInstance);
    mImages[inDstBinding]->mUniformImagePtr = mv(VMAimage);
    mImages[inDstBinding]->mSampler =
         MakeUp<VulkanSampler>(inInstance.GetDevice(), ImageContext::CubeCompatible);
    mImages[inDstBinding]->Register(0, inDstBinding, 0, *mVulkanDescriptorSet, inDstSet);
}
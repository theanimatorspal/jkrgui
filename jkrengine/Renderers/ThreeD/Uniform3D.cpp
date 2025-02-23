#include "Uniform3D.hpp"
#include "Global/Standards.hpp"
#include "Renderers/ThreeD/glTF_Model.hpp"
#include "TestUtils.hpp"
#include "PBR.hpp"
#include "Uniform3D.hpp"
#include "World3D.hpp"

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
    mAssociatedSimple3D = &inSimple3D;
}

void Uniform3D::Build(Simple3D &inSimple3D, VulkanDescriptorPool &inPool) {
    mVulkanDescriptorSet = MakeUp<VulkanDescriptorSet>(
         mInstance.GetDevice(),
         inPool,
         inSimple3D.GetPainterCache().GetVertexFragmentDescriptorSetLayout());
    mAssociatedSimple3D = &inSimple3D;
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
    VulkanCommandBuffer &Cmd = inWindow.GetCommandBuffers(inParam)[inWindow.GetCurrentFrame()];
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
                                           Jkr::Misc::File& inFile,
                                           int inDstBinding,
                                           int inDstSet) {
    auto VMAimage                           = Renderer::PBR::GenerateBRDFLookupTable(inInstance,
                                                           inWindow,
                                                           inFileName,
                                                           inVertexShader,
                                                           inFragmentShader,
                                                           inComputeShader,
                                                           inFile);

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
                                          Jkr::Misc::File& inFile,
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
                                                          inFile,
                                                          inWorld);
    mImages[inDstBinding]                   = MakeUp<ImageType>(inInstance);
    mImages[inDstBinding]->mUniformImagePtr = mv(VMAimage);
    mImages[inDstBinding]->mSampler         = MakeUp<VulkanSampler>(
         inInstance.GetDevice(),
         ImageContext::CubeCompatible,
         0.0,
         static_cast<float>(
              static_cast<uint32_t>(floor(log2((float)PBR::IrradianceCubeDimension))) + 1));
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
                                           Jkr::Misc::File& inFile,
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
                                                           inFile,
                                                           inWorld);
    mImages[inDstBinding]                   = MakeUp<ImageType>(inInstance);
    mImages[inDstBinding]->mUniformImagePtr = mv(VMAimage);
    mImages[inDstBinding]->mSampler         = MakeUp<VulkanSampler>(
         inInstance.GetDevice(),
         ImageContext::CubeCompatible,
         0.0,
         static_cast<float>(
              static_cast<uint32_t>(floor(log2((float)PBR::PrefilteredCubeDimension))) + 1));
    mImages[inDstBinding]->Register(0, inDstBinding, 0, *mVulkanDescriptorSet, inDstSet);
}
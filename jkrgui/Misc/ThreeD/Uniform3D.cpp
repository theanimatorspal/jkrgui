#include "Uniform3D.hpp"
#include "Global/Standards.hpp"
#include "Renderers/ThreeD/glTF_Model.hpp"
#include "TestUtils.hpp"
#include "VulkanBuffer.hpp"

using namespace Jkr::Misc::_3D;
using namespace Jkr;
constexpr int JointMatricesReserveSize = 500;

void Uniform3D::AddTexture(int inDstBinding, s inFileName, ui inDestSet) {
    Up<ImageType> Image = MakeUp<ImageType>(mInstance);
    Image->Setup(inFileName);
    Image->Register(0, inDstBinding, 0, *mVulkanDescriptorSet, inDestSet);
    mImages[inDstBinding] = mv(Image);
}

void Uniform3D::AddTextureByVector(
     int inDstBinding, v<uc>& inImageVector, ui inWidth, ui inHeight, ui inDstSet) {
    Up<ImageType> Image = MakeUp<ImageType>(mInstance);
    void* data          = inImageVector.data();
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
void Uniform3D::AddTextureToUniform3D(Uniform3D& inUniform3D, int inTextureId, ui inDstSet) {
    mImages[inTextureId]->Register(0, inTextureId, 0, *inUniform3D.mVulkanDescriptorSet, inDstSet);
    {};
}
void Uniform3D::AddUniformBufferToUniform3D(Uniform3D& inUniform3D, int inBufferId, ui inDstSet) {
    mUniformBuffers[inBufferId]->Register(
         0, inBufferId, 0, *inUniform3D.mVulkanDescriptorSet, inDstSet);
    {};
}
void Uniform3D::AddStorageBufferToUniform3D(Uniform3D& inUniform3D, int inStorageId, ui inDstSet) {
    mStorageBuffers[inStorageId]->RegisterCoherent(
         0, inStorageId, 0, *inUniform3D.mVulkanDescriptorSet, inDstSet);
    {};
}

void Uniform3D::AddSkyboxImage(SkyboxImageType& inType, int inDstBinding, ui inDstSet) {
    inType.Register(0, inDstBinding, 0, *mVulkanDescriptorSet, inDstSet);
}

void Uniform3D::UpdateByGLTFAnimation(Renderer::_3D::glTF_Model& inModel) {
    inModel.UpdateAllJoints([&](v<glm::mat4>& inMatrices) {
        void* data = inMatrices.data();
        UpdateStorageBuffer(kstd::BindingIndex::Storage::JointMatrix,
                            &data,
                            inMatrices.size() * sizeof(glm::mat4));
    }

    );
}

Up<Uniform3D> Uniform3D::CreateByGLTFNodeIndex(const Instance& inInstance,
                                               Simple3D& inSimple,
                                               Renderer::_3D::glTF_Model& inLoadedModel,
                                               ui inNodeIndex,
                                               bool inShouldSkin) {
    auto Uniform = MakeUp<Uniform3D>(inInstance);
    Uniform->Build(inSimple, inLoadedModel, inNodeIndex, inShouldSkin);
    return mv(Uniform);
};

void Uniform3D::Build(Simple3D& inSimple3D) {
    mVulkanDescriptorSet = MakeUp<VulkanDescriptorSet>(
         mInstance.GetDevice(),
         mInstance.GetDescriptorPool(),
         inSimple3D.GetPainterCache().GetVertexFragmentDescriptorSetLayout());
}

void Uniform3D::Build(Simple3D& inSimple3D,
                      Renderer::_3D::glTF_Model& inModel,
                      ui inNodeIndex,
                      bool inShouldSkin,
                      bool inShouldTextures) {
    if (not mVulkanDescriptorSet) {
        Build(inSimple3D);
    }
    Renderer::_3D::glTF_Model ModelCopy(inModel.GetFileName());
    if (inShouldSkin) {
        v<kstd::JointInfluence> JointInfluence;
        auto FillJointInfluence = [&JointInfluence](kstd::Vertex3DExt inVertex) {
            JointInfluence.push_back(kstd::JointInfluence{.mJointIndices = inVertex.mJointIndices,
                                                          .mJointWeights = inVertex.mJointWeights});
            return kstd::Vertex3D{.mPosition = inVertex.mPosition,
                                  .mNormal   = inVertex.mNormal,
                                  .mUV       = inVertex.mUV,
                                  .mColor    = inVertex.mColor};
        };
        ModelCopy.Load(FillJointInfluence, [](ui inIndex) { return inIndex; });

        this->AddStorageBuffer(kstd::BindingIndex::Storage::JointInfluence,
                               JointInfluence.size() * sizeof(kstd::JointInfluence));
        void* data = JointInfluence.data();
        this->UpdateStorageBuffer(kstd::BindingIndex::Storage::JointInfluence,
                                  &data,
                                  JointInfluence.size() * sizeof(kstd::JointInfluence));

        if (inModel.GetSkinsSize() != 0) {
            auto& Skins = inModel.GetSkinsRef()[0]; // TODO
                                                    // inSkinIndex
            auto& InverseBindMatrices = Skins.mInverseBindMatrices;
            void* Data                = InverseBindMatrices.data();
            this->AddStorageBuffer(kstd::BindingIndex::Storage::JointMatrix,
                                   InverseBindMatrices.size() * sizeof(glm::mat4));
        }
    }
    ui BindingIndex = kstd::BindingIndex::Uniform::DiffuseImage;
    if (inShouldTextures) {
        for (auto& I : inModel.GetTexturesRef()) {
            auto& Image = inModel.GetImagesRef()[I.mImageIndex];
            AddTextureByVector(BindingIndex, Image.mTextureImage, Image.mWidth, Image.mHeight);
            BindingIndex++;
        }
    }
}

void Uniform3D::AddBindingsToUniform3DGLTF(Uniform3D& modUniform3D,
                                           bool inShouldSkin,
                                           bool inShouldTexture,
                                           ui inSet) {
    if (inShouldSkin) {
        for (auto& u : mUniformBuffers) {
            AddUniformBufferToUniform3D(modUniform3D, u.first, inSet);
        }
        for (auto& s : mStorageBuffers) {
            AddStorageBufferToUniform3D(modUniform3D, s.first, inSet);
        }
    }
    if (inShouldTexture) {
        for (auto& t : mImages) {
            AddTextureToUniform3D(modUniform3D, t.first, inSet);
        }
    }
}

void Uniform3D::UpdateUniformBuffer(int inDstBinding, void** inData, size_t inSize) {
    void* memory = mUniformBuffers[inDstBinding]->GetUniformMappedMemoryRegion();
    memcpy(memory, *inData, inSize);
}

void Uniform3D::UpdateStorageBuffer(int inDstBinding, void** inData, size_t inSize) {
    void* memory = mStorageBuffers[inDstBinding]->GetStorageMappedMemoryRegion();
    memcpy(memory, *inData, inSize);
}

void Uniform3D::Bind(Window& inWindow,
                     Simple3D& inSimple3D,
                     int inSet,
                     Window::ParameterContext inParam) {
    const VulkanCommandBuffer& Cmd =
         inWindow.GetCommandBuffers(inParam)[inWindow.GetCurrentFrame()];
    mVulkanDescriptorSet->Bind(vk::PipelineBindPoint::eGraphics,
                               Cmd,
                               inSimple3D.GetPainterCache().GetVertexFragmentPipelineLayout(),
                               inSet);
}
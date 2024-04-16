#include "Uniform3D.hpp"
#include "Global/Standards.hpp"
#include "TestUtils.hpp"

using namespace Jkr::Misc::_3D;
using namespace Jkr;

void Uniform3D::AddTexture(int inDstBinding, s inFileName) {
               Up<ImageType> Image = MakeUp<ImageType>(mInstance);
               Image->Setup(inFileName);
               Image->Register(0, inDstBinding, 0, *mVulkanDescriptorSet);
               mImages[inDstBinding] = mv(Image);
}

void Uniform3D::AddTextureByVector(int inDstBinding, v<uc>& inImageVector, ui inWidth, ui inHeight) {
               Up<ImageType> Image = MakeUp<ImageType>(mInstance);
               void* data = inImageVector.data();
               Image->Setup(&data, inWidth, inHeight, 4);
               Image->Register(0, inDstBinding, 0, *mVulkanDescriptorSet);
               mImages[inDstBinding] = mv(Image);
}

void Uniform3D::AddUniformBuffer(int inDstBinding, size_t inSize) {
               Up<UniformBufferType> Buffer = MakeUp<UniformBufferType>(mInstance);
               Buffer->Setup(inSize);
               Buffer->Register(0, inDstBinding, 0, *mVulkanDescriptorSet);
               mUniformBuffers[inDstBinding] = mv(Buffer);
}

void Uniform3D::AddStorageBuffer(int inDstBinding, size_t inSize) {
               Up<StorageBufferType> Buffer = MakeUp<StorageBufferType>(mInstance);
               Buffer->SetupCoherent(inSize);
               Buffer->RegisterCoherent(0, inDstBinding, 0, *mVulkanDescriptorSet);
               mStorageBuffers[inDstBinding] = mv(Buffer);
}

static Up<Uniform3D> CreateByGLTFNodeIndex(const Instance& inInstance, Simple3D& inSimple, Renderer::_3D::glTF_Model& inLoadedModel, ui inNodeIndex) {
               auto Uniform = MakeUp<Uniform3D>(inInstance);
               Uniform->Build(inSimple, inLoadedModel);
               return mv(Uniform);
};

void Uniform3D::Build(Simple3D& inSimple3D) {
               mVulkanDescriptorSet = MakeUp<VulkanDescriptorSet>(mInstance.GetDevice(), mInstance.GetDescriptorPool(), inSimple3D.GetPainterCache().GetVertexFragmentDescriptorSetLayout());
}

void Uniform3D::Build(Simple3D& inSimple3D, Renderer::_3D::glTF_Model& inModel, ui inNodeIndex) {
               ui BindingIndex = 0;
               Build(inSimple3D);
               v<kstd::JointInfluence> JointInfluence;
               inModel.Load(
                         [&JointInfluence](kstd::Vertex3DExt inVertex) {
                                        JointInfluence.push_back(kstd::JointInfluence{.mJointIndices = inVertex.mJointIndices, .mJointWeights = inVertex.mJointWeights});
                                        return kstd::Vertex3D{.mPosition = inVertex.mPosition, .mNormal = inVertex.mNormal, .mUV = inVertex.mUV, .mColor = inVertex.mColor};
                         },
                         [](ui inIndex) { return inIndex; });
               inModel.Draw(
                         *inModel.GetNodesRef()[inNodeIndex].get(),
                         [](glm::mat4 inMat) {},
                         [&](ui inUI, Renderer::_3D::glTF_Model::Texture inTexture) {
                                        ui ImageIndex = inTexture.mImageIndex;
                                        auto& Image = inModel.GetImagesRef()[ImageIndex];
                                        AddTextureByVector(BindingIndex++, Image.mTextureImage, Image.mWidth, Image.mHeight);
                         });
}
#include "Uniform3D.hpp"
#include "Global/Standards.hpp"
#include "Renderers/ThreeD/glTF_Model.hpp"
#include "TestUtils.hpp"
#include "VulkanBuffer.hpp"

using namespace Jkr::Misc::_3D;
using namespace Jkr;
constexpr int JointMatricesReserveSize = 500;

void Uniform3D::AddTexture(int inDstBinding, s inFileName) {
               Up<ImageType> Image = MakeUp<ImageType>(mInstance);
               Image->Setup(inFileName);
               Image->Register(0, inDstBinding, 0, *mVulkanDescriptorSet);
               mImages[inDstBinding] = mv(Image);
}

void Uniform3D::AddTextureByVector(int inDstBinding, v<uc>& inImageVector, ui inWidth, ui inHeight) {
               Up<ImageType> Image = MakeUp<ImageType>(mInstance);
               void* data          = inImageVector.data();
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

void Uniform3D::UpdateByGLTFAnimation(Renderer::_3D::glTF_Model& inModel, float inDeltaTime, int inActiveAnimationIndex) {
               inModel.SetActiveAnimation(inActiveAnimationIndex);
               inModel.UpdateAnimation(inDeltaTime, [&](v<glm::mat4>& inMatrices) {
                              void* data = inMatrices.data();
                              UpdateStorageBuffer(inModel.GetSkinsSize() - 1, &data, inMatrices.size() * sizeof(glm::mat4));
               });
}

Up<Uniform3D> Uniform3D::CreateByGLTFNodeIndex(const Instance& inInstance, Simple3D& inSimple, Renderer::_3D::glTF_Model& inLoadedModel, ui inNodeIndex, bool inShouldSkin) {
               auto Uniform = MakeUp<Uniform3D>(inInstance);
               Uniform->Build(inSimple, inLoadedModel, inNodeIndex, inShouldSkin);
               return mv(Uniform);
};

void Uniform3D::Build(Simple3D& inSimple3D) {
               mVulkanDescriptorSet = MakeUp<VulkanDescriptorSet>(mInstance.GetDevice(), mInstance.GetDescriptorPool(), inSimple3D.GetPainterCache().GetVertexFragmentDescriptorSetLayout());
}

void Uniform3D::Build(Simple3D& inSimple3D, Renderer::_3D::glTF_Model& inModel, ui inNodeIndex, bool inShouldSkin) {
               ui BindingIndex = 0;
               if (not mVulkanDescriptorSet) {
                              Build(inSimple3D);
               }
               Renderer::_3D::glTF_Model ModelCopy(inModel.GetFileName());
               if (inShouldSkin) {
                              v<kstd::JointInfluence> JointInfluence;
                              ModelCopy.Load(
                                        [&JointInfluence](kstd::Vertex3DExt inVertex) {
                                                       JointInfluence.push_back(kstd::JointInfluence{.mJointIndices = inVertex.mJointIndices, .mJointWeights = inVertex.mJointWeights});
                                                       return kstd::Vertex3D{.mPosition = inVertex.mPosition, .mNormal = inVertex.mNormal, .mUV = inVertex.mUV, .mColor = inVertex.mColor};
                                        },
                                        [](ui inIndex) { return inIndex; });
                              this->AddStorageBuffer(BindingIndex, JointInfluence.size() * sizeof(kstd::JointInfluence));
                              void* data = JointInfluence.data();
                              this->UpdateStorageBuffer(BindingIndex, &data, JointInfluence.size() * sizeof(kstd::JointInfluence));
                              BindingIndex++;

                              if (inModel.GetSkinsSize() != 0) {
                                             auto& Skins               = inModel.GetSkinsRef()[0]; // TODO inSkinIndex
                                             auto& InverseBindMatrices = Skins.mInverseBindMatrices;
                                             void* Data                = InverseBindMatrices.data();
                                             this->AddUniformBuffer(BindingIndex, InverseBindMatrices.size() * sizeof(glm::mat4));
                                             BindingIndex++;
                              }
               }
               if (inModel.GetImagesSize() != 0) {
                              inModel.Draw(
                                        *inModel.GetNodesRef()[inNodeIndex].get(),
                                        [](glm::mat4 inMat) {},
                                        [&](ui inUI, opt<Renderer::_3D::glTF_Model::Texture> inTexture) {
                                                       if (inTexture.has_value()) {
                                                                      ui ImageIndex = inTexture.value().mImageIndex;
                                                                      auto& Image   = inModel.GetImagesRef()[ImageIndex];
                                                                      AddTextureByVector(BindingIndex, Image.mTextureImage, Image.mWidth, Image.mHeight);
                                                                      BindingIndex++;
                                                       }
                                        });
               }
}
#include "Painter.hpp"
#include "PainterParameter.hpp"
#include <Renderers/ThreeD/Simple3D.hpp>
#include <cstring>

namespace Jkr::Misc::_3D {
using Simple3D = Renderer::_3D::Simple3D;
class Uniform3D {
       public:
               using ImageType = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformImage>;
               using UniformBufferType = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformBuffer>;
               using StorageBufferType = Jkr::PainterParameter<Jkr::PainterParameterContext::StorageBuffer>;

               GETTER& GetImages() { return mImages; }
               GETTER& GetUniformBuffers() { return mUniformBuffers; }
               GETTER& GetStorageBuffers() { return mStorageBuffers; }

               static Up<Uniform3D> CreateByGLTFNodeIndex(const Instance& inInstance, Simple3D& inSimple3D, Renderer::_3D::glTF_Model& inLoadedModel, ui inNodeIndex = 0);

               void Build(Simple3D& inSimple3D);
               void Build(Simple3D& inSimple3D, Renderer::_3D::glTF_Model& inModel, ui inNodeIndex = 0);

               void AddTexture(int inDstBinding, s inFileName);
               void AddTextureByVector(int inDstBinding, v<uc>& inImageVector, ui inWidth, ui inHeight);
               void AddTextureBindless(int inDstBinding, s inFileName); // TODO To be Implemented
               void AddUniformBuffer(int inDstBinding, size_t inSize);
               void AddStorageBuffer(int inDstBinding, size_t inSize);

               template <typename T> void UpdateStorageBuffer(int inDstBinding, T inData);
               template <typename T> void UpdateUniformBuffer(int inDstBinding, T inData);
               void Bind(Window& inWindow, Simple3D& inSimple, Window::ParameterContext inParam);

               Uniform3D(const Instance& inInstance, Simple3D& inSimple3D) : mInstance(inInstance) { Build(inSimple3D); }
               Uniform3D(const Instance& inInstance) : mInstance(inInstance) {}

       private:
               const Instance& mInstance;
               umap<int, Up<ImageType>> mImages;
               umap<int, Up<UniformBufferType>> mUniformBuffers;
               umap<int, Up<StorageBufferType>> mStorageBuffers;
               Up<VulkanDescriptorSet> mVulkanDescriptorSet;
};

inline void Uniform3D::Bind(Window& inWindow, Simple3D& inSimple3D, Window::ParameterContext inParam) {
               const VulkanCommandBuffer& Cmd = inWindow.GetCommandBuffers(inParam)[inWindow.GetCurrentFrame()];
               mVulkanDescriptorSet->Bind(vk::PipelineBindPoint::eGraphics, Cmd, inSimple3D.GetPainterCache().GetVertexFragmentPipelineLayout());
}

template <typename T> inline void Uniform3D::UpdateUniformBuffer(int inDstBinding, T inData) {
               T data = inData;
               void* memory = mUniformBuffers[inDstBinding]->GetUniformMappedMemoryRegion();
               memcpy(memory, &data, sizeof(T));
}

template <typename T> inline void Uniform3D::UpdateStorageBuffer(int inDstBinding, T inData) {
               T data = inData;
               void* memory = mStorageBuffers[inDstBinding]->GetStorageMappedMemoryRegion();
               memcpy(memory, &data, sizeof(T));
}

} // namespace Jkr::Misc::_3D
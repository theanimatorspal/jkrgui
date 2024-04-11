#include "Painter.hpp"
#include "PainterParameter.hpp"
#include <Renderers/ThreeD/Simple3D.hpp>
#include <cstring>

namespace Jkr::Misc::_3D {
using Simple3D = Renderer::_3D::Simple3D;
class Uniform3D {
          using ImageType = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformImage>;
          using BufferType = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformBuffer>;

      public:
          void AddTexture(int inDstBinding, s inFileName);
          void AddTextureBindless(int inDstBinding, s inFileName); // TODO To be Implemented
          void AddBuffer(int inDstBinding, size_t inSize);
          template <typename T> void UpdateBuffer(int inDstBinding, T inData);
          void Bind(Window& inWindow, Window::ParameterContext inParam);

          Uniform3D(const Instance& inInstance, Simple3D& inSimple) : mInstance(inInstance), mSimple3D(inSimple) {
                    mVulkanDescriptorSet = MakeUp<VulkanDescriptorSet>(
                     mInstance.GetDevice(), mInstance.GetDescriptorPool(), mSimple3D.GetPainterCache().GetVertexFragmentDescriptorSetLayout());
          }

      private:
          const Instance& mInstance;
          Simple3D& mSimple3D;
          umap<int, Up<ImageType>> mImages;
          umap<int, Up<BufferType>> mBuffers;
          Up<VulkanDescriptorSet> mVulkanDescriptorSet;
};

inline void Uniform3D::Bind(Window& inWindow, Window::ParameterContext inParam) {
          const VulkanCommandBuffer& Cmd = inWindow.GetCommandBuffers(inParam)[inWindow.GetCurrentFrame()];
          mVulkanDescriptorSet->Bind(vk::PipelineBindPoint::eGraphics, Cmd, mSimple3D.GetPainterCache().GetVertexFragmentPipelineLayout());
}

template <typename T> inline void Uniform3D::UpdateBuffer(int inDstBinding, T inData) {
          T data = inData;
          void* memory = mBuffers[inDstBinding]->GetUniformMappedMemoryRegion();
          memcpy(memory, &data, sizeof(T));
}

} // namespace Jkr::Misc::_3D
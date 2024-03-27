#pragma once
#include "Primitive.hpp"
#include "Shape3D_base.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanBufferVMA.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "glTF_Model.hpp"
#include "spirv_cross/spirv.hpp"
#include <Renderers/Renderer_base.hpp>
#include <string>

namespace Jkr::Renderer::_3D {

class Shape : public Renderer_base, public Shape_base {
          using gb = Shape_base;
          using rb = Renderer_base;
          using StagingBufferType = VulkanBuffer<BufferContext::Staging, MemoryType::HostVisibleAndCoherenet>;

          using ComPar = Window::ParameterContext;

      public:
          GETTER& GetPrimitive() { return *mPrimitive; }
          Shape(const Instance& inInstance, Window& inCompatibleWindow);
          void Add(const sv inFileName, ui& outId);
          void Bind(Window& inWindow, ComPar inCompar);
          void Dispatch(Window& inWindow);

          ui AddEXT(const sv inFileName);

      private:
          void CheckAndResize(const glTF_Model& inModel);
          // TODO Remove this
          v<Up<glTF_Model>> mModels;
          Up<Primitive> mPrimitive;

          const Instance& mInstance;

          ui mTotalNoOfVerticesRendererCanHold = rb::InitialRendererElementArraySize;
          ui mTotalNoOfIndicesRendererCanHold = rb::InitialRendererElementArraySize;
};

inline void Shape::Bind(Window& inWindow, ComPar inCompar) {
          Painter::BindDrawParamtersVertexAndIndexBuffersOnly_EXT(mInstance, *mPrimitive, inWindow, inCompar);
}

inline ui Shape::AddEXT(const sv inFileName) {
          ui i;
          Add(inFileName, i);
          return i;
}
} // namespace Jkr::Renderer::_3D

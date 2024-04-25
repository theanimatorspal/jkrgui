#pragma once
#include "Primitive.hpp"
#include "Shape3D_base.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanBufferVMA.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "glTF_Model.hpp"
#include "spirv_cross/spirv.hpp"
#include <Renderers/Generator.hpp>
#include <Renderers/Renderer_base.hpp>
#include <string>

namespace Jkr::Renderer::_3D {

class Shape : public Renderer_base, public Shape_base {
    using gb = Shape_base;
    using rb = Renderer_base;
    using StagingBufferType =
         VulkanBuffer<BufferContext::Staging, MemoryType::HostVisibleAndCoherenet>;

    using ComPar = Window::ParameterContext;

    public:
    GETTER& GetPrimitive() { return *mPrimitive; }
    Shape(const Instance& inInstance, Window& inCompatibleWindow);
    void Add(glTF_Model& inModel, ui& outId);
    void Add(Generator& inGenerator, glm::vec3 inPosition, ui& outId);
    void Update(ui inId, Generator& inGenerator, glm::vec3 inPosition);
    void Bind(Window& inWindow, ComPar inCompar);
    void Dispatch(Window& inWindow);

    ui AddEXT(Generator& inGenerator, glm::vec3 inPosition);
    ui AddEXT(glTF_Model& inModel);

    private:
    void CheckAndResize(const glTF_Model& inModel);
    void CheckAndResize(size_t inIndicesSize, size_t inVerticesSize);
    void CopyToPrimitive(ui inOffsetId, ui inModelId);
    Up<Primitive> mPrimitive;

    const Instance& mInstance;

    ui mTotalNoOfVerticesRendererCanHold = rb::InitialRendererElementArraySize;
    ui mTotalNoOfIndicesRendererCanHold  = rb::InitialRendererElementArraySize;
};

inline void Shape::Bind(Window& inWindow, ComPar inCompar) {
    Painter::BindDrawParamtersVertexAndIndexBuffersOnly_EXT(
         mInstance, *mPrimitive, inWindow, inCompar);
}

inline ui Shape::AddEXT(Generator& inGenerator, glm::vec3 inPosition) {
    ui i;
    Add(inGenerator, inPosition, i);
    return i;
}

inline ui Shape::AddEXT(glTF_Model& inModel) {
    ui i;
    Add(inModel, i);
    return i;
}
} // namespace Jkr::Renderer::_3D

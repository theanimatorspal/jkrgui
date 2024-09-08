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
    using gb                = Shape_base;
    using rb                = Renderer_base;
    using StagingBufferType = VulkanBuffer;
    using ComPar            = Window_base::ParameterContext;

    public:
    GETTER& GetPrimitive() { return *mPrimitive; }
    Shape(const Instance& inInstance, Window_base& inCompatibleWindow);
    void Add(glTF_Model& inModel, ui& outId);
    void Add(Generator& inGenerator, glm::vec3 inPosition, ui& outId);
    void Update(ui inId, Generator& inGenerator, glm::vec3 inPosition);
    void Bind(Window_base& inWindow, ComPar inCompar);
    void BindByCommandBuffer(VulkanCommandBuffer& inCmdBuffer);
    void Dispatch(Window_base& inWindow);

    ui AddEXT(Generator& inGenerator, glm::vec3 inPosition);
    ui AddEXT(glTF_Model& inModel);

    void CheckAndResize(size_t inIndicesSize, size_t inVerticesSize);

    private:
    std::mutex mAddMutex;

    void CheckAndResize(const glTF_Model& inModel);
    void CopyToPrimitive(ui inOffsetId, ui inModelId);
    Up<Primitive> mPrimitive;

    const Instance& mInstance;

    ui mTotalNoOfVerticesRendererCanHold = rb::InitialRendererElementArraySize;
    ui mTotalNoOfIndicesRendererCanHold  = rb::InitialRendererElementArraySize;
};

} // namespace Jkr::Renderer::_3D

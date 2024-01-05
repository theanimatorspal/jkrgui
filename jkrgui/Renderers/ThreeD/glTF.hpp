#pragma once
#include "Primitive.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanBufferVMA.hpp"
#include "VulkanPhysicalDevice.hpp"
#include "glTF_Model.hpp"
#include "glTF_base.hpp"
#include <Renderers/Renderer_base.hpp>
#include <string>

namespace Jkr::Renderer::_3D {

struct GlobalUB {
    glm::mat4 view;
    glm::mat4 projection;
};

struct GlobalSSBO {
    glm::vec4 light;
};

class Shape : public Renderer_base, public glTF_base {
    using gb = glTF_base;
    using rb = Renderer_base;
    using SSBOType = PainterParameter<Jkr::PainterParameterContext::StorageBuffer>;
    using UBType = PainterParameter<Jkr::PainterParameterContext::UniformBuffer>;
    using StagingBufferType
        = VulkanBuffer<BufferContext::Staging, MemoryType::HostVisibleAndCoherenet>;
    using ImageType = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformImage>;

public:
    Shape(const Instance& inInstance, Window& inCompatibleWindow, sz inGlobalUBsize = sizeof(GlobalUB), sz inSSBOsize = sizeof(GlobalSSBO) * 10);
    void Add(const sv inFileName, ui& outId);
    void Bind(Window& inWindow);
    void Dispatch(Window& inWindow);
    template <typename T>
    void WriteToGlobalUB(T inData);
    void RegisterGlobalUBToPainter(ui inPainterId, sz inOffset = 0, ui inBinding = 0, ui inDestinationArrayElement = 0);
    void RegisterGlobalSSBOToPainter(ui inPainterId, sz inOffset = 0, ui inBinding = 0, ui inDestinationArrayElement = 0);

    void AddPainter(const sv inFileName,
        const sv inVertexShader,
        const sv inFragmentShader,
        const sv inComputeShader,
        Window& inCompatibleWindow,
        ui& outId, bool inForceStore = false);
    void PainterBindDraw(ui inPainterId, Window& inWindow);
    void AddModelTextureToPainter(ui inId,
        ui inPainterId,
        sz inOffset = 0,
        ui inBinding = 0,
        ui inDestinationArrayElement = 0);

    void RegisterExternalTextureToPainter(ui& outId,
        ui inPainterId,
        const sv inFilename,
        sz inOffset = 0,
        ui inBinding = 0,
        ui inDestinationArrayElement = 0);

    void RegisterExternalTextureArrayToPainter(ui& outId,
        ui inPainterId,
        std::span<const sv> inFileNames,
        sz inOffset = 0,
        ui inBindingn = 0,
        ui inDestinationArrayElement = 0);

    template <typename T>
    void PainterDraw(ui inId, T inPush, Window& inWindow);
    void PainterDispatch(ui inId);

private:
    void AddPainter(Up<Painter> inPainter, Up<PainterCache> inPainterCaches, ui& outId);
    void CheckAndResize(const glTF_Model& inModel);
    v<Up<glTF_Model>> mModels;
    v<Up<Painter>> mPainters;
    v<Up<PainterCache>> mPainterCaches;
    ui mBoundPainterVF = 0;
    ui mBoundPainterC = 0; // Compute
    Up<Primitive> mPrimitive;

    v<Up<ImageType>> mExternalTextures;

private:
    Up<SSBOType> mGlobalSSBO;
    Up<UBType> mGlobalUB;
    Up<StagingBufferType> mGlobalUBOStatingBuffer;

private:
    const Instance& mInstance;

private:
    ui mTotalNoOfVerticesRendererCanHold = rb::InitialRendererElementArraySize;
    ui mTotalNoOfIndicesRendererCanHold = rb::InitialRendererElementArraySize;
};

/*


































*/

template <typename T>
void Shape::WriteToGlobalUB(T inData)
{
    T d = inData;
    assert(mGlobalUB->GetUniformBufferSize() >= sizeof(inData));
    void* region = mGlobalUB->GetUniformMappedMemoryRegion();
    std::memcpy(region, &d, sizeof(d));
}

template <typename T>
void Shape::PainterDraw(ui inId, T inPush, Window& inWindow)
{
    mPainters[mBoundPainterVF]
        ->Draw_EXT<T>(*mPrimitive, inPush, inWindow, gb::GetIndexCount(inId), 1, 0, 0);
}

inline void Shape::Bind(Window& inWindow)
{
    Painter::BindDrawParamtersVertexAndIndexBuffersOnly_EXT(mInstance, *mPrimitive, inWindow);
}

inline void Shape::RegisterGlobalUBToPainter(ui inPainterId,
    sz inOffset,
    ui inBinding,
    ui inDestinationArrayElement)
{
    mPainters[inPainterId]->RegisterPainterParameter(*mGlobalUB, inOffset, inBinding, inDestinationArrayElement, Jkr::RegisterMode::VertexFragmentOnly);
}

inline void Shape::RegisterGlobalSSBOToPainter(ui inPainterId,
    sz inOffset,
    ui inBinding,
    ui inDestinationArrayElement)
{
    mPainters[inPainterId]->RegisterPainterParameter(*mGlobalSSBO,
        inOffset,
        inBinding,
        inDestinationArrayElement,
        RegisterMode::AllShaders);
}

inline void Shape::PainterBindDraw(ui inPainterId, Window& inWindow)
{
    mPainters[inPainterId]->BindDrawParamtersPipelineOnly_EXT(*mPrimitive, inWindow);
    mPainters[inPainterId]->BindDrawParamtersDescriptorsOnly_EXT(*mPrimitive, inWindow);
}

inline void Shape::AddModelTextureToPainter(
    ui inId, ui inPainterId, sz inOffset, ui inBinding, ui inDestinationArrayElement)
{
    auto& pp = mModels[inId]->GetPainterImageParameters().back().mTextureImage;
    mPainters[inPainterId]->RegisterPainterParameter(*pp,
        inOffset,
        inBinding,
        inDestinationArrayElement);
}

inline void Shape::RegisterExternalTextureToPainter(ui& outId,
    ui inPainterId,
    const sv inFilename,
    sz inOffset,
    ui inBinding,
    ui inDestinationArrayElement)
{
    Up<ImageType> AnImage = MakeUp<ImageType>(mInstance);
    AnImage->Setup(inFilename);
    mExternalTextures.push_back(mv(AnImage));
    outId = mExternalTextures.size() - 1;
    mPainters[inPainterId]->RegisterPainterParameter(*mExternalTextures.back(),
        inOffset,
        inBinding,
        inDestinationArrayElement,
        RegisterMode::VertexFragmentOnly);
}

inline void Shape::RegisterExternalTextureArrayToPainter(ui& outId,
    ui inPainterId,
    std::span<const sv> inFileNames,
    sz inOffset,
    ui inBindingn,
    ui inDestinationArrayElement)
{
    Up<ImageType> AnImage = MakeUp<ImageType>(mInstance);
    AnImage->Setup(inFileNames);
    mExternalTextures.push_back(mv(AnImage));
    outId = mExternalTextures.size() - 1;
    mPainters[inPainterId]->RegisterPainterParameter(*mExternalTextures.back(),
        inOffset,
        inBindingn,
        inDestinationArrayElement,
        RegisterMode::VertexFragmentOnly);
}

} // namespace Jkr::Renderer::_3D

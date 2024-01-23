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
    using SkyboxType = Jkr::PainterParameter<Jkr::PainterParameterContext::SkyboxImage>;

    using ComPar = Window::ParameterContext;

public:
    Shape(const Instance& inInstance, Window& inCompatibleWindow, sz inGlobalUBsize = sizeof(GlobalUB), sz inSSBOsize = sizeof(GlobalSSBO) * 10);
    void Add(const sv inFileName, ui& outId);
    void Bind(Window& inWindow, ComPar inCompar);
    void Dispatch(Window& inWindow);

    std::unique_lock<std::mutex> MakeLockedLocker()
    {
        return std::unique_lock<std::mutex>(mPainterMutex);
    }
    void Wait(std::unique_lock<std::mutex>& inLocker)
    {
        mCondVar.wait(inLocker, [this]() { return IsNotLocked; });
        IsNotLocked = false;
    }
    void UnlockLocker(std::unique_lock<std::mutex>& inLocker)
    {
        IsNotLocked = true;
        inLocker.unlock();
        mCondVar.notify_all();
    }

private:
    std::mutex mPainterMutex;
    std::condition_variable mCondVar;
    bool IsNotLocked = true;

public:
    /*
         Global Uniform buffer and Global SSBO
    */
    template <typename T>
    void WriteToGlobalUB(T inData);
    void RegisterGlobalUBToPainter(ui inPainterId, sz inOffset = 0, ui inBinding = 0, ui inDestinationArrayElement = 0);
    void RegisterGlobalSSBOToPainter(ui inPainterId, sz inOffset = 0, ui inBinding = 0, ui inDestinationArrayElement = 0);
    void AddSkybox(std::span<const sv> inSkyboxFiles);
    void RegisterSkyboxToPainter(ui inPainterId, ui inBinding);

    /*
        Painters that can be added with Shader Code, Each Painter Contains a Vertex, Fragment and Compute Shaders.
    */

    void AddPainter(const sv inFileName, const sv inVertexShader, const sv inFragmentShader, const sv inComputeShader, Window& inCompatibleWindow, ui& outId, bool inForceStore = false);
    void AddModelTextureToPainter(ui inId, ui inPainterId, sz inOffset = 0, ui inBinding = 0, ui inDestinationArrayElement = 0);
    void RegisterExternalTextureToPainter(ui& outId, ui inPainterId, const sv inFilename, sz inOffset = 0, ui inBinding = 0, ui inDestinationArrayElement = 0);
    void RegisterExternalTextureArrayToPainter(ui& outId, ui inPainterId, std::span<const sv> inFileNames, sz inOffset = 0, ui inBindingn = 0, ui inDestinationArrayElement = 0);
    void PainterBindForDraw(ui inPainterId, Window& inWindow, ComPar inCompar);
    template <typename T>
    void PainterDraw(ui inId, T inPush, Window& inWindow, ComPar inCompar);
    template <typename T>
    void PainterDraw(ui inPipelineId, ui inId, T inPush, Window& inWindow, ComPar inCompar);

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
    Up<SkyboxType> mSkyboxTexture;

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
void Shape::PainterDraw(ui inId, T inPush, Window& inWindow, ComPar inComPar)
{
    mPainters[mBoundPainterVF]
        ->Draw_EXT<T>(*mPrimitive, inPush, inWindow, gb::GetIndexCount(inId), 1, 0, 0, inComPar);
}

template <typename T>
inline void Shape::PainterDraw(ui inPipelineId, ui inId, T inPush, Window& inWindow, ComPar inComPar)
{
    mPainters[inPipelineId]->Draw_EXT<T>(*mPrimitive, inPush, inWindow, gb::GetIndexCount(inId), 1, 0, 0, inComPar);
}

inline void Shape::Bind(Window& inWindow, ComPar inCompar)
{
    Painter::BindDrawParamtersVertexAndIndexBuffersOnly_EXT(mInstance, *mPrimitive, inWindow, inCompar);
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

inline void Shape::PainterBindForDraw(ui inPainterId, Window& inWindow, ComPar inComPar)
{
    mPainters[inPainterId]->BindDrawParamtersPipelineOnly_EXT(*mPrimitive, inWindow, inComPar);
    mPainters[inPainterId]->BindDrawParamtersDescriptorsOnly_EXT(*mPrimitive, inWindow, inComPar);
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

inline void Jkr::Renderer::_3D::Shape::AddSkybox(std::span<const sv> inSkyboxFiles)
{
    mSkyboxTexture = MakeUp<SkyboxType>(mInstance);
    mSkyboxTexture->Setup(inSkyboxFiles);
}

inline void Jkr::Renderer::_3D::Shape::RegisterSkyboxToPainter(ui inPainterId, ui inBinding)
{
    mPainters[inPainterId]->RegisterPainterParameter(*mSkyboxTexture, 0, inBinding, 0, Jkr::RegisterMode::VertexFragmentOnly);
}

} // namespace Jkr::Renderer::_3D

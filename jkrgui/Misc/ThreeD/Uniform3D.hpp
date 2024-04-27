#pragma once
#include "Global/Standards.hpp"
#include "Painter.hpp"
#include "PainterParameter.hpp"
#include <Renderers/ThreeD/Simple3D.hpp>
#include <cstring>

namespace Jkr::Misc::_3D {
using Simple3D = Renderer::_3D::Simple3D;
class Uniform3D {
    public:
    using SkyboxImageType   = Jkr::PainterParameter<PainterParameterContext::SkyboxImage>;
    using ImageType         = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformImage>;
    using UniformBufferType = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformBuffer>;
    using StorageBufferType = Jkr::PainterParameter<Jkr::PainterParameterContext::StorageBuffer>;

    GETTER& GetImagesRef() { return mImages; }
    GETTER& GetUniformBuffersRef() { return mUniformBuffers; }
    GETTER& GetStorageBuffersRef() { return mStorageBuffers; }
    GETTER& GetVulkanDescriptorSet() { return *mVulkanDescriptorSet; }

    static Up<Uniform3D> CreateByGLTFNodeIndex(const Instance& inInstance,
                                               Simple3D& inSimple3D,
                                               Renderer::_3D::glTF_Model& inLoadedModel,
                                               ui inNodeIndex    = 0,
                                               bool inShouldSkin = false);

    void Build(Simple3D& inSimple3D);
    void Build(Simple3D& inSimple3D,
               Renderer::_3D::glTF_Model& inModel,
               ui inNodeIndex        = 0,
               bool inShouldSkin     = false,
               bool inShouldTextures = false);

    void AddTexture(int inDstBinding, s inFileName);
    void AddTextureByVector(int inDstBinding, v<uc>& inImageVector, ui inWidth, ui inHeight);
    void AddTextureBindless(int inDstBinding,
                            s inFileName); // TODO To be Implemented
    void AddUniformBuffer(int inDstBinding, size_t inSize);
    void AddStorageBuffer(int inDstBinding, size_t inSize);
    void AddTextureToUniform3D(Uniform3D& modUniform3D, int inTextureId);
    void AddUniformBufferToUniform3D(Uniform3D& modUniform3D, int inBufferId);
    void AddStorageBufferToUniform3D(Uniform3D& modUniform3D, int inStorageId);
    void AddBindingsToUniform3DGLTF(Uniform3D& modUniform3D,
                                    bool inShouldSkin    = false,
                                    bool inShouldTexture = false);
    void AddSkyboxImage(SkyboxImageType& inType,
                        int inDstBinding = kstd::BindingIndex::Uniform::CubeMapImage);

    template <typename T> void UpdateStorageBuffer(int inDstBinding, T inData);
    template <typename T> void UpdateUniformBuffer(int inDstBinding, T inData);
    void UpdateStorageBuffer(int inDstBinding, void** inData, size_t inSize);
    void UpdateUniformBuffer(int inDstBinding, void** inData, size_t inSize);
    void UpdateByGLTFAnimation(Renderer::_3D::glTF_Model& inModel,
                               float inDeltaTime          = 0.0f,
                               int inActiveAnimationIndex = 0);
    void Bind(Window& inWindow, Simple3D& inSimple, int inSet, Window::ParameterContext inParam);
    void Print(); // TODO ChatGPT

    Uniform3D(const Instance& inInstance, Simple3D& inSimple3D) : mInstance(inInstance) {
        Build(inSimple3D);
    }
    Uniform3D(const Instance& inInstance) : mInstance(inInstance) {}

    private:
    const Instance& mInstance;
    umap<int, Up<ImageType>> mImages;
    umap<int, Up<UniformBufferType>> mUniformBuffers;
    umap<int, Up<StorageBufferType>> mStorageBuffers;
    Up<VulkanDescriptorSet> mVulkanDescriptorSet;
};

template <typename T> inline void Uniform3D::UpdateUniformBuffer(int inDstBinding, T inData) {
    T data       = inData;
    void* memory = mUniformBuffers[inDstBinding]->GetUniformMappedMemoryRegion();
    memcpy(memory, &data, sizeof(T));
}

template <typename T> inline void Uniform3D::UpdateStorageBuffer(int inDstBinding, T inData) {
    T data       = inData;
    void* memory = mStorageBuffers[inDstBinding]->GetStorageMappedMemoryRegion();
    memcpy(memory, &data, sizeof(T));
}

} // namespace Jkr::Misc::_3D
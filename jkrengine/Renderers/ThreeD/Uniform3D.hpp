#pragma once
#include "Global/Standards.hpp"
#include "PainterParameter.hpp"
#include <Renderers/ThreeD/Simple3D.hpp>
#include <Renderers/TwoD/Shape.hpp>

namespace Jkr::Renderer::_3D {
using Simple3D = Renderer::_3D::Simple3D;
class World3D;
class Uniform3D {
    public:
    using SkyboxImageType   = Jkr::PainterParameter<PainterParameterContext::SkyboxImage>;
    using ImageType         = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformImage>;
    using UniformBufferType = Jkr::PainterParameter<Jkr::PainterParameterContext::UniformBuffer>;
    using StorageBufferType = Jkr::PainterParameter<Jkr::PainterParameterContext::StorageBuffer>;

    /* ============================================================

        GETTER

    ============================================================== */

    GETTER &GetImagesRef() { return mImages; }
    GETTER &GetUniformBuffersRef() { return mUniformBuffers; }
    GETTER &GetStorageBuffersRef() { return mStorageBuffers; }
    GETTER &GetVulkanDescriptorSet() { return *mVulkanDescriptorSet; }

    /* ============================================================

        Creator/Builder Pattern

    ============================================================== */

    static Up<Uniform3D> CreateByGLTFNodeIndex(Instance &inInstance,
                                               Simple3D &inSimple3D,
                                               Renderer::_3D::glTF_Model &inLoadedModel,
                                               ui inNodeIndex    = 0,
                                               bool inShouldSkin = false);

    void Build(Simple3D &inSimple3D);
    void Build(Simple3D &inSimple3D, VulkanDescriptorPool &inPool);
    void Build(Simple3D &inSimple3D,
               Renderer::_3D::glTF_Model &inModel,
               ui inNodeIndex        = 0,
               bool inShouldSkin     = false,
               bool inShouldTangents = false);
    void Build(Simple3D &inSimple3D,
               Renderer::_3D::glTF_Model &inModel,
               Renderer::_3D::glTF_Model::Primitive &inPrimitive);
    void
    BuildByMaterial(Simple3D &inSimpe3D, Renderer::_3D::glTF_Model &inModel, int inMaterialIndex);

    /* ============================================================

        Modification Routines

    ============================================================== */

    void AddTexture(int inDstBinding, s inFileName, ui inDstSet = 1);
    void AddTextureByVector(
         int inDstBinding, v<uc> &inImageVector, ui inWidth, ui inHeight, ui inDstSet = 1);
    void
    AddTextureBindless(int inDstBinding, s inFileName, ui inDstSet = 1); // TODO To be Implemented
    void AddUniformBuffer(int inDstBinding, size_t inSize, ui inDstSet = 1);
    void AddStorageBuffer(int inDstBinding, size_t inSize, ui inDstSet = 1);
    void AddTextureToUniform3D(Uniform3D &modUniform3D, int inTextureId, ui inDstSet = 1);
    void AddUniformBufferToUniform3D(Uniform3D &modUniform3D, int inBufferId, ui inDstSet = 1);
    void AddStorageBufferToUniform3D(Uniform3D &modUniform3D, int inStorageId, ui inDstSet = 1);
    void AddBindingsToUniform3DGLTF(Uniform3D &modUniform3D,
                                    bool inShouldSkin    = false,
                                    bool inShouldTexture = false,
                                    ui inDstSet          = 0);
    void AddSkyboxImage(SkyboxImageType &inType,
                        int inDstBinding = kstd::BindingIndex::Uniform::CubeMapImage,
                        ui inDstSet      = 0);
    void AddTextureFromShapeImage(Jkr::Renderer::Shape &inShapeRenderer,
                                  int inShapeImageId,
                                  int inDstImageBinding = kstd::BindingIndex::Uniform::Images,
                                  int inDstSet          = 0);

    void AddGenerateBRDFLookupTable(Instance &inInstance,
                                    Window &inWindow,
                                    std::string_view inFileName,
                                    std::string_view inVertexShader,
                                    std::string_view inFragmentShader,
                                    std::string_view inComputeShader,
                                    bool inShouldLoad,
                                    int inDstBinding = kstd::BindingIndex::Uniform::Images,
                                    int inDstSet     = 1);

    void AddGenerateIrradianceCube(Instance &inInstance,
                                   Window &inWindow,
                                   Renderer::_3D::Shape &inShape,
                                   int inSkyboxModelIndex,
                                   VulkanImageBase &inEnvironmentCubeMap,
                                   Renderer::_3D::World3D &inWorld,
                                   std::string_view inFileName,
                                   std::string_view inVertexShader,
                                   std::string_view inFragmentShader,
                                   std::string_view inComputeShader,
                                   bool inShouldLoad,
                                   int inDstBinding = kstd::BindingIndex::Uniform::CubeMapImage,
                                   int inDstSet     = 1);

    void AddGeneratePrefilteredCube(Instance &inInstance,
                                    Window &inWindow,
                                    Renderer::_3D::Shape &inShape,
                                    int inSkyboxModelIndex,
                                    VulkanImageBase &inEnvironmentCubeMap,
                                    Renderer::_3D::World3D &inWorld,
                                    std::string_view inFileName,
                                    std::string_view inVertexShader,
                                    std::string_view inFragmentShader,
                                    std::string_view inComputeShader,
                                    bool inShouldLoad,
                                    int inDstBinding = kstd::BindingIndex::Uniform::CubeMapImage,
                                    int inDstSet     = 1);

    template <typename T> void UpdateStorageBuffer(int inDstBinding, T inData);
    template <typename T> void UpdateUniformBuffer(int inDstBinding, T inData);
    void UpdateStorageBuffer(int inDstBinding, void **inData, size_t inSize);
    void UpdateUniformBuffer(int inDstBinding, void **inData, size_t inSize);
    void UpdateByGLTFAnimation(Renderer::_3D::glTF_Model &inModel);
    void Bind(Window_base &inWindow,
              Simple3D &inSimple,
              int inSet,
              Window_base::ParameterContext inParam);
    void Print(); // TODO ChatGPT

    Uniform3D(Instance &inInstance, Simple3D &inSimple3D) : mInstance(inInstance) {
        Build(inSimple3D);
    }
    Uniform3D(Instance &inInstance) : mInstance(inInstance) {}

    private:
    Instance &mInstance;
    umap<int, Up<ImageType>> mImages;
    umap<int, Up<UniformBufferType>> mUniformBuffers;
    umap<int, Up<StorageBufferType>> mStorageBuffers;
    Up<VulkanDescriptorSet> mVulkanDescriptorSet;
};

template <typename T> inline void Uniform3D::UpdateUniformBuffer(int inDstBinding, T inData) {
    T data       = inData;
    void *memory = mUniformBuffers[inDstBinding]->GetUniformMappedMemoryRegion();
    memcpy(memory, &data, sizeof(T));
}

template <typename T> inline void Uniform3D::UpdateStorageBuffer(int inDstBinding, T inData) {
    T data       = inData;
    void *memory = mStorageBuffers[inDstBinding]->GetStorageMappedMemoryRegion();
    memcpy(memory, &data, sizeof(T));
}

} // namespace Jkr::Renderer::_3D
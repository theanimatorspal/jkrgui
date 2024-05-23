#pragma once
#include "Simple3D.hpp"
#include <Renderers/ThreeD/Shape3D.hpp>
#include <Instance.hpp>
#include <Painter.hpp>
#include <VulkanFrameBuffer.hpp>
#include <VulkanImageVMA.hpp>
#include <VulkanRenderPass.hpp>

namespace Jkr::Renderer {
using namespace ksai;
using namespace Jkr;
struct PBR {
    static Up<VulkanImageVMA> GenerateBRDFLookupTable(Instance& inInstance,
                                                      WindowMulT& inWindow,
                                                      std::string_view inFileName,
                                                      std::string_view inVertexShader,
                                                      std::string_view inFragmentShader,
                                                      std::string_view inComputeShader,
                                                      bool inShouldLoad);
    static Up<VulkanImageVMA> GenerateIrradianceCube(Instance& inInstance,
                                                     WindowMulT& inWindow,
                                                     _3D::Shape& inShape,
                                                     int inSkyboxModelIndex,
                                                     VulkanImageBase& inEnvironmentCubeMap,
                                                     std::string_view inFileName,
                                                     std::string_view inVertexShader,
                                                     std::string_view inFragmentShader,
                                                     std::string_view inComputeShader,
                                                     bool inShouldLoad,
                                                     kstd::WorldInfoUniform inUniform);
    static Up<VulkanImageVMA> GeneratePrefilteredCube(Instance& inInstance,
                                                      WindowMulT& inWindow,
                                                      _3D::Shape& inShape,
                                                      int inSkyboxModelIndex,
                                                      VulkanImageBase& inEnvironmentCubeMap,
                                                      std::string_view inFileName,
                                                      std::string_view inVertexShader,
                                                      std::string_view inFragmentShader,
                                                      std::string_view inComputeShader,
                                                      bool inShouldLoad,
                                                      kstd::WorldInfoUniform inUniform);
};
} // namespace Jkr::Renderer
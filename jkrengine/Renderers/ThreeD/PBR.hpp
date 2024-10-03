#pragma once
#include <Renderers/ThreeD/Shape3D.hpp>
#include <Instance.hpp>
#include <Painter.hpp>
#include <VulkanFrameBuffer.hpp>
#include <VulkanImageVMA.hpp>
#include <VulkanRenderPass.hpp>

namespace Jkr::Renderer {

namespace _3D {
class World3D;
}

using namespace ksai;
using namespace Jkr;
namespace PBR {
const int BRDFLUTDimension         = 512;
const int PrefilteredCubeDimension = 512;
const int IrradianceCubeDimension  = 64;
Up<VulkanImageVMA> GenerateBRDFLookupTable(Instance &inInstance,
                                           Window &inWindow,
                                           std::string_view inFileName,
                                           std::string_view inVertexShader,
                                           std::string_view inFragmentShader,
                                           std::string_view inComputeShader,
                                           bool inShouldLoad);
///@warning I dont fucking know why this call if called before GeneratePrefilteredCube gives a
/// HAZARD READ_AFTER_WRITE for inEnvironmentCube, @todo Fix this
Up<VulkanImageVMA> GenerateIrradianceCube(Instance &inInstance,
                                          Window &inWindow,
                                          _3D::Shape &inShape,
                                          int inSkyboxModelIndex,
                                          VulkanImageBase &inEnvironmentCubeMap,
                                          std::string_view inFileName,
                                          std::string_view inVertexShader,
                                          std::string_view inFragmentShader,
                                          std::string_view inComputeShader,
                                          bool inShouldLoad,
                                          _3D::World3D &inWorld);
Up<VulkanImageVMA> GeneratePrefilteredCube(Instance &inInstance,
                                           Window &inWindow,
                                           _3D::Shape &inShape,
                                           int inSkyboxModelIndex,
                                           VulkanImageBase &inEnvironmentCubeMap,
                                           std::string_view inFileName,
                                           std::string_view inVertexShader,
                                           std::string_view inFragmentShader,
                                           std::string_view inComputeShader,
                                           bool inShouldLoad,
                                           _3D::World3D &inWorld3D);
}; // namespace PBR

} // namespace Jkr::Renderer

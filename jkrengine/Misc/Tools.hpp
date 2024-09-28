#include <Renderers/TwoD/Shape.hpp>
#include <Renderers/ThreeD/Shape3D.hpp>
#include <Window.hpp>

namespace Jkr {

/// @brief The Image should've been created with the same size as that of Window's deferred pass
void CopyWindowDeferredImageToShapeImage(Window &inWindow, Renderer::Shape &inShape2d, int inId);
void CopyWindowDeferredImageToCustomPainterImage(
     Window &inWindow, Renderer::CustomPainterImage &inCustomPainterImage);

/// @brief This function takes shape3d as a parameter and CustomPainterImage as a parameter for
/// registering the vertex buffer and index buffer of shape3d into the descriptor set of
/// CustomPainterImage as storage buffers.
///
/// @param inShape3d This has the primitive (vertex, index buffers) to be registered to descriptor
/// of inCustomImagePainter
///
/// @param inCustomImagePainter This has the image and a descriptor (bad design but too late to
/// change) to which the primitive of inShape3d (vertex, index buffer) is registered in the given
/// binding
///
/// @param  inVertexStorageBufferIndex the binding index for storage buffer of Vertex Buffer
/// @param  inIndexStorageBufferIndex the binding index for storage buffer of Index Buffer
///
/// @warning This CustomPainterImage's descriptor will be invalidated (and is not automatically
/// checked for), if, after this function call
/// the inShape3d buffer gets resized. So be sure to load everything and allocate enough space
/// in the vertex buffer before invoking this function.
/// @see Renderer::_3D::Shape3D class for further understanding. (it reallocates itself like
/// std::vector)
///
/// @warning Call Register() on the CustomPainterImage before this function, registering the image
/// is mandatory before registering anyother things
///
void RegisterShapeRenderer3DToCustomPainterImage(Jkr::Instance &inInstance,
                                                 Renderer::_3D::Shape &inShape3d,
                                                 Renderer::CustomPainterImage &inCustomPainterImage,
                                                 int inVertexStorageBufferIndex,
                                                 int inIndexStorageBufferIndex);

/// @brief Since Custom Painter Image also has a descriptor, this function registers ANOTHER
/// custom painter image, to the descriptor of given custom painter image.
void RegisterCustomPainterImageToCustomPainterImage(
     Jkr::Instance &inInstance,
     Renderer::CustomPainterImage &inCustomPainterImage,
     Renderer::CustomPainterImage &inCustomPainterImageTobeRegistered,
     int inIndex);

/// @brief This function stores the BRDF Lookup table, pre filtered cube and irradiance cube
/// onto the folder name provided, if the folder with the PBR Data doesn't exist.
/// if the folder with the PBR Data exists, it will load those onto the uniform3d specified.
///
/// @note This function also supports HDR file formats. (.hdr) for skybox/cubemap.
///
/// @param inPBRCacheName this is the DIRECTORY (Will the created if doesn't exist) which the
/// function will look for for PBR related Caches.
void SetupPBR(Jkr::Instance &inInstance,
              Jkr::Window &inWindow,
              Renderer::_3D::Uniform3D &inUniform3D,
              Renderer::_3D::World3D &inWorld3D,
              Renderer::_3D::Shape &inShape3D,
              int inSkyboxModelIndex,
              std::string_view inPBRCacheName,
              std::string_view inCachePrefix,
              std::string_view inBRDF_vs,
              std::string_view inBRDF_fs,
              std::string_view inIrradianceCube_vs,
              std::string_view inIrradianceCube_fs,
              std::string_view inPrefilteredCube_vs,
              std::string_view inPrefilteredCube_fs,
              std::string_view inEquirectangularToCubeMap_vs,
              std::string_view inEquirectangularToCubeMap_fs,
              std::string_view inEnvironmentCubeMapHDR);
}; // namespace Jkr
#include <Renderers/TwoD/Shape.hpp>
#include <Renderers/ThreeD/Shape3D.hpp>
#include <Renderers/ThreeD/Simple3D.hpp>
#include <Window.hpp>
#include "JkrFile.hpp"
#include "Renderers/ThreeD/World3D.hpp"

namespace Jkr::Misc {

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
void RegisterShapeRenderer3DToCustomPainterImage(Instance &inInstance,
                                                 Renderer::_3D::Shape &inShape3d,
                                                 Renderer::CustomPainterImage &inCustomPainterImage,
                                                 int inVertexStorageBufferIndex,
                                                 int inIndexStorageBufferIndex);

/// @brief Since Custom Painter Image also has a descriptor, this function registers ANOTHER
/// custom painter image, to the descriptor of given custom painter image.
void RegisterCustomPainterImageToCustomPainterImage(
     Instance &inInstance,
     Renderer::CustomPainterImage &inCustomPainterImage,
     Renderer::CustomPainterImage &inCustomPainterImageTobeRegistered,
     int inIndex);

/// @note This function also supports HDR file formats. (.hdr) for skybox/cubemap.
void SetupPBR(Instance &inInstance,
              Window &inWindow,
              Renderer::_3D::Uniform3D &inUniform3D,
              Renderer::_3D::World3D &inWorld3D,
              Renderer::_3D::Shape &inShape3D,
              int inSkyboxModelIndex,

              ///@todo Remove this
              Misc::FileJkr &inJkrFile,

              sv inCachePrefix,
              sv inBRDF_vs,
              sv inBRDF_fs,
              sv inIrradianceCube_vs,
              sv inIrradianceCube_fs,
              sv inPrefilteredCube_vs,
              sv inPrefilteredCube_fs,
              sv inEquirectangularToCubeMap_vs,
              sv inEquirectangularToCubeMap_fs,
              sv inEnvironmentCubeMapHDR);

///@brief This serializes the Uniform3D
void SerializeDeserializeUniform3D(Instance &ini,
                                   sv inIdName,
                                   Misc::FileJkr &inJkrFile,
                                   Renderer::_3D::Uniform3D &inUniform3D);

///@brief Serializes a Whole World
void SerializeDeserializeWorld3D(Instance &ini,
                                 Window &inw,
                                 sv inIdName,
                                 Misc::FileJkr &inJkrFile,
                                 Renderer::_3D::World3D &inWorld3D);
///@brief This function is to be used with the SerializeDeserializeWorld3D function
void SerializeDeserializeShape3D(Instance &ini,
                                 sv inIdName,
                                 Misc::FileJkr &inJkrFile,
                                 Renderer::_3D::Shape &inShape);

///@brief for Objects in World3D
void SerializeDeserializeObjectVector(sv inIdName,
                                      v<Renderer::_3D::Object3D> &inObject3d,
                                      Misc::FileJkr &inJkrFile);

///@brief This assists to use external shaders for Shape (2D rendering), using Simple3D, the
/// simple
/// 3D should be created externally and should be compatible with the shape2d shaders. Shape2d
/// is a renderer I wrote way back, Nearly every thing is dependent upon that, so I am not
/// changing anything there, I am writing another function for that purpose
///
///@warning Use Shape::SetFillMode for binding descriptors and Bind the Simple3D BEFORE calling
/// the
/// following function, if the Simple3D takes an image then set it FillMode::Image and
/// FillMode::Fill if else
///@warning This function is defined in JkrMisc.cpp in jkrgui executable
/// NOT USABLE IN C++ right now @todo Fix this with proper standard rules in Standards.hpp with
/// the push constant layout standards for everything and use those from there.
struct Matrix2CustomImagePainterPushConstant {
    glm::mat4 a, b;
};
void DrawShape2DWithSimple3D(Window_base &inWindow,
                             Renderer::_3D::Simple3D &inSimple3D,
                             Renderer::Shape &inShape,
                             Matrix2CustomImagePainterPushConstant inC,
                             ui inStartShapeId,
                             ui inEndShapeId,
                             Window_base::ParameterContext inParam);

}; // namespace Jkr::Misc
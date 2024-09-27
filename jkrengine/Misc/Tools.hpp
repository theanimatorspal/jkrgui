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
/// @warning This CustomPainterImage's descriptor will be invalidated (and is not automatically
/// checked for), if, after this function call
/// the inShape3d buffer gets resized. So be sure to load everything and allocate enough space
/// in the vertex buffer before invoking this function.
/// @see Renderer::_3D::Shape3D class for further understanding.
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
}; // namespace Jkr
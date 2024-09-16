#include <Renderers/TwoD/Shape.hpp>
#include <Window.hpp>

namespace Jkr {
/// @brief The Image should've been created with the same size as that of Window's deferred pass
void CopyWindowDeferredImageToShapeImage(Window &inWindow, Renderer::Shape &inShape2d, int inId);
void CopyWindowDeferredImageToCustomPainterImage(
     Window &inWindow, Renderer::CustomPainterImage &inCustomPainterImage);
}; // namespace Jkr
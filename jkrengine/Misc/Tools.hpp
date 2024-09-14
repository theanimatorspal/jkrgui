#include <Renderers/TwoD/Shape.hpp>
#include <Window.hpp>

namespace Jkr {
/// @brief The Image should've been created with the same size as that of Window's deferred pass
void CopyWindowDeferredImageToShapeImage(Window &inWindow, Renderer::Shape &inShape2d, int inId);
}; // namespace Jkr
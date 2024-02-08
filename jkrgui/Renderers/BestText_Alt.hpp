#include "BestText_base.hpp"
#include "TextD/BestText.hpp"
#include "TwoD/Shape.hpp"
#include <Renderers/Renderer_base.hpp>
#include <Vendor/Tracy/tracy/Tracy.hpp>
#include <Vendor/stbi/stb_image_write.h>

/*
An alternate renderer of the Best Text Renderer for low power platforms
This implementation renders the text into an image and uses the Shape Renderer
to display into a rectangle
*/

namespace Jkr::Renderer {
class BestText_Alt {

public:
    struct ImageId {
        ui mRectId;
        ui mImgId;
    };
    BestText_Alt(Shape& inSh, BestText& inBestText)
        : sh(inSh)
        , bt(inBestText)
    {
    }

    void Add(ui inFontId, glm::vec2 inPos, int inDepth, const sv inText, ImageId& outId, ksai::optref<ksai::ThreadPool> inThreadPool = std::nullopt, bool inAlignBottom = true);
    void Update(ImageId inId, ui inFontId, glm::vec2 inPos, int inDepth, const sv inText, ksai::optref<ksai::ThreadPool> inThreadPool = std::nullopt, bool inAlignBottom = true);
    void Draw(ImageId inId, Window& inWindow, glm::vec4 inColor, ui inW, ui inH, glm::mat4 inMatrix);

private:
    Shape& sh;
    BestText& bt;
};
}

#include <Renderers/Renderer_base.hpp>
#include "BestText_base.hpp"
#include <Vendor/stbi/stb_image_write.h>
/* An alternate renderer of the Best Text Renderer for low power platforms */
namespace Jkr::Renderer {
class BestText_Alt : public BestText_base, Renderer_base {
    using bb = BestText_base;
    using rb = Renderer_base;

public:
    std::vector<uc> Add(ui inFontShapeId, sv inStringView)
    {
        std::vector<uc> img;
        auto TextDimensions = RenderTextToImage(inStringView, inFontShapeId, img);
        stbi_write_bmp("File.bmp", TextDimensions.mWidth, TextDimensions.mHeight, 4, img.data());
        return img;
    }
};
}

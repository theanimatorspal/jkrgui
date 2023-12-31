#include "BestText_base.hpp"
#include "TextD/BestText.hpp"
#include "TwoD/Shape.hpp"
#include <Renderers/Renderer_base.hpp>
#include <Vendor/stbi/stb_image_write.h>

/*
An alternate renderer of the Best Text Renderer for low power platforms
This implementation renders the text into an image and uses the Shape Renderer
to display into a triangle
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

    void Add(ImageId& outId, ui inFontId, glm::vec2 inPos, int inDepth, const sv inText)
    {
        Jkr::Renderer::BestText_base::TextDimensions dimens;
        using namespace ksai;
        v<uc> img = bt.RenderTextToImage(inFontId, inText, dimens);
        ui font_image;
        sh.AddImage(img, dimens.mWidth, dimens.mHeight, font_image);

        ui font_rect_id;
        Jkr::Generator FontRectGen(Jkr::Shapes::Rectangle, glm::uvec2(dimens.mWidth, dimens.mHeight));
        sh.Add(FontRectGen, inPos.x, inPos.y, inDepth, font_rect_id);
        outId = ImageId { .mRectId = font_rect_id, .mImgId = font_image };
    }

    void Update() { }

    void Draw(ImageId inId, Window& inWindow, glm::vec4 inColor, ui inW, ui inH, glm::mat4 inMatrix)
    {
        sh.BindImage(inWindow, inId.mImgId);
        sh.Draw(inWindow, inColor, inW, inH, inId.mRectId, inId.mRectId, inMatrix);
    }

private:
    Shape& sh;
    BestText& bt;
};
}

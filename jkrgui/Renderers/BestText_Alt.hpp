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

    void Add(ui inFontId, glm::vec2 inPos, int inDepth, const sv inText, ImageId& outId, ksai::optref<ksai::ThreadPool> inThreadPool = std::nullopt, bool inAlignBottom = true)
    {
        ZoneScoped;
        Jkr::Renderer::BestText_base::TextDimensions dimens;
        using namespace ksai;
        int y;
        v<uc> img = bt.RenderTextToImage(inFontId, inText, dimens, inThreadPool, y);
        ui font_image;
        sh.AddImage(img, dimens.mWidth, dimens.mHeight, font_image);
        ui font_rect_id;
        Jkr::Generator FontRectGen(Jkr::Shapes::Rectangle_Fill, glm::uvec2(dimens.mWidth, dimens.mHeight));
        if (inAlignBottom) {
            sh.Add(FontRectGen, inPos.x, inPos.y - dimens.mHeight + y, inDepth, font_rect_id);
        } else {
            sh.Add(FontRectGen, inPos.x, inPos.y, inDepth, font_rect_id);
        }
        outId = ImageId { .mRectId = font_rect_id, .mImgId = font_image };
    }

    void Update(ImageId inId, ui inFontId, glm::vec2 inPos, int inDepth, const sv inText, ksai::optref<ksai::ThreadPool> inThreadPool = std::nullopt, bool inAlignBottom = true)
    {
        Jkr::Renderer::BestText_base::TextDimensions dimens;
        using namespace ksai;
        int y;
        v<uc> img = bt.RenderTextToImage(inFontId, inText, dimens, inThreadPool, y);
        sh.UpdateImage(inId.mImgId, img, dimens.mWidth, dimens.mHeight);
        Jkr::Generator FontRectGen(Jkr::Shapes::Rectangle_Fill, glm::uvec2(dimens.mWidth, dimens.mHeight));
        if (inAlignBottom) {
            sh.Update(inId.mRectId, FontRectGen, inPos.x, inPos.y - dimens.mHeight + y, inDepth);
        } else {
            sh.Update(inId.mRectId, FontRectGen, inPos.x, inPos.y, inDepth);
        }
    }

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

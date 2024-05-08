#include "BestText_Alt.hpp"
#include "Renderers/Renderer_base.hpp"
using namespace ksai;

Jkr::Renderer::BestText_Alt::ImageId
Jkr::Renderer::BestText_Alt::AddEXT(ui inFontId, glm::vec3 inPos, const sv inText) {
    ImageId outId;
    Jkr::Renderer::BestText_base::TextDimensions dimens;
    v<uc> img = bt.RenderTextToImage(inFontId, inText, dimens, i.GetThreadPool());
    ui font_image;
    sh.AddImage(img, dimens.mWidth, dimens.mHeight, font_image);
    ui font_rect_id;
    Jkr::Generator FontRectGen(Jkr::Shapes::RectangleFill,
                               glm::uvec2(dimens.mWidth, dimens.mHeight));
    sh.Add(FontRectGen, inPos.x, inPos.y, inPos.z, font_rect_id);
    outId = ImageId{.mRectId = font_rect_id, .mImgId = font_image};
    return outId;
}

void Jkr::Renderer::BestText_Alt::UpdateEXT(ImageId inId,
                                            ui inFontId,
                                            glm::vec3 inPos,
                                            const sv inText) {
    Jkr::Renderer::BestText_base::TextDimensions dimens;
    v<uc> img = bt.RenderTextToImage(inFontId, inText, dimens, i.GetThreadPool());
    sh.UpdateImage(inId.mImgId, img, dimens.mWidth, dimens.mHeight);
    Jkr::Generator FontRectGen(Jkr::Shapes::RectangleFill,
                               glm::uvec2(dimens.mWidth, dimens.mHeight));
    sh.Update(inId.mRectId, FontRectGen, inPos.x, inPos.y, inPos.z);
}

void Jkr::Renderer::BestText_Alt::UpdatePosOnlyEXT(ImageId inId,
                                                   ui inFontId,
                                                   glm::vec3 inPos,
                                                   const sv inText) {
    Jkr::Renderer::BestText_base::TextDimensions dimens = bt.GetTextDimensions(inText, inFontId);
    Jkr::Generator FontRectGen(Jkr::Shapes::RectangleFill,
                               glm::uvec2(dimens.mWidth, dimens.mHeight));
    sh.Update(inId.mRectId, FontRectGen, inPos.x, inPos.y, inPos.z);
}

void Jkr::Renderer::BestText_Alt::DrawEXT(
     ImageId inId, Window& inWindow, glm::vec4 inColor, glm::mat4 inMatrix, CmdParam inParam) {
    sh.BindImage(inWindow, inId.mImgId, inParam);
    sh.DrawEXT(inWindow, inColor, inId.mRectId, inId.mRectId, inMatrix, inParam);
}
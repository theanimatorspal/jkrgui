#include "BestText_Alt.hpp" 
using namespace ksai;

void Jkr::Renderer::BestText_Alt::Add(ui inFontId, glm::vec2 inPos, int inDepth, const sv inText, ImageId& outId, ksai::optref<ksai::ThreadPool> inThreadPool, bool inAlignBottom)
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

void Jkr::Renderer::BestText_Alt::Update(ImageId inId, ui inFontId, glm::vec2 inPos, int inDepth, const sv inText, ksai::optref<ksai::ThreadPool> inThreadPool, bool inAlignBottom)
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

void Jkr::Renderer::BestText_Alt::Update(ImageId inId, ui inFontId, glm::vec2 inPos, int inDepth, const sv inText, bool inAlignBottom)
{
    int y = 0;
    Jkr::Renderer::BestText_base::TextDimensions dimens = bt.GetTextDimensions(inText, inFontId, y);
    Jkr::Generator FontRectGen(Jkr::Shapes::Rectangle_Fill, glm::uvec2(dimens.mWidth, dimens.mHeight));
    if (inAlignBottom) {
        sh.Update(inId.mRectId, FontRectGen, inPos.x, inPos.y - dimens.mHeight + y, inDepth);
    } else {
        sh.Update(inId.mRectId, FontRectGen, inPos.x, inPos.y, inDepth);
    }
}

void Jkr::Renderer::BestText_Alt::Draw(ImageId inId, Window& inWindow, glm::vec4 inColor, ui inW, ui inH, glm::mat4 inMatrix)
{
    sh.BindImage(inWindow, inId.mImgId);
    sh.Draw(inWindow, inColor, inW, inH, inId.mRectId, inId.mRectId, inMatrix);
}

// TODO Remove Above Functions

Jkr::Renderer::BestText_Alt::ImageId Jkr::Renderer::BestText_Alt::AddEXT(ui inFontId, glm::vec3 inPos, const sv inText)
{   
    ImageId outId;
    Jkr::Renderer::BestText_base::TextDimensions dimens;
    v<uc> img = bt.RenderTextToImage(inFontId, inText, dimens, i.GetThreadPool());
    ui font_image;
    sh.AddImage(img, dimens.mWidth, dimens.mHeight, font_image);
    ui font_rect_id;
    Jkr::Generator FontRectGen(Jkr::Shapes::Rectangle_Fill, glm::uvec2(dimens.mWidth, dimens.mHeight));
    sh.Add(FontRectGen, inPos.x, inPos.y, inPos.z, font_rect_id);
    outId = ImageId { .mRectId = font_rect_id, .mImgId = font_image };
    return outId;
}

void Jkr::Renderer::BestText_Alt::UpdateEXT(ImageId inId, ui inFontId, glm::vec3 inPos, const sv inText)
{
    Jkr::Renderer::BestText_base::TextDimensions dimens;
    v<uc> img = bt.RenderTextToImage(inFontId, inText, dimens, i.GetThreadPool());
    sh.UpdateImage(inId.mImgId, img, dimens.mWidth, dimens.mHeight);
    Jkr::Generator FontRectGen(Jkr::Shapes::Rectangle_Fill, glm::uvec2(dimens.mWidth, dimens.mHeight));
    sh.Update(inId.mRectId, FontRectGen, inPos.x, inPos.y, inPos.z);
}

void Jkr::Renderer::BestText_Alt::UpdatePosOnlyEXT(ImageId inId, ui inFontId, glm::vec3 inPos, const sv inText)
{
    Jkr::Renderer::BestText_base::TextDimensions dimens = bt.GetTextDimensions(inText, inFontId);
    Jkr::Generator FontRectGen(Jkr::Shapes::Rectangle_Fill, glm::uvec2(dimens.mWidth, dimens.mHeight));
    sh.Update(inId.mRectId, FontRectGen, inPos.x, inPos.y, inPos.z);
}

void Jkr::Renderer::BestText_Alt::DrawEXT(ImageId inId, Window& inWindow, glm::vec4 inColor, glm::mat4 inMatrix)
{
    sh.BindImage(inWindow, inId.mImgId);
    sh.DrawEXT(inWindow, inColor, inId.mRectId, inId.mRectId, inMatrix);
}

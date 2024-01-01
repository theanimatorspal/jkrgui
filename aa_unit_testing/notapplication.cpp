#include "Shape_base.hpp"
#include "glm/ext/matrix_transform.hpp"
#include <EventManager.hpp>
#include <Instance.hpp>
#include <Renderers/ResourceManager.hpp>
#include <Renderers/TextD/BestText.hpp>
#include <Renderers/TextD/FastText.hpp>
#include <Renderers/TwoD/Line.hpp>
#include <Renderers/TwoD/Shape.hpp>
#include <Window.hpp>
#include <Renderers/BestText_Alt.hpp>

int main()
{
    auto Instance = Jkr::Instance();
    auto Window = Jkr::Window(Instance, "Heell", 1080 / 2, 1920 / 2);
    auto EventManager = Jkr::EventManager();
    auto RendererResources = Jkr::Renderer::ResourceManager();
    RendererResources.Load(Instance);
    auto lr = Jkr::Renderer::Line(Instance, Window, RendererResources.GetLineRendererCache());
    // auto ftx = Jkr::Renderer::FastText(Instance, Window, RendererResources.GetFastTextRendererCache());
    auto sr = Jkr::Renderer::Shape(Instance, Window, RendererResources.GetShapePainterCaches());
    auto bst = Jkr::Renderer::BestText(Instance, Window, RendererResources.GetBestTextRendererCache());
    uint32_t sid1, sid2;
    Jkr::Generator Rectangle(Jkr::Shapes::Rectangle, glm::uvec2(50, 50));
    sr.Add(Rectangle, 300, 300, 5, sid1);
    sr.Add(Rectangle, 0, 0, 5, sid2);
    uint32_t Font_id, BestText_id, BestText_length;
    uint32_t BestText_id1, BestText_length1;

    Jkr::Renderer::CustomImagePainter ImagePainter(Instance,
        "file.bin",

        R"""(
float color =  distance(xy, vec2(0, 0)) - 0.5;
imageStore(storageImage, to_draw_at, vec4(push.mColor.x * color, push.mColor.y * color, push.mColor.z * color, 1 * color));

)""",
        R"""(

layout(push_constant, std430) uniform pc {
	vec4 mPosDimen;
	vec4 mColor;
	vec4 mParam;
} push;

 )""",
        16,
        16,
        1);

    using namespace glm;

    struct pc {
        vec4 mPosDimen;
        vec4 mColor;
        vec4 mParam;
    };
    pc push_constant = { .mPosDimen = vec4(0, 0, 1, 1),
        .mColor = vec4(1, 1, 0, 1),
        .mParam = vec4(0) };
    uint32_t image_to_draw_id;
    sr.AddImage(100, 100, image_to_draw_id);
    ImagePainter.Store(Window);
    ImagePainter.RegisterImageToBeDrawnTo(Window, 100, 100);
    sr.CopyToImage(image_to_draw_id, ImagePainter);

    bst.AddFontFace("font.ttf", 8, Font_id);
    bst.AddText("कस्मिन् विषये कार्यं कर्तुमिच्छसि मानव !", 100, 300, 5, BestText_id, BestText_length);
    bst.AddText("don", 500, 300, 5, BestText_id1, BestText_length1);

    Jkr::Renderer::BestText_Alt Alt(sr, bst);
    Jkr::Renderer::BestText_Alt::ImageId fntimgid;
    Alt.Add(Font_id, glm::vec2(100, 100), 5, "Dipesh Regmi", fntimgid);

    auto RenderFontImage = [&]() {
        sr.BindShapes(Window);
        sr.BindFillMode(Jkr::Renderer::FillType::Image, Window);
        Alt.Draw(fntimgid, Window, glm::vec4(1, 1, 1, 1), 1920 / 2, 1080 / 2, glm::identity<glm::mat4>());
        bst.Bind(Window);
        bst.Draw(Window, glm::vec4(1, 1, 1, 1), 1920 / 2, 1080 / 2, BestText_id, BestText_length, glm::identity<glm::mat4>());
    };

    Jkr::Generator CircleGen(Jkr::Shapes::Circle, glm::uvec2(50, 16));
    uint32_t circle_id;
    sr.Add(CircleGen, 100, 100, 20, circle_id);

    std::array<glm::uvec2, 4> Pts;
    Pts[0] = glm::uvec2(300, 150);
    Pts[1] = glm::uvec2(500, 100);
    Pts[2] = glm::uvec2(300, 50);
    Pts[3] = glm::uvec2(500, 0);
    Jkr::Generator::Arguments x = Pts;
    Jkr::Generator Bezier(Jkr::Shapes::Bezier2_8, x);

    uint32_t bez_id;
    sr.Add(Bezier, 100, 100, 6, bez_id);

    uint32_t sidn;
    for (int i = 0; i < 3; i++){
        uint32_t id;
        lr.AddLine(glm::vec2(i * 20, 0), glm::vec2(100, 100), 5, id);
        sr.Add(Rectangle, i * 20, 300, 5, sidn);
    }
    uint32_t xxx;
    uint32_t id;
    uint32_t idx;

    auto Dispatch = [&](void* data) {
        ImagePainter.Draw<pc>(Window, push_constant, 16, 16, 1);
        lr.Dispatch(Window);
        sr.Dispatch(Window);
        bst.Dispatch(Window);
    };

    static int i = 0;
    auto Draw = [&](void* data) {
        RenderFontImage();
    };

    auto Update = [&](void* data) {
    };

    Window.SetUpdateCallBack(Update);
    Window.SetDrawCallBack(Draw);
    Window.SetComputeDispatchCallBack(Dispatch);

    while (!EventManager.ShouldQuit()) {
        EventManager.ProcessEvents();
        Window.Draw(0.2f, 0.2f, 0.2f, 0.2f);
    }
    return 0;
}

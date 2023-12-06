#include <EventManager.hpp>
#include <Instance.hpp>
#include <Renderers/ResourceManager.hpp>
#include <Renderers/TextD/BestText.hpp>
#include <Renderers/TextD/FastText.hpp>
#include <Renderers/TwoD/Line.hpp>
#include <Renderers/TwoD/Shape.hpp>
#include <Window.hpp>

int not_main()
{
    auto Instance = Jkr::Instance();
    auto Window = Jkr::Window(Instance, "Heell", 1080 / 2, 1920 / 2);
    auto EventManager = Jkr::EventManager();
    auto RendererResources = Jkr::Renderer::ResourceManager();
    RendererResources.Load(Instance);
    auto lr = Jkr::Renderer::Line(Instance, Window, RendererResources.GetLineRendererCache());
    auto ftx = Jkr::Renderer::FastText(Instance, Window, RendererResources.GetFastTextRendererCache());
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

    struct pc
    {
        vec4 mPosDimen;
        vec4 mColor;
        vec4 mParam;
    };
    pc push_constant = {.mPosDimen = vec4(0, 0, 1, 1),
                        .mColor = vec4(1, 1, 0, 1),
                        .mParam = vec4(0)};
    uint32_t image_to_draw_id;
    sr.AddImage(100, 100, image_to_draw_id);
    ImagePainter.Store(Window);
    ImagePainter.RegisterImageToBeDrawnTo(Window, 100, 100);
    sr.CopyToImage(image_to_draw_id, ImagePainter);

    bst.AddFontFace("font.ttf", 8, Font_id);
    bst.AddText("Wow", 100, 300, 5, BestText_id, BestText_length);
    bst.AddText("don", 500, 300, 5, BestText_id1, BestText_length1);

    std::array<glm::uvec2, 4> Pts;
    Pts[0] = glm::uvec2(300, 150);
    Pts[1] = glm::uvec2(500, 100);
    Pts[2] = glm::uvec2(300, 50);
    Pts[3] = glm::uvec2(500, 0);
    Jkr::Generator::Arguments x = Pts;
    Jkr::Generator Bezier(Jkr::Shapes::Bezier2_8, x);

    uint32_t bez_id;
    sr.Add(Bezier, 100, 100, 6, bez_id);

    uint32_t imageId;
    sr.AddImage("image.png", imageId);
    uint32_t imageId2;
    sr.AddImage("image.png", imageId2);

    uint32_t sidn;
    for (int i = 0; i < 3; i++) {
        uint32_t id;
        lr.AddLine(glm::vec2(i * 20, 0), glm::vec2(100, 100), 5, id);
        sr.Add(Rectangle, i * 20, 300, 5, sidn);
    }
    uint32_t xxx;
    uint32_t id;
    uint32_t idx;
    ftx.AddText("Hello", 300, 300, 5, xxx);
    ftx.AddText("World", 400, 300, 5, idx);
    ftx.AddText("Fucko", 500, 300, 5, id);
    ftx.UpdateText(id, "Hello", 500, 300, 5);

    auto Dispatch = [&](void *data) {
        ImagePainter.Draw<pc>(Window, push_constant, 16, 16, 1);
        lr.Dispatch(Window);
        ftx.Dispatch(Window);
        sr.Dispatch(Window);
        bst.Dispatch(Window);
    };

    static int i = 0;
    auto Draw = [&](void* data) {
        glm::mat4 matrixF = glm::identity<glm::mat4>();
        auto ws = Window.GetWindowSize();
        lr.Bind(Window);
        lr.DrawAll(Window, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), ws.first, ws.second, matrixF);
        ftx.Bind(Window);
        ftx.DrawAll(Window, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), ws.first, ws.second, matrixF);

        sr.BindShapes(Window);
        matrixF = glm::translate(matrixF, glm::vec3(1.0f * i, 0.0f, 0.0f));

        sr.BindFillMode(Jkr::Renderer::FillType::Image, Window);
        sr.BindImage(Window, image_to_draw_id);
        sr.Draw(Window, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), ws.first, ws.second, sid1, sid1, matrixF);
        matrixF = glm::translate(matrixF, glm::vec3(-5.0f * i, 0.0f, 0.0f));
        sr.Draw(Window, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), ws.first, ws.second, sid1, sid1, matrixF);
        sr.BindFillMode(Jkr::Renderer::FillType::ContinousLine, Window);
        sr.Draw(Window, glm::vec4(1.0f, 0.3f, 0.5f, 1.0f), ws.first, ws.second, bez_id, bez_id, glm::identity<glm::mat4>());
        bst.Bind(Window);

        matrixF = glm::identity<glm::mat4>();
        bst.Draw(Window, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), ws.first, ws.second, BestText_id, BestText_length, matrixF);
        bst.Draw(Window, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), ws.first, ws.second, BestText_id1, BestText_length1, matrixF);
    };

    auto Update = [&](void* data) {
        // lr.AddLine(glm::vec2(10 * 20, i), glm::vec2(100, 100), 5, id);
        // ftx.AddText("Fucko", 500, 300, 5, id);
		bst.UpdateText(BestText_id, "mo", 100, 300, 5);
		bst.UpdateText(BestText_id1, "od", 500, 300, 5);
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

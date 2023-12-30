#include <EventManager.hpp>
#include <Instance.hpp>
#include <Renderers/ThreeD/glTF.hpp>
#include <Window.hpp>
std::string vshader = R"""(

layout(push_constant, std430) uniform pc {
    mat4 mvp;
} push;

void GlslMain()
{
	gl_Position = push.mvp * vec4(inPosition, 1.0f);	
}

)""";

std::string fshader = R"""(
layout(location = 0) out vec4 out_color;

void GlslMain()
{
	out_color = vec4(1, 0, 0, 1);
}

)""";

std::string cshader = R"""(
void GlslMain()
{
}
)""";

int main()
{
    std::stringstream s;
    auto Instance = Jkr::Instance();
    auto Window = Jkr::Window(Instance, "Heell", 1080 / 2, 1920 / 2);
    auto EventManager = Jkr::EventManager();

    Jkr::Renderer::_3D::glTF r(Instance, Window);
    s << r.GetGlslVertexShaderHeaderString() << vshader;
    std::string vs = s.str();
    s.str(std::string());

    s << r.GetGlslFragmentShaderHeaderString() << fshader;
    std::string fs = s.str();
    s.str(std::string());

    s << r.GetGlslComputeShaderHeaderString() << cshader;
    std::string cs = s.str();
    s.str(std::string());

    uint32_t pid;
    r.AddPainter("Cache/simpleshader.bin", vs, fs, cs, Window, pid, true);
    uint32_t objid;
    r.Add("Box.gltf", objid);

    struct PushConstant {
        glm::mat4 mat;
    } push;

    glm::mat4 model = glm::identity<glm::mat4>();
    glm::mat4 view = glm::lookAt(glm::vec3(20, 20, 20), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 proj = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 100.0f);

    push.mat = proj * view * model;

    Window.SetUpdateCallBack([](void*) {});
    Window.SetDrawCallBack([&](void*) {
        r.Bind(Window);
        r.PainterBindDraw(pid, Window);
        r.PainterDraw<PushConstant>(objid, push, Window);
	    });
    Window.SetComputeDispatchCallBack([](void*) {});
    Window.SetComputeDispatchCallBack([&](void*) {
        r.Dispatch(Window);
	    });

    while (!EventManager.ShouldQuit()) {
        EventManager.ProcessEvents();
        Window.Draw(0.2f, 0.2f, 0.2f, 0.2f);
    }
}

#include <EventManager.hpp>
#include <Instance.hpp>
#include <Renderers/ThreeD/glTF.hpp>
#include <Window.hpp>
std::string vshader = R"""(
layout (location = 0) out vec2 vUV;
layout(push_constant, std430) uniform pc {
    mat4 mvp;
} push;
layout(set = 0, binding = 1) uniform UBO {
	mat4 view;
	mat4 proj;
} ubo;


void GlslMain()
{
	gl_Position = ubo.proj * ubo.view * push.mvp * vec4(inPosition, 1.0f);	
	vUV = inUV;
}

)""";

std::string fshader = R"""(
layout(location = 0) in vec2 vUV;
layout(location = 0) out vec4 out_color;
layout(set = 0, binding = 0) uniform sampler2D image;

layout(push_constant, std430) uniform pc {
	mat4 mvp;
	vec3 ro_met;
	vec3 rgb;
} push;

const float PI = 3.14159;

// Normal Distribution Function

float D_GGX(float dotNH, float roughness)
{
	float alpha = roughness * roughness;
	float alpha2 = alpha * alpha;
	float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
	return (alpha2)/ (PI * denom * denom);
}

void GlslMain()
{
	out_color = texture(image, vUV);
}

)""";

std::string cshader = R"""(
layout(set = 0, binding = 0) uniform sampler2D image;
layout(push_constant, std430) uniform pc {
	vec4 light;
} push;

void GlslMain()
{
}

)""";

int not_main_fuck_yuou()
{
    std::stringstream s;
    auto Instance = Jkr::Instance();
    auto Window = Jkr::Window(Instance, "Heell", 1080 / 2, 1920 / 2);
    auto EventManager = Jkr::EventManager();

    Jkr::Renderer::_3D::Shape r(Instance, Window);
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
    r.Add("BoxTextured.gltf", objid);
    r.AddModelTextureToPainter(objid, pid, 0, 0, 0);



    struct PushConstant {
        glm::mat4 mat;
        glm::vec3 ro_met;
        glm::vec3 rgb;
    } push;

    glm::mat4 model = glm::identity<glm::mat4>();
    glm::mat4 view = glm::lookAt(glm::vec3(1, -4, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 proj = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 100.0f);

    using namespace Jkr::Renderer::_3D;

    struct UB {
        glm::mat4 view;
        glm::mat4 proj;
        glm::vec4 lights[10];
    } ub;


    ub = { .view = view, .proj = proj };
    ub.lights[0] = glm::vec4(5, 5, 5, 0.7);

    r.WriteToGlobalUB(ub);
    r.RegisterGlobalUBToPainter(pid, 0, 1, 0);

    push.mat = model;

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

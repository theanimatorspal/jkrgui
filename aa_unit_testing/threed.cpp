#include <EventManager.hpp>
#include <Instance.hpp>
#include <Renderers/ThreeD/glTF.hpp>
#include <Window.hpp>
#include <WindowMulT.hpp>
#include <Vendor/Tracy/tracy/TracyVulkan.hpp>


const std::string_view vshader = R"""(
layout (location = 0) out vec2 vUV;
layout (location = 1) out vec3 vNormal;
layout(push_constant, std430) uniform pc {
	mat4 mvp;
	vec3 rough;
	vec3 rgb;
} push;

layout(set = 0, binding = 1) uniform UBO {
   mat4 view;
   mat4 proj;
   vec3 campos;
   vec4 lights[10];
} ubo;

void GlslMain()
{
	gl_Position = ubo.proj * ubo.view * push.mvp * vec4(inPosition, 1.0f);	
	vUV = inUV;
	vNormal = inNormal;
}

)""";

const std::string_view fshader = R"""(
	layout(location = 0) in vec2 vUV;
	layout(location = 1) in vec3 vNormal;
	layout(location = 0) out vec4 out_color;
	layout(set = 0, binding = 0) uniform sampler2D image;

	layout(set = 0, binding = 1) uniform UBO {
	   mat4 view;
	   mat4 proj;
	   vec4 campos;
	   vec4 lights[10];
	} ubo;

	layout(push_constant, std430) uniform pc {
		mat4 mvp;
		vec3 roughness;
		vec3 rgb;
	} push;

	const float PI = 3.14159;
	vec3 materialcolor()
	{
		return vec3(texture(image, vUV));
	}

	// Normal Distribution Function

	float D_GGX(float dotNH, float roughness)
	{
		float alpha = roughness * roughness;
		float alpha2 = alpha * alpha;
		float denom = dotNH * dotNH * (alpha2 - 1.0) + 1.0;
		return (alpha2)/ (PI * denom * denom);
	}

	// Geometric Shadowing Function
	float G_SchlicksmithGGX(float dotNL, float dotNV, float roughness)
	{
		float r = roughness + 1.0;
		float k = (r * r) / 8.0;
		float GL = dotNL / (dotNL * (1.0 - k) + k);
		float GV = dotNV / (dotNV * (1.0 - k) + k);
		return GL * GV;
	}

	// Fresnel Function
	vec3 F_Schlick(float cosTheta, float metallic)
	{
		vec3 F0 = mix(vec3(0.04), materialcolor(), metallic); // material.specular
		vec3 F = F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
		return F;
	}

	// Specular BRDF composition
	vec3 BRDF(vec3 L, vec3 V, vec3 N, float metallic, float roughness)
	{
		vec3 H = normalize(V + L);
		float dotNV = clamp(dot(N, V), 0.0, 1.0);
		float dotNL = clamp(dot(N, L), 0.0, 1.0);
		float dotLH = clamp(dot(L, H), 0.0, 1.0);
		float dotNH = clamp(dot(N, H), 0.0, 1.0);

		// Light color fixed
		vec3 lightColor = vec3(1.0);

		vec3 color = vec3(0.0);

		if (dotNL > 0.0)
		{
			float rroughness = max(0.05, roughness);
			// D = Normal distribution (Distribution of the microfacets)
			float D = D_GGX(dotNH, roughness); 
			// G = Geometric shadowing term (Microfacets shadowing)
			float G = G_SchlicksmithGGX(dotNL, dotNV, rroughness);
			// F = Fresnel factor (Reflectance depending on angle of incidence)
			vec3 F = F_Schlick(dotNV, metallic);

			vec3 spec = D * F * G / (4.0 * dotNL * dotNV);

			color += spec * dotNL * lightColor;
		}

		return color;
	}

	void GlslMain()
	{
		vec3 N = normalize(vNormal);
		vec3 V = normalize(ubo.campos - vec3(0, 0, 0)); // TODO ubo.campos - PositioninWorld
		float roughness = push.roughness.x;
		// Specular Contribution
		vec3 Lo = vec3(0.0);
		vec3 L = normalize(vec3(ubo.lights[0]) - vec3(0, 0, 0)); // in World ko position
		Lo += BRDF(L, V, N, 1, roughness);

		// Combination With Ambient
		vec3 color = materialcolor() * 0.02;
		color += Lo;
		color = pow(color, vec3(0.4545));
		out_color = vec4(color, 1.0);
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

int oajdsfklsajldfkja()
{

    struct UB {
        glm::mat4 view;
        glm::mat4 proj;
        glm::vec4 campos;
        glm::vec4 lights[10];
    } ub;
    std::stringstream s;
    auto Instance = Jkr::Instance();
    std::vector<ksai::ui> CmdBufferCountPerThread;
    uint32_t NoOfThreads = Instance.GetThreadPool().mThreads.size();
    CmdBufferCountPerThread.resize(NoOfThreads, 2);
    auto Window = Jkr::WindowMulT(Instance, "Heell", 1080 / 2, 1920 / 2, NoOfThreads, CmdBufferCountPerThread, Instance.GetThreadPool());
    auto EventManager = Jkr::EventManager();
    SpirvHelper::Init();

    Jkr::Renderer::_3D::Shape r(Instance, Window, sizeof(UB));
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
    } ;
    PushConstant push;
    PushConstant push2;

    glm::mat4 model = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(-1, 0, -5));
    glm::mat4 view = glm::lookAt(glm::vec3(1, -4, 1), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 proj = glm::perspective(glm::radians(90.0f), 16.0f / 9.0f, 0.1f, 100.0f);

    using namespace Jkr::Renderer::_3D;

    ub = { .view = view, .proj = proj, .campos = glm::vec4(1, -4, 1, 1) };
    ub.lights[0] = glm::vec4(5, -2, 5, 0.7);

    r.WriteToGlobalUB(ub);
    r.RegisterGlobalUBToPainter(pid, 0, 1, 0);

    push.mat = model;
    push.ro_met = glm::vec3(0.5, 0, 0);
    push.rgb = glm::vec3(0.6, 0.2, 0.5);

    Window.SetUpdateCallBack([](void*) {});

    auto itr = 10000;
    Window.SetBackgroundCallback(
        [&](void*) {
            for (int i = 0; i < itr; i++) {
                r.Bind(Window, Jkr::Window::ParameterContext::Background);
                r.PainterBindForDraw(pid, Window, Jkr::Window::ParameterContext::Background);
                push.mat = glm::translate(model, glm::vec3(i / 10.0f, i / 10.0f, i / 10.0f));
                push.mat = glm::rotate(push.mat, Window.GetWindowCurrentTime() / 1000.0f * i, glm::vec3(0, 1, 0));
                r.PainterDraw<PushConstant>(objid, push, Window, Jkr::Window::ParameterContext::Background);
                //TracyVkZone(Window.mctx_ui1, Window.GetCommandBuffers(Jkr::Window::ParameterContext::Background)[Window.GetCurrentFrame()].GetCommandBufferHandle(), "BG1");
                ZoneScoped;
            }
            ZoneScoped;
        });

    glm::mat4 model2 = glm::identity<glm::mat4>();
    model = glm::translate(model, glm::vec3(-5, -5, -5));
    push2.mat = model;
    push2.ro_met = glm::vec3(0.5, 0, 0);
    push2.rgb = glm::vec3(1.0, 0.2, 0.5);
    Window.SetDrawCallBack(
        [&](void*) {
            for (int i = 0; i < itr; i++) {
                auto ctime = Window.GetWindowCurrentTime() / 1000.0f;
                r.Bind(Window, Jkr::Window::ParameterContext::UI);
                r.PainterBindForDraw(pid, Window, Jkr::Window::ParameterContext::UI);
                push2.mat = glm::translate(model2, glm::vec3(i / 10.0f, i / 10.0f, i / 10.0f));
                push2.mat = glm::rotate(push2.mat, Window.GetWindowCurrentTime() / 1000.0f * i, glm::vec3(0, 1, 0));
                r.PainterDraw<PushConstant>(objid, push2, Window, Jkr::Window::ParameterContext::UI);
                //TracyVkZone(Window.mctx_ui1, Window.GetCommandBuffers(Jkr::Window::ParameterContext::UI)[Window.GetCurrentFrame()].GetCommandBufferHandle(), "UI1");
                ZoneScoped;
            }
            ZoneScoped;
        });

    Window.SetComputeDispatchCallBack([&](void*) {
        r.Dispatch(Window);
    });

    SpirvHelper::Finalize();

    while (!EventManager.ShouldQuit()) {
        EventManager.ProcessEvents();
        Window.Draw(0.2f, 0.2f, 0.2f, 0.2f);
	   FrameMark;
    }
}

int main ( ) {
}

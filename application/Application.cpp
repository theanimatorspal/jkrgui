#include <Instance.hpp>
#include <Window.hpp>
#include <EventManager.hpp>
#include <Renderers/ResourceManager.hpp>
#include <Renderers/Line.hpp>
#include <Renderers/FastText.hpp>
#include <sol/sol.hpp>

int main()
{
	auto Instance = Jkr::Instance();
	auto Window = Jkr::Window(Instance, "Heell", 1080 / 2, 1920 / 2);
	auto EventManager = Jkr::EventManager();
	auto RendererResources = Jkr::Renderer::ResourceManager();
	auto lua = sol::state();
	RendererResources.Load(Instance);
	auto lr = Jkr::Renderer::Line(Instance, Window, RendererResources.GetLineRendererCache());
	auto ftx = Jkr::Renderer::FastText(Instance, Window, RendererResources.GetFastTextRendererCache());

	for (int i = 0; i < 3; i++)
	{
		uint32_t id;
		lr.AddLine(glm::vec2(i * 20, 0), glm::vec2(100, 100), 5, id);
	}
	uint32_t xxx;
	uint32_t id;
	uint32_t idx;
	ftx.AddText("Hello", 300, 300, 5, xxx);
	ftx.AddText("World", 400, 300, 5, idx);
	ftx.AddText("Fucko", 500, 300, 5, id);
	ftx.UpdateText(id, "Hello", 500, 300, 5);

	auto Dispatch = [&](void* data)
		{
			lr.Dispatch(Window);
			ftx.Dispatch(Window);
		};
	auto Draw = [&](void* data)
		{
			lr.DrawInit(Window);
			lr.DrawAll(Window, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), Window.GetWindowSize().first, Window.GetWindowSize().second, glm::identity<glm::mat4>());
			ftx.DrawInit(Window);
			ftx.DrawAll(Window, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), Window.GetWindowSize().first, Window.GetWindowSize().second, glm::identity<glm::mat4>());
		};

	static int i = 0;
	auto Update = [&](void* data)
		{
			lr.AddLine(glm::vec2(10 * 20, i), glm::vec2(100, 100), 5, id);
			ftx.AddText("Fucko", 500, 300, 5, id);
			i++;
		};

	Window.SetUpdateCallBack(Update);
	Window.SetDrawCallBack(Draw);
	Window.SetComputeDispatchCallBack(Dispatch);


	while (!EventManager.ShouldQuit())
	{
		EventManager.ProcessEvents();
		Window.Draw();
	}
	return 0;
}
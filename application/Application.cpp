#include <Instance.hpp>
#include <Window.hpp>
#include <EventManager.hpp>
#include <Renderers/ResourceManager.hpp>
#include <Renderers/Line.hpp>
#include <Renderers/FastText.hpp>

int main()
{
	auto Instance = Jkr::Instance();
	auto Window = Jkr::Window(Instance, "Heell", 1080 / 2, 1920 / 2);
	auto EventManager = Jkr::EventManager();
	auto RendererResources = Jkr::Renderer::ResourceManager();
	RendererResources.Load(Instance);
	auto lr = Jkr::Renderer::Line(Instance, Window, RendererResources.GetLineRendererCache());
	uint32_t id;
	lr.AddLine(glm::vec2(0, 0), glm::vec2(100, 100), 5, id);

	auto Dispatch = [&](void* data)
		{
			lr.Dispatch(Window);
		};
	auto Draw = [&](void* data)
		{
			lr.DrawInit(Window);
			lr.DrawBatched(Window, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), Window.GetWindowSize().first, Window.GetWindowSize().second, 0, 1, glm::identity<glm::mat4>());
		};

	Window.SetDrawCallBack(Draw);
	Window.SetComputeDispatchCallBack(Dispatch);


	while (!EventManager.ShouldQuit())
	{
		EventManager.ProcessEvents();
		Window.Draw();
	}
	return 0;
}
#include <Instance.hpp>
#include <Window.hpp>
#include <EventManager.hpp>
#include <Renderers/ResourceManager.hpp>
#include <Renderers/Line.hpp>
#include <Renderers/FastText.hpp>
#include <Renderers/Shape.hpp>
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
	auto sr = Jkr::Renderer::Shape(Instance, Window, RendererResources.GetShapePainterCaches());
	uint32_t sid1, sid2;
	Jkr::Generator Rectangle(Jkr::Shapes::Rectangle, glm::uvec2(50, 50));
	sr.Add(Rectangle, 300, 300, 5, sid1);
	sr.Add(Rectangle, 200, 200, 5, sid2);

	Jkr::Generator Rectangle2(Jkr::Shapes::Rectangle, glm::uvec2(50, 50));

	uint32_t imageId; sr.AddImage("image.png", imageId);
	uint32_t imageId2; sr.AddImage("image.png", imageId);

	uint32_t sidn;
	for (int i = 0; i < 3; i++)
	{
		uint32_t id;
		lr.AddLine(glm::vec2(i * 20, 0), glm::vec2(100, 100), 5, id);
		sr.Add(Rectangle, i*20, 300, 5, sidn);
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
			sr.Dispatch(Window);
		};
	auto Draw = [&](void* data)
		{
			auto ws = Window.GetWindowSize();
			lr.Bind(Window);
			lr.DrawAll(Window, glm::vec4(1.0f, 1.0f, 0.0f, 1.0f), ws.first, ws.second, glm::identity<glm::mat4>());
			ftx.Bind(Window);
			ftx.DrawAll(Window, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f), ws.first, ws.second, glm::identity<glm::mat4>());

			sr.BindShapes(Jkr::Renderer::FillType::Fill, Window);
			//sr.BindFillMode(Jkr::Renderer::FillType::Fill, Window);
			//sr.Draw(Window, glm::vec4(1.0f, 0.3f, 0.5f, 1.0f), ws.first, ws.second, sid1, sid1, glm::identity<glm::mat4>());
			sr.BindFillMode(Jkr::Renderer::FillType::Image, Window);
			sr.BindImage(Window, imageId);
			sr.Draw(Window, glm::vec4(1.0f, 0.3f, 0.5f, 1.0f), ws.first, ws.second, sid1, sid1, glm::identity<glm::mat4>());
			sr.BindImage(Window, imageId2);
			sr.Draw(Window, glm::vec4(1.0f, 0.3f, 0.5f, 1.0f), ws.first, ws.second, sid2, sid2, glm::identity<glm::mat4>());
			sr.BindFillMode(Jkr::Renderer::FillType::Fill, Window);
			sr.Draw(Window, glm::vec4(1.0f, 0.3f, 0.5f, 1.0f), ws.first, ws.second, sid2, sidn, glm::identity<glm::mat4>());
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
		Window.Draw(1.0f, 1.0f, 1.0f, 0.2f);
	}
	return 0;
}
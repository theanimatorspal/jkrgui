#include <Instance.hpp>
#include <Window.hpp>
#include <EventManager.hpp>
#include <Renderers/ResourceManager.hpp>
#include <Renderers/TwoD/2d.hpp>
#include <Components/GridSheet.hpp>
#include "NodeEditor/NodeEditorScene.hpp"

int main()
{
	constexpr uint32_t h = 1080 / 2;
	constexpr uint32_t w = 1920 / 2;
	auto Instance = Jkr::Instance();
	auto Window = Jkr::Window(Instance, "Heell", 1080 / 2, 1920 / 2);
	auto EventManager = Jkr::EventManager();
	auto RendererResources = Jkr::Renderer::ResourceManager();
	RendererResources.Load(Instance);
	auto TwoD = Jkr::Renderer::_2d(Instance, Window, RendererResources);
	auto GSheet = App::NodeScene(TwoD, EventManager);
	GSheet.Load(w, h);

	auto Event = [&](void*)
		{
			auto wh = Window.GetWindowSize();
			GSheet.Event(Window, wh.first, wh.second);
		};

	EventManager.SetEventCallBack(Event);

	auto Draw = [&](void* data) {
		auto wh = Window.GetWindowSize();
		GSheet.BeginDraw();

		TwoD.ln.Bind(Window);
		GSheet.DrawLines(Window, wh.first, wh.second);

		TwoD.sh.BindShapes(Window);
		TwoD.sh.BindFillMode(Jkr::Renderer::FillType::Fill, Window);
		GSheet.DrawShapes(Window, wh.first, wh.second);

		TwoD.ft.Bind(Window);
		GSheet.DrawTexts(Window, wh.first, wh.second);

		GSheet.EndDraw();
		};
	Window.SetDrawCallBack(Draw);

	auto Update = [&](void* data)
		{
			auto wh = Window.GetWindowSize();
			GSheet.Update(wh.first, wh.second);
		};
	Window.SetUpdateCallBack(Update);

	auto Dispatch = [&](void* data)
		{
			TwoD.ft.Dispatch(Window);
			TwoD.ln.Dispatch(Window);
			TwoD.sh.Dispatch(Window);
		};
	Window.SetComputeDispatchCallBack(Dispatch);

	while (!EventManager.ShouldQuit())
	{
		EventManager.ProcessEvents();
		Window.Draw();
	}


}

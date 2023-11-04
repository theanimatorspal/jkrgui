#include <Instance.hpp>
#include <Window.hpp>
#include <EventManager.hpp>
#include <Renderers/ResourceManager.hpp>
#include <Renderers/TwoD/2d.hpp>
#include <Components/GridSheet.hpp>

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
	auto GSheet = Jkr::Component::GridSheet(TwoD, EventManager);
	GSheet.Load(w, h);

	auto Event = [&](void*)
		{
			GSheet.Event(Window, w, h);
		};

	EventManager.SetEventCallBack(Event);

	auto Draw = [&](void* data) {
		TwoD.ln.Bind(Window);
		GSheet.DrawLines(Window, w, h);
		TwoD.sh.BindShapes(Window);
		TwoD.sh.BindFillMode(Jkr::Renderer::FillType::Fill, Window);
		GSheet.DrawShapes(Window, w, h);
		TwoD.ft.Bind(Window);
		};
	Window.SetDrawCallBack(Draw);

	auto Update = [&](void* data)
		{
			GSheet.Update(w, h);
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

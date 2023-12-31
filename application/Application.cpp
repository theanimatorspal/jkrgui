#include "DrawArea.hpp"
#include <EventManager.hpp>
#include <Instance.hpp>
#include <Renderers/ResourceManager.hpp>
#include <Renderers/2d.hpp>
#include <Window.hpp>

int main()
{
    constexpr uint32_t h = 1080 / 1.2;
    constexpr uint32_t w = 1920 / 1.2;
    auto Instance = Jkr::Instance();
    auto Window = Jkr::Window(Instance, "Heell", h, w);
    auto EventManager = Jkr::EventManager();
    auto RendererResources = Jkr::Renderer::ResourceManager();
    RendererResources.Load(Instance);
    auto TwoD = Jkr::Renderer::_2d(Instance, Window, RendererResources);

    auto DrawArea = App::DrawArea(TwoD, EventManager, &Window, w, h);

    auto Event = [&](void*) {
        DrawArea.Event();
    };

    EventManager.SetEventCallBack(Event);

    auto Draw = [&](void* data) { DrawArea.Draw(); };
    Window.SetDrawCallBack(Draw);

    auto Update = [&](void* data) {
        auto wh = Window.GetWindowSize();
        DrawArea.Update(&Window, wh.first, wh.second);
    };
    Window.SetUpdateCallBack(Update);

    auto Dispatch = [&](void* data) {
        TwoD.ln.Dispatch(Window);
        TwoD.sh.Dispatch(Window);
        TwoD.bt.Dispatch(Window);
    };
    Window.SetComputeDispatchCallBack(Dispatch);

    while (!EventManager.ShouldQuit()) {
        EventManager.ProcessEvents();
        Window.Draw(0.1f, 0.1f, 0.1f, 1.0f);
    }
}

#include <EventManager.hpp>
#include <Instance.hpp>
#include <Renderers/TextD/BestText_Alt.hpp>
#include <Window.hpp>

int main()
{
    using namespace Jkr::Renderer;
    BestText_Alt alt;
    ui i;
    alt.AddFontFace("font.ttf", 64, i);
    alt.Add(i, "किमपि तद्भवति");
}

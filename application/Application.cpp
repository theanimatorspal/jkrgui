#include <Instance.hpp>
#include <GUIMainWindow.hpp>

auto main() -> int
{
	auto Instance = Jkr::Instance();
	auto Window = Jkr::GUIMainWindow(Instance, "Heell", 1080 / 2, 1920 / 2);

	while (!Window.WindowShouldClose())
	{
		Window.PollEvents();
		Window.Draw();
	}
	return 0;
}
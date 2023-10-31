#include "Application.hpp"

class EventListener
{
public:
	void MouseBarClick(App::EventManager::Data inData)
	{
		auto X = std::get<glm::ivec2>(inData);
		std::cout << "MousebarCliick(" << X.r << X.g << ")\n";
	}

	void MouseBarClick2(App::EventManager::Data inData)
	{
		auto X = std::get<glm::ivec2>(inData);
		std::cout << "MouseBarClick2(" << X.r << X.g << ")\n";
	}
};

















int main()
{
	constexpr auto Window_h = 1080 / 2;
	constexpr auto Window_w = 1920 / 2;
	const auto Instance = Jkr::Instance();
	auto MainWindow = Jkr::GUIMainWindow(Instance, "Hi", Window_h, Window_w);
	App::EventManager EventManager;
	EventListener listener;
	EventManager.RegisterListener(App::EventManager::Type::MenuBarClick, [&listener](App::EventManager::Data inData) {
		listener.MouseBarClick(inData);
		});

	auto FontPainterCache = Jkr::GUIPainterCache(Instance);
	auto GUI2DPainterCache = Jkr::GUIPainterCache(Instance);
	auto LineRendererCache = Jkr::GUIPainterCache(Instance, ksai::PipelinePropertiesContext::Line);
	LoadCaches(FontPainterCache, GUI2DPainterCache, LineRendererCache);
	Jkr::Font::FastTextRenderer Font16(Instance, MainWindow, FontPainterCache, "res/ksai_fast_text/Font32");
	Jkr::GUI2DRenderer GUI2DRenderer(Instance, MainWindow, GUI2DPainterCache);
	Jkr::Renderer::Line LineRenderer(Instance, MainWindow, LineRendererCache);



























	App::LuaConfig Config;
	MenuBar Bar(GUI2DRenderer, Font16, Config, EventManager);
	Bar.LoadConfigurationFile("MenuBarConfig.lua");
	Bar.Register(Window_w, Window_h);
	App::NodeEditorScene NodeEditorScene;
	NodeEditorScene.LoadConfigAndRegister(GUI2DRenderer, Font16, LineRenderer, Config, "NodeConfig.lua");
	EventManager.RegisterListener(App::EventManager::Type::MenuBarClick,
		[&](App::EventManager::Data inData) {
			auto X = std::get<glm::ivec2>(inData);
			NodeEditorScene.UpdateMenuBarSelection(X);
		}
	);




















	ContextData Data = {
		.mInstance = Instance,
		.mWindow = MainWindow ,
		.mTextRenderer = Font16,
		.mGUI2D = GUI2DRenderer,
		.mMenuBar = Bar,
		.mNodeEditorScene = NodeEditorScene,
		.mLineR = LineRenderer,
		.mLuaConfig = Config
	};

















	MainWindow.SetContextData(reinterpret_cast<void*>(&Data));
	MainWindow_Load(&Data);

	uint32_t FrameOverlayText_id = 0;
	Font16.RegisterDynamicText(20, FrameOverlayText_id);
	Jkr::Test::FrameRateMeasure<100> FrameRateMeasure;
	while (!MainWindow.WindowShouldClose())
	{
		FrameRateMeasure.StartFrame();
		MainWindow.PollEvents();
		MainWindow.Draw();
		FrameRateMeasure.EndFrame();
		std::string FrameRateDisplay = "FPS: " + std::to_string(FrameRateMeasure.GetAvg());
		auto windowD = MainWindow.GetWindowSize();
		Font16.UpdateDynamicText(FrameOverlayText_id, FrameRateDisplay.c_str(), windowD.first / 2, 5,
			Jkr::Font::FastTextRenderer::FeatureFlags{ .h = Jkr::Font::FastTextRenderer::HorizontalAlignment::Right, .inSizeFactor = 0.5, .v = Jkr::Font::FastTextRenderer::VerticalAlignment::Bottom }
		);
		EventManager.ProcessEvents();
	}
}


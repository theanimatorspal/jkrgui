// JkrLuaExe.cpp : Source file for your target.
//
#define SOL_PRINT_ERRORS 1
#define SOL_ALL_SAFETIES_ON 1
#include "JkrLuaExe.hpp"

static auto my_exception_handler(lua_State* L, sol::optional<const std::exception&> maybe_execption, sol::string_view description) -> int
{
	std::cout << "An Exception has occured in the function here is what it says: ";
	if (maybe_execption) {
		std::cout << "(Straight from the exception) ";
		const std::exception& ex = *maybe_execption;
		std::cout << ex.what() << std::endl;
	}
	else {
		std::cout << "(From the Descriptor Parameter) ";
		std::cout.write(description.data(), static_cast<std::streamsize>(description.size()));
		std::cout << std::endl;
	}
	return sol::stack::push(L, description);
};

using namespace std;
using namespace ksai;

extern std::string_view GetSolHpp();
extern void CreateGLMBindings(sol::state& lua);
extern void CreateKeyBindings(sol::state& lua);
extern void CopyAllHeaderFiles(std::filesystem::path& insrc, std::filesystem::path& indst);

void CreateMainBindings(sol::state& s)
{
	CreateGLMBindings(s);
	CreateKeyBindings(s);
	auto Jkr = s["Jkr"].get_or_create<sol::table>();
	Jkr.new_usertype<Jkr::Instance>(
		"Instance",
		sol::call_constructor,
		sol::factories([](int inDescriptorSize, int inPoolSize) {
			return std::make_unique<Jkr::Instance>(inDescriptorSize, inPoolSize);
			})
	);
	Jkr.new_usertype<Jkr::WindowMulT>(
		"Window",
		sol::call_constructor,
		sol::factories([](Jkr::Instance& inInstance, std::string_view inTitle, int inWidth, int inHeight) {
			int ThreadsCount = 7; // TODO for now
			int NoOfCmdBufferPerThread = 2;
			std::vector<ui> CmdBufferCountPerThreadVec;
			CmdBufferCountPerThreadVec.resize(ThreadsCount, NoOfCmdBufferPerThread);
			return std::make_unique<Jkr::WindowMulT>(inInstance, inTitle, inHeight, inWidth, ThreadsCount, CmdBufferCountPerThreadVec, inInstance.GetThreadPool());
			}),
		"SetDrawCallback",
		[](Jkr::WindowMulT& inWindow, sol::protected_function inFunction) {
			inWindow.SetDrawCallBack([inFunction](void*) { inFunction(); });
		},
		"SetComputeDispatchCallback",
		[](Jkr::WindowMulT& inWindow, sol::protected_function inFunction) {
			inWindow.SetComputeDispatchCallBack([inFunction](void*) { inFunction(); });
		},
		"Draw", &Jkr::WindowMulT::Draw,
		"SetTitle", &Jkr::WindowMulT::SetTitle,
		"BeginUpdates", &Jkr::WindowMulT::BeginUpdates,
		"EndUpdates", &Jkr::WindowMulT::EndUpdates,
		"BeginDispatches", &Jkr::WindowMulT::BeginDispatches,
		"EndDispatches", &Jkr::WindowMulT::EndDispatches,
		"BeginDraws", &Jkr::WindowMulT::BeginDraws,
		"EndDraws", &Jkr::WindowMulT::EndDraws,
		"Present", &Jkr::WindowMulT::Present,

		"BeginUIs", &Jkr::WindowMulT::BeginUIs,
		"EndUIs", &Jkr::WindowMulT::EndUIs,
		"ExecuteUIs", &Jkr::WindowMulT::ExecuteUIs,

		"GetWindowCurrentTime", &Jkr::WindowMulT::GetWindowCurrentTime
	);

	Jkr.new_usertype<Jkr::EventManager>(
		"EventManager",
		sol::call_constructor,
		sol::factories([]() {
			return mu<Jkr::EventManager>();
			}),
		"ShouldQuit", &Jkr::EventManager::ShouldQuit,
		"ProcessEvents", &Jkr::EventManager::ProcessEvents
	);

	auto PainterCacheTypeEnum = Jkr.new_enum<false>("PainterType", "Default", PipelinePropertiesContext::Default, "Line", PipelinePropertiesContext::Line, "LineStrip", PipelinePropertiesContext::LineStrip);
	Jkr.new_usertype<Jkr::PainterCache>(
		"PainterCache",
		sol::call_constructor,
		[](Jkr::Instance& inInstance, PipelinePropertiesContext inContext) {
			return mu<Jkr::PainterCache>(inInstance, inContext);
		},
		"Store",
		[](Jkr::PainterCache& inCache, string_view inFilename, string_view inVertexShader, string_view inFragmentShader, string_view inComputeShader)
		{
			inCache.Store(string(inFilename), string(inVertexShader), string(inFragmentShader), string(inComputeShader));
		},
		"Load",
		[](Jkr::PainterCache& inCache, string_view inFilename)
		{
			inCache.Load(string(inFilename));
		}
		//,
		//"VarDesStore",
		//[](Jkr::PainterCache& inCache, string_view inFilename, string_view inVertexShader, string_view inFragmentShader, string_view inComputeShader, int inVarDesCount)
		//{
		//	inCache.__var_des_index_Store_EXT(string(inFilename), string(inVertexShader), string(inFragmentShader), string(inComputeShader), inVarDesCount);
		//},
		//"VarDesLoad",
		//[](Jkr::PainterCache& inCache, string_view inFilename, int inVarDesCount)
		//{
		//	inCache.__var_des_index_Load_EXT(string(inFilename), inVarDesCount);
		//}
	);


	using namespace Jkr::Renderer;
	auto FillTypeEnum = Jkr.new_enum<false>("FillType", "Fill", FillType::Fill, "Image", FillType::Image, "ContinousLine", FillType::ContinousLine);

	struct ShapeRendererResources {
		void Add(Jkr::Instance& inInstance, FillType inFillType, PipelinePropertiesContext inContext, std::string_view inFileName, std::string_view inVertexShader, std::string_view inFragmentShader, std::string_view inComputeShader, bool inShouldStore) {
			mCaches[inFillType] = mu<PainterCache>(inInstance, inContext);
			if (inShouldStore)
			{
				mCaches[inFillType]->Store(string(inFileName), string(inVertexShader), string(inFragmentShader), string(inComputeShader));
			}
			else {
				mCaches[inFillType]->Load(string(inFileName));
			}
		}
		std::unordered_map<FillType, up<PainterCache>> mCaches;
	};

	Jkr.new_usertype<ShapeRendererResources>(
		"ShapeRendererResources",
		sol::call_constructor, 
		[]() {return ShapeRendererResources(); },
		"Add", &ShapeRendererResources::Add
	);

	Jkr.new_usertype<Jkr::Renderer::Shape>(
		"ShapeRenderer",
		sol::call_constructor,
		sol::factories([](Jkr::Instance& inInstance, Jkr::WindowMulT& inCompatibleWindow, ShapeRendererResources& inResources, int invardescount) {
			return mu<Jkr::Renderer::Shape>(inInstance, inCompatibleWindow, inResources.mCaches, invardescount);
			}
		)
	);

	Jkr.new_usertype<Jkr::Renderer::Line>(
		"LineRenderer",
		sol::call_constructor,
		sol::factories([](Jkr::Instance& inInstance, Jkr::WindowMulT& inWindow, Jkr::PainterCache& inCache) {
			return mu<Jkr::Renderer::Line>(inInstance, inWindow, inCache);
			})
		,
		"Add", &Jkr::Renderer::Line::Add,
		"Update", &Jkr::Renderer::Line::UpdateLine,
		"Dispatch", &Jkr::Renderer::Line::Dispatch,
		"Draw", &Jkr::Renderer::Line::Draw
	);
}

int main(int ArgCount, char** ArgStrings)
{
	sol::state s;
	s.open_libraries();
	using namespace std;
	CreateMainBindings(s);

	const vector<string_view> CommandLineArgs(ArgStrings + 1, ArgStrings + ArgCount);
	s.set_exception_handler(&my_exception_handler);
	sol::protected_function_result result = s.safe_script_file("app.lua", &sol::script_pass_on_error);
	if (not result.valid()) {
		sol::error error = result;
		std::cout << error.what();
		exit(EXIT_FAILURE);
	}
	return 0;
}

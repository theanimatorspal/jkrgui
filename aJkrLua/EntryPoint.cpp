#ifdef ANDROID
#undef SDL_MAIN_HANDLED
#include <SDL.h>
#include <unistd.h>
#endif

#include "EntryPoint.hpp"
#include "WindowMulT.hpp"
#include "ksai_filestream.hpp"
#include <Vendor/Tracy/tracy/TracyLua.hpp>
#include <algorithm>
#include <filesystem>


#ifdef ANDROID
#define JKR_EXPORT __attribute__((visibility("default")))
#else
#define JKR_EXPORT
#endif

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

JKR_EXPORT int main(int ArgCount, char** ArgStrings)
{
	auto& cf = gConfiguration;
	const vector<string_view> CommandLineArgs(ArgStrings + 1, ArgStrings + ArgCount);
	FillConfigFrom(CommandLineArgs);

#ifdef ANDROID
	std::string InternalPath = SDL_AndroidGetInternalStoragePath();
	std::string cd_to = InternalPath.substr(0, InternalPath.find_last_of('/') + 1);
	ksai_print(cd_to.c_str());
	// ksai_print(InternalPath.c_str());
	char initialdir[256];
	getcwd(initialdir, sizeof(initialdir));
	ksai_print("InitialWD: %s", initialdir);
	chdir(cd_to.c_str());

	char finaldir[256];
	getcwd(finaldir, sizeof(finaldir));
	ksai_print("After CD: %s", finaldir);

	auto hello = SDL_AndroidRequestPermission("WRITE_EXTERNAL_STORAGE");
	if (hello == SDL_TRUE) {
		ksai_print("WRITE EXTERNAL STORAGE PERMISSION GRANTED");
	}
	else {
		ksai_print("WRITE EXTERNAL STORAGE PERMISSION REJECTED");
	}
	char finaldirdir[256];
	ksai_print("After CD into documents Folder: %s", finaldirdir);
#endif

	int ThreadsCount = 7;
	int NoOfCmdBufferPerThread = 2;
	std::vector<ui> CmdBufferCountPerThreadVec;
	CmdBufferCountPerThreadVec.resize(ThreadsCount, NoOfCmdBufferPerThread);

	auto i = Instance(stoi(string(cf["-des_size"])), stoi(string(cf["-des_pool_size"])));
	auto w = WindowMulT(i, cf["-title"], stoi(string(cf["-height"])), stoi(string(cf["-width"])), ThreadsCount, CmdBufferCountPerThreadVec, i.GetThreadPool());
	auto em = EventManager();
	auto rr = ResourceManager(cf["-cache_folder"]);

	SpirvHelper::Init();
	rr.SetThreadPool(i.GetThreadPool());
	auto VarDesCount = stoi(string(cf["-var_des_size"]));
	if (toBool(cf["-store"])) {
		rr.Store(i);
	}
	else {
		rr.Load(i, VarDesCount);
	}
	auto td = _2d(i, w, rr, VarDesCount);
	auto ALT = Jkr::Renderer::BestText_Alt(td.sh, td.bt);
	auto FrameCapturer = Jkr::Misc::FrameCapturer(w);

	auto bind_and_get_LuaState = [&]() -> sol::state {
		sol::state lua_state;
		lua_state.open_libraries();
		lua_state.set_exception_handler(&my_exception_handler);
		CreateGLMBindings(lua_state);
		BindMathFunctions(lua_state);
		CreateSDLEventBindings(lua_state);
		Create2DBindings(i, w, lua_state, em, td, ALT, FrameCapturer);
		Create3DBindings(i, w, lua_state);
		return lua_state;
		};

	vector<sol::state> states;
	states.resize(ThreadsCount);
	{
		int i = 0;
		for (auto& u : states) {
			u = bind_and_get_LuaState();
			tracy::LuaRegister(u);

			if (toBool(cf["-debug"]))
			{
				sol::protected_function_result result = u.safe_script_file(string(cf["-main_file"]), &sol::script_pass_on_error);
				if (not result.valid() && i == 0) {
					sol::error error = result;
					ksai_print(error.what());
					cout << error.what() << endl;
					exit(EXIT_FAILURE);
				}
			}
			else {
				sol::function_result result = u.script_file(string(cf["-main_file"]));
			}
			i++;
		}
	}

	auto FillCallbackFunctions = [&](string_view Suffix, sol::state& luaState) {
		vector<string_view> views = {
			"Load",
			"Event",
			"Update",
			"Dispatch",
			"Draw",
		};

		std::vector<sol::protected_function> callbacks;
		for (auto& u : views) {
			callbacks.push_back(luaState[string(u) + string(Suffix)]);
		}
		return callbacks;
		};

	sol::state& main_state = states[0];
	sol::state& background_state = states[1];
	sol::state& ui_state = states[2];

	// All Legacy Callbacks are UI-ONLY callbacks Now.
	auto UICallbacks = FillCallbackFunctions("", ui_state);
	auto BackgroundCallbacks = FillCallbackFunctions("Background", background_state);

	auto SafeCall = [&](sol::protected_function& infunc) {
		auto result = infunc();
		if (not result.valid()) {
			sol::error err = result;
			std::string what = err.what();
			ksai_print(what.c_str());
			std::cout << what << std::endl;
			exit(EXIT_FAILURE);
		}
		};

	if (toBool(cf["-debug"]))
	{
		em.SetEventCallBack([&](void*) { SafeCall(UICallbacks[CallbackType::Event]); });
		w.SetDrawCallBack([&](void* data) { SafeCall(UICallbacks[CallbackType::Draw]); });
		w.SetUpdateCallBack([&](void* data) { SafeCall(UICallbacks[CallbackType::Update]); });
		w.SetComputeDispatchCallBack([&](void* data) { SafeCall(UICallbacks[CallbackType::Dispatch]); td.ln.Dispatch(w); td.sh.Dispatch(w); td.bt.Dispatch(w); });
		w.SetBackgroundCallBack([&](void*) {ZoneScoped; SafeCall(BackgroundCallbacks[CallbackType::Draw]); });
		w.SetResizeCallBack([&](void*) {FrameCapturer.Refresh(); });
		w.SetPostRenderingCallBack([&](void*) { FrameCapturer.Dispatch(); });
	}
	else {
		em.SetEventCallBack([&](void*) { UICallbacks[CallbackType::Event](); });
		w.SetDrawCallBack([&](void* data) { UICallbacks[CallbackType::Draw](); });
		w.SetUpdateCallBack([&](void* data) { UICallbacks[CallbackType::Update](); });
		w.SetComputeDispatchCallBack([&](void* data) { UICallbacks[CallbackType::Dispatch](); td.ln.Dispatch(w); td.sh.Dispatch(w); td.bt.Dispatch(w); });
		w.SetBackgroundCallBack([&](void*) {ZoneScoped; BackgroundCallbacks[CallbackType::Draw](); });
		w.SetResizeCallBack([&](void*) {FrameCapturer.Refresh(); });
		w.SetPostRenderingCallBack([&](void*) { FrameCapturer.Dispatch(); });
	}

	SafeCall(UICallbacks[CallbackType::Load]);
	SafeCall(BackgroundCallbacks[CallbackType::Load]);
	SpirvHelper::Finalize();

	array<float, 4> bg = { toFloat(cf["-bgr"]), toFloat(cf["-bgg"]), toFloat(cf["-bgb"]), toFloat(cf["-bga"]) };
	while (not em.ShouldQuit()) {
		em.ProcessEvents();
		w.Draw(bg[0], bg[1], bg[2], bg[3]);
		FrameMark;
	}
	return 0;
}

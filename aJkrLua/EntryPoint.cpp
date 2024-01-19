#ifdef ANDROID
#undef SDL_MAIN_HANDLED
#include <SDL.h>
#include <unistd.h>
#endif

#include "EntryPoint.hpp"
#include "WindowMulT.hpp"
#include "ksai_filestream.hpp"
#include <algorithm>
#include <filesystem>

int main(int ArgCount, char** ArgStrings)
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
    } else {
        rr.Load(i, VarDesCount);
    }
    auto td = _2d(i, w, rr, VarDesCount);
    auto ALT = Jkr::Renderer::BestText_Alt(td.sh, td.bt);

    auto bind_and_get_LuaState = [&]() -> sol::state {
        sol::state lua_state;
        lua_state.open_libraries();
        lua_state.set_exception_handler(&my_exception_handler);
        CreateGLMBindings(lua_state);
        BindMathFunctions(lua_state);
        CreateSDLEventBindings(lua_state);
        Create2DBindings(i, w, lua_state, em, td, ALT);
        Create3DBindings(i, w, lua_state);
        return move(lua_state);
    };

    vector<sol::state> states;
    states.resize(ThreadsCount);
    for (auto& u : states) {
        u = bind_and_get_LuaState();
        sol::protected_function_result result = u.safe_script_file(string(cf["-main_file"]), &sol::script_pass_on_error);
        if (not result.valid()) {
            sol::error error = result;
            ksai_print(error.what());
            cout << error.what() << endl;
            exit(EXIT_FAILURE);
        }
    }

    auto FillCallbackFunctions = [&](string_view Suffix, sol::state& luaState) {
        vector<string_view> views = {
            "Load",
            "Event",
            "Update",
            "Dispatch",
            "Draw"
        };

        std::vector<sol::protected_function> callbacks;
        for (auto& u : views) {
            callbacks.push_back(luaState[string(u) + string(Suffix)]);
        }
        return move(callbacks);
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

    em.SetEventCallBack([&](void*) { SafeCall(UICallbacks[CallbackType::Event]); });
    w.SetDrawCallBack([&](void* data) { SafeCall(UICallbacks[CallbackType::Draw]); });
    w.SetUpdateCallBack([&](void* data) { SafeCall(UICallbacks[CallbackType::Update]); });
    w.SetComputeDispatchCallBack([&](void* data) { SafeCall(UICallbacks[CallbackType::Dispatch]); td.ln.Dispatch(w); td.sh.Dispatch(w); td.bt.Dispatch(w); });
    w.SetBackgroundCallBack([&](void*) { SafeCall(BackgroundCallbacks[CallbackType::Draw]); });

    SafeCall(UICallbacks[CallbackType::Load]);
    SafeCall(BackgroundCallbacks[CallbackType::Load]);

    SpirvHelper::Finalize();

    array<float, 4> bg = { toFloat(cf["-bgr"]), toFloat(cf["-bgg"]), toFloat(cf["-bgb"]), toFloat(cf["-bga"]) };
    while (not em.ShouldQuit()) {
        em.ProcessEvents();
        w.Draw(bg[0], bg[1], bg[2], bg[3]);
    }
    return 0;
}

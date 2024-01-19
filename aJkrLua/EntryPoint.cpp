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

// int main (int ArgCount , char **ArgStrings)
//{
//     SDL_Init(SDL_INIT_VIDEO);
//     SDL_Window* Window = SDL_CreateWindow("Darshan", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 100, 100, SDL_WINDOW_VULKAN);
//     while (true) {
//
//     }
//     SDL_Quit();
//     return 0;
// }

int main(int ArgCount, char** ArgStrings)
{
    ksai_print("Started");
    auto& cf = gConfiguration;
    const vector<string_view> Arguments(ArgStrings + 1, ArgStrings + ArgCount);
    FillConfig(Arguments);

    ksai_print("Starting Instance");
    auto i = Instance(stoi(string(cf["-des_size"])), stoi(string(cf["-des_pool_size"])));
    ksai_print("Finished Instance");

    std::vector<ui> inNoOfCmdBufferObjectsPerThread = { 2, 2, 2, 2, 2 };
    ksai_print("Starting Window");
    auto w = WindowMulT(i, cf["-title"], stoi(string(cf["-height"])), stoi(string(cf["-width"])), 5, inNoOfCmdBufferObjectsPerThread);
    ksai_print("Finished Window");
    ksai_print("Event Manager");
    auto em = EventManager();
    auto rr = ResourceManager(cf["-cache_folder"]);
    auto VarDesCount = stoi(string(cf["-var_des_size"]));
    SpirvHelper::Init();
    rr.SetThreadPool(i.GetThreadPool());
    ksai_print("Starting Load/Store");
    if (toBool(cf["-store"])) {
        ksai_print("Starting Store");
        rr.Store(i);
        ksai_print("Finished Store");
    } else {
        ksai_print("Starting Load");
        rr.Load(i, VarDesCount);
        ksai_print("Finished Load");
    }
    ksai_print("Finished Load/Store");
    auto td = _2d(i, w, rr, VarDesCount);
    auto ALT = Jkr::Renderer::BestText_Alt(td.sh, td.bt);

    sol::state l;
    l.open_libraries();
    l.set_exception_handler(&my_exception_handler);
    CreateGLMBindings(l);
    BindMathFunctions(l);
    CreateSDLEventBindings(l);
    Create2DBindings(i, w, l, em, td, ALT);
    Create3DBindings(i, w, l);

    ksai_print("Protected Function Result");
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

    ksai_print((string(cf["-main_file"]).c_str()));
    ksai_print("Protected Function Result");

    sol::protected_function_result resultlksdjaf = l.safe_script("require\"jkrgui.jkrgui\"", &sol::script_pass_on_error);
    if (not resultlksdjaf.valid()) {
        sol::error error = resultlksdjaf;
        ksai_print(error.what());
        exit(-1);
    }
    ksai_print("require mainh executed");

    sol::protected_function_result result = l.safe_script_file(string(cf["-main_file"]), &sol::script_pass_on_error);
    ksai_print("Protected Function Result Finished");

    if (not result.valid()) {
        sol::error error = result;
        ksai_print(error.what());
        cout << error.what() << endl;
        exit(-1);
    }

    sol::protected_function draw_callback = l["Draw"];
    sol::protected_function load_callback = l["Load"];
    sol::protected_function event_callback = l["Event"];
    sol::protected_function disptach_callback = l["Dispatch"];
    sol::protected_function update_callback = l["Update"];

    std::mutex mut;
    std::condition_variable var;
    bool is_locked = false;

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

    auto Event = [&](void*) { SafeCall(event_callback); };
    em.SetEventCallBack(Event);
    auto Draw = [&](void* data) {
        SafeCall(draw_callback);
    };
    w.SetDrawCallBack(Draw);
    auto Update = [&](void* data) {
        SafeCall(update_callback);
    };
    w.SetUpdateCallBack(Update);
    auto Dispatch = [&](void* data) {
        SafeCall(disptach_callback);

        td.ln.Dispatch(w);
        td.sh.Dispatch(w);
        td.bt.Dispatch(w);
    };
    w.SetComputeDispatchCallBack(Dispatch);
    SafeCall(load_callback);
    //array<float, 4> bg = { toFloat(cf["-bgr"]), toFloat(cf["-bgg"]), toFloat(cf["-bgb"]), toFloat(cf["-bga"]) };
    array<float, 4> bg = {1, 1, 1, 1};

    SpirvHelper::Finalize();
    ksai_print("Draw Start");
    while (!em.ShouldQuit()) {
        //em.ProcessEvents();
        w.Draw(bg[0] * sin(w.GetWindowCurrentTime()), bg[1], bg[2], bg[3]);
	   ksai_print("Drawing");
    }
    ksai_print("Draw Start Finished");
}

#pragma once
#define SDL_MAIN_HANDLED
#include "Instance.hpp"
#include <any>
#include <functional>

namespace Jkr {
class SDLWindow {
public:
    std::pair<int, int> GetWindowSize() const;
    GETTER GetWindowHandle() const { return mSDLWindowPtr; }
    SETTER SetWindowShouldClose(bool inValue) { mWindowShouldClose = inValue; }
    SETTER SetResizeCallBack(const std::function<void(void*)>& inFunction) { mResizeFunction = inFunction; }
    SETTER SetSize(int inWidth, int inHeight);
    SETTER SetTitle(std::string inString) const;
    SETTER SetWindowBorderless();

    SDLWindow(std::string_view inName, int inHeight, int inWidth);
    ~SDLWindow();

    void ToggleWindowFullScreen();
    void Minimize();
    static auto GetWindowCurrentTime() { return SDL_GetTicks(); }

protected:
    void* mData = nullptr;
    std::function<void(void*)> mResizeFunction = [](void*) {};
    bool mWindowShouldClose = false;
    bool mWindowIsFullScreen = false;
    std::string mName;
    int mHeight;
    int mWidth;
    SDL_Window* mSDLWindowPtr;
};
}

#pragma once

#ifndef ANDROID
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "Instance.hpp"
#include <SDL2/SDL_vulkan.h>
#include <functional>

namespace Jkr {
class SDLWindow {
    public:
    std::pair<int, int> GetWindowSize() const;
    glm::vec2 GetWindowDimension() const;
    glm::vec2 GetVulkanDrawableSize() const;
    GETTER GetWindowHandle() const { return mSDLWindowPtr; }
    SETTER SetWindowShouldClose(bool inValue) { mWindowShouldClose = inValue; }
    SETTER SetResizeCallBack(const std::function<void(void*)>& inFunction) {
        mResizeFunction = inFunction;
    }
    void SetSize(int inWidth, int inHeight);
    void SetTitle(std::string_view inString) const;
    void SetWindowBorderless();

    void ToggleWindowFullScreen();
    void Minimize();
    static auto GetWindowCurrentTime() { return SDL_GetTicks(); }

    SDLWindow(std::string_view inName, int inHeight, int inWidth);
    ~SDLWindow();

    protected:
    void* mData                                = nullptr;
    std::function<void(void*)> mResizeFunction = [](void*) {};
    bool mWindowShouldClose                    = false;
    bool mWindowIsFullScreen                   = false;
    std::string mName;
    int mHeight;
    int mWidth;
    SDL_Window* mSDLWindowPtr;
};
} // namespace Jkr

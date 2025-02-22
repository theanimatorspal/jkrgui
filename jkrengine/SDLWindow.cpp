#include "SDLWindow.hpp"
#include <SDL2/SDL_vulkan.h>

/*
from dele256/sdl_borderless_resize.cpp (Github)
*/

#define MOUSE_GRAB_PADDING 5

SDL_HitTestResult HitTestCallback(SDL_Window *Window, const SDL_Point *Area, void *Data) {
    int Width, Height;
    SDL_GetWindowSize(Window, &Width, &Height);

    if (Area->y < MOUSE_GRAB_PADDING) {
        if (Area->x < MOUSE_GRAB_PADDING) {
            return SDL_HITTEST_RESIZE_TOPLEFT;
        } else if (Area->x > Width - MOUSE_GRAB_PADDING) {
            return SDL_HITTEST_RESIZE_TOPRIGHT;
        } else {
            return SDL_HITTEST_RESIZE_TOP;
        }
    } else if (Area->y > Height - MOUSE_GRAB_PADDING) {
        if (Area->x < MOUSE_GRAB_PADDING) {
            return SDL_HITTEST_RESIZE_BOTTOMLEFT;
        } else if (Area->x > Width - MOUSE_GRAB_PADDING) {
            return SDL_HITTEST_RESIZE_BOTTOMRIGHT;
        } else {
            return SDL_HITTEST_RESIZE_BOTTOM;
        }
    } else if (Area->x < MOUSE_GRAB_PADDING) {
        return SDL_HITTEST_RESIZE_LEFT;
    } else if (Area->x > Width - MOUSE_GRAB_PADDING) {
        return SDL_HITTEST_RESIZE_RIGHT;
    } else if (Area->y < 25 && Area->x < 10) {
        return SDL_HITTEST_DRAGGABLE;
    }

    return SDL_HITTEST_NORMAL; // SDL_HITTEST_NORMAL <- Windows behaviour
}

void Jkr::SDLWindow::SetWindowBorderless() {
    SDL_SetWindowBordered(mSDLWindowPtr, SDL_FALSE);
    SDL_SetWindowHitTest(mSDLWindowPtr, HitTestCallback, 0);
}

Jkr::SDLWindow::SDLWindow(std::string_view inName, int inHeight, int inWidth) {
    Init({inName, inHeight, inWidth});
}

Jkr::SDLWindow::~SDLWindow() {
    if (mInitialized) {
        Destroy();
    }
}

glm::vec2 Jkr::SDLWindow::GetWindowDimension() const {
    int w, h;
    SDL_Vulkan_GetDrawableSize(mSDLWindowPtr, &w, &h);
    return glm::vec2(w, h);
}

void Jkr::SDLWindow::SetSize(int inWidth, int inHeight) {
    // TODO
}

void Jkr::SDLWindow::SetTitle(std::string_view inString) const {
    SDL_SetWindowTitle(mSDLWindowPtr, inString.data());
}

std::pair<int, int> Jkr::SDLWindow::GetWindowSize() const {
    int w, h;
    SDL_GetWindowSize(mSDLWindowPtr, &w, &h);
    return std::make_pair(w, h);
}

void Jkr::SDLWindow::ToggleWindowFullScreen() {
    if (!mWindowIsFullScreen) {
        SDL_SetWindowFullscreen(mSDLWindowPtr, SDL_WINDOW_FULLSCREEN_DESKTOP);
        mWindowIsFullScreen = true;
    } else {
        SDL_SetWindowFullscreen(mSDLWindowPtr, 0);
        SDL_SetWindowSize(mSDLWindowPtr, mWidth, mHeight);
        mWindowIsFullScreen = false;
    }
}

void Jkr::SDLWindow::Minimize() {
    SDL_SetWindowFullscreen(mSDLWindowPtr, 0);
    SDL_SetWindowSize(mSDLWindowPtr, mWidth, mHeight);
    SDL_MinimizeWindow(mSDLWindowPtr);
    mWindowIsFullScreen = false;
}

glm::vec2 Jkr::SDLWindow::GetVulkanDrawableSize() const {
    int x, y;
    SDL_Vulkan_GetDrawableSize(mSDLWindowPtr, &x, &y);
    float ddpi;
    float hdpi;
    float vdpi;
    SDL_GetDisplayDPI(0, &ddpi, &hdpi, &vdpi);
    return glm::vec2(x, y);
}

using namespace Jkr;
void SDLWindow::Init(CreateInfo info) {
    mName   = info.inName;
    mHeight = info.inHeight;
    mWidth  = info.inWidth;
#ifndef ANDROID
    mSDLWindowPtr = SDL_CreateWindow(mName.c_str(),
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED,
                                     mWidth,
                                     mHeight,
                                     SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
#else
    SDL_DisplayMode mode;
    SDL_GetCurrentDisplayMode(0, &mode);
    mSDLWindowPtr = SDL_CreateWindow(mName.c_str(),
                                     SDL_WINDOWPOS_UNDEFINED,
                                     SDL_WINDOWPOS_UNDEFINED,
                                     mode.w,
                                     mode.h,
                                     SDL_WINDOW_VULKAN | SDL_WINDOW_BORDERLESS);
#endif
    mInitialized = true;
}

void SDLWindow::Destroy() {
    if (mSDLWindowPtr) {
        SDL_DestroyWindow(mSDLWindowPtr);
    }
    mInitialized = false;
}

void SDLWindow::Show() { SDL_ShowWindow(mSDLWindowPtr); }

void SDLWindow::Hide() { SDL_HideWindow(mSDLWindowPtr); }
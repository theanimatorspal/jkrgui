#pragma once

#ifndef ANDROID
#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#else
#include <SDL.h>
#endif

#include "Instance.hpp"
#include <SDL2/SDL_vulkan.h>
#include <any>
#include <functional>

namespace Jkr {
class SDLWindow {
      public:
          std::pair<int, int> GetWindowSize() const;
          glm::vec2 GetWindowDimension() const;
          GETTER GetWindowHandle() const { return mSDLWindowPtr; }
          SETTER SetWindowShouldClose(bool inValue) { mWindowShouldClose = inValue; }
          SETTER SetResizeCallBack(const std::function<void(void*)>& inFunction) { mResizeFunction = inFunction; }
          SETTER SetSize(int inWidth, int inHeight);
          SETTER SetTitle(std::string_view inString) const;
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
} // namespace Jkr

inline glm::vec2 Jkr::SDLWindow::GetWindowDimension() const {
          int w, h;
          SDL_Vulkan_GetDrawableSize(mSDLWindowPtr, &w, &h);
          return glm::vec2(w, h);
}

inline void Jkr::SDLWindow::SetSize(int inWidth, int inHeight) {
          // TODO
}

inline void Jkr::SDLWindow::SetTitle(std::string_view inString) const { SDL_SetWindowTitle(mSDLWindowPtr, inString.data()); }

inline std::pair<int, int> Jkr::SDLWindow::GetWindowSize() const {
          int w, h;
          SDL_Vulkan_GetDrawableSize(mSDLWindowPtr, &w, &h);
          return std::make_pair(w, h);
}

inline void Jkr::SDLWindow::ToggleWindowFullScreen() {
          if (!mWindowIsFullScreen) {
                    SDL_SetWindowFullscreen(mSDLWindowPtr, SDL_WINDOW_FULLSCREEN_DESKTOP);
                    mWindowIsFullScreen = true;
          } else {
                    SDL_SetWindowFullscreen(mSDLWindowPtr, 0);
                    SDL_SetWindowSize(mSDLWindowPtr, mWidth, mHeight);
                    mWindowIsFullScreen = false;
          }
}

inline void Jkr::SDLWindow::Minimize() {
          SDL_SetWindowFullscreen(mSDLWindowPtr, 0);
          SDL_SetWindowSize(mSDLWindowPtr, mWidth, mHeight);
          SDL_MinimizeWindow(mSDLWindowPtr);
          mWindowIsFullScreen = false;
}

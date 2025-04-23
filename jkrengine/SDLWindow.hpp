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
                    struct CreateInfo {
                                        sv inName;
                                        int inHeight;
                                        int inWidth;
                    };

                    SDLWindow() = default;
                    ~SDLWindow();
                    SDLWindow(const SDLWindow &other)            = delete;
                    SDLWindow &operator=(const SDLWindow &other) = delete;
                    SDLWindow(SDLWindow &&other)                 = default;
                    SDLWindow &operator=(SDLWindow &&other)      = default;
                    operator SDL_Window *() const { return mSDLWindowPtr; }

                    void Init(CreateInfo inCreateInfo);
                    void Destroy();

                    std::pair<int, int> GetWindowSize() const;
                    glm::vec2 GetWindowDimension() const;
                    glm::vec2 GetVulkanDrawableSize() const;
                    GETTER GetWindowHandle() const { return mSDLWindowPtr; }
                    SETTER SetWindowShouldClose(bool inValue) { mWindowShouldClose = inValue; }
                    void SetSize(int inWidth, int inHeight);
                    void SetTitle(std::string_view inString) const;
                    void SetWindowBorderless();
                    void ToggleWindowFullScreen();
                    void Hide();
                    void Show();
                    void Minimize();
                    void Maximize();
                    static auto GetWindowCurrentTime() { return SDL_GetTicks(); }

                    SDLWindow(std::string_view inName, int inHeight, int inWidth);

        protected:
                    void *mData              = nullptr;
                    bool mWindowShouldClose  = false;
                    bool mWindowIsFullScreen = false;
                    std::string mName;
                    int mHeight;
                    int mWidth;
                    SDL_Window *mSDLWindowPtr = nullptr;
                    bool mInitialized         = false;
};
} // namespace Jkr

#pragma once
#include "Window_base.hpp"

namespace Jkr {
struct BoundRect2D {
    glm::vec2 mXy;
    glm::vec2 mWh;
    constexpr bool IsPointWithin(glm::vec2 inXy) {
        return ((mXy.x < inXy.x) && (inXy.x < mXy.x + mWh.x)) &&
               ((mXy.y < inXy.y) && (inXy.y < mXy.y + mWh.y));
    }
};
class EventManager {
    public:
    EventManager() { mBoundRect2Ds.reserve(100); }
    void ProcessEvents();
    GETTER ShouldQuit() const { return should_quit; }
    GETTER GetEventHandle() const { return mEvent; }
    GETTER GetMousePos() const { return mMousePos; }
    GETTER GetRelativeMousePos() const { return mRelativePos; }
    GETTER GetMouseButtonValue() const { return mCurrentPushedMouseButton; }
    GETTER IsKeyPressedContinous(int inScanCode) const { return (bool)mKeyboardState[inScanCode]; }
    GETTER IsKeyReleased(SDL_Keycode inKey) {
        return mEvent.type == SDL_KEYUP and mEvent.key.keysym.sym == inKey;
    }
    GETTER IsKeyPressed(SDL_Keycode inKey) {
        return mEvent.type == SDL_KEYDOWN and mEvent.key.keysym.sym == inKey;
    }
    GETTER IsLeftButtonPressedContinous() const {
        return (SDL_BUTTON(SDL_BUTTON_LEFT) bitand this->GetMouseButtonValue()) != 0;
    }
    GETTER IsRightButtonPressedContinous() const {
        return (SDL_BUTTON(SDL_BUTTON_RIGHT) bitand this->GetMouseButtonValue()) != 0;
    }
    GETTER IsLeftButtonPressed() const {
        return (mEvent.type == SDL_MOUSEBUTTONDOWN) && (mEvent.button.button == SDL_BUTTON_LEFT);
    }
    GETTER IsRightButtonPressed() const {
        return (mEvent.type == SDL_MOUSEBUTTONDOWN) && (mEvent.button.button == SDL_BUTTON_RIGHT);
    }
    void StartTextInput() { SDL_StartTextInput(); }
    void StopTextInput() { SDL_StopTextInput(); }
    bool IsMouseWithinAtTopOfStack(uint32_t inId, uint32_t inDepthValue);
    bool IsMouseWithin(uint32_t inId, uint32_t inDepthValue);
    uint32_t SetBoundedRect(glm::vec2 inXy, glm::vec2 inWh, uint32_t inDepthValue);
    void UpdateBoundedRect(uint32_t inId, glm::vec2 inXy, glm::vec2 inWh, uint32_t inDepthValue);
    void SetEventCallBack(const std::function<void()>& inEventCallBack);

    private:
    SDL_Event mEvent;
    glm::ivec2 mMousePos;
    glm::ivec2 mRelativePos;
    int mCurrentPushedMouseButton;
    bool should_quit = false;
    const uint8_t* mKeyboardState;

    private:
    std::function<void()> mEventCallBack = []() {};
    std::unordered_map<uint32_t, std::vector<BoundRect2D>> mBoundRect2Ds;
};
} // namespace Jkr

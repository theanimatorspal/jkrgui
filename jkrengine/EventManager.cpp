#include "EventManager.hpp"
#include "Window.hpp"
void Jkr::EventManager::ProcessEventsEXT(Window &inWindow) {
    mWindowSize            = inWindow.GetWindowDimension();
    mFrameSize             = inWindow.GetOffscreenFrameDimension();
    mOffscreenByWindowSize = glm::vec2(mFrameSize.x / mWindowSize.x, mFrameSize.y / mWindowSize.y);
    mCloseWindowEvent      = false;

    while (SDL_PollEvent(&mEvent)) {
        if (mEvent.type == SDL_WINDOWEVENT and mEvent.window.event == SDL_WINDOWEVENT_CLOSE) {
            mCloseWindowEvent = true;
        }

        for (auto &callback : mEventCallBack) {
            callback();
        }

        mCurrentPushedMouseButton = SDL_GetMouseState(&mMousePos.x, &mMousePos.y);
        switch (mEvent.type) {
            case SDL_QUIT:
                should_quit = true;
            default:
                break;
        }
        SDL_GetRelativeMouseState(&mRelativePos.x, &mRelativePos.y);
    }
    int NumKeys;
    mKeyboardState = SDL_GetKeyboardState(&NumKeys);
}

void Jkr::EventManager::ProcessEvents() {
    mCloseWindowEvent = false;

    while (SDL_PollEvent(&mEvent)) {

        if (mEvent.type == SDL_WINDOWEVENT and mEvent.window.event == SDL_WINDOWEVENT_CLOSE) {
            mCloseWindowEvent = true;
        }

        for (auto &callback : mEventCallBack) {
            callback();
        }

        mCurrentPushedMouseButton = SDL_GetMouseState(&mMousePos.x, &mMousePos.y);
        switch (mEvent.type) {
            case SDL_QUIT:
                should_quit = true;
            default:
                break;
        }
        SDL_GetRelativeMouseState(&mRelativePos.x, &mRelativePos.y);
    }
    int NumKeys;
    mKeyboardState = SDL_GetKeyboardState(&NumKeys);
}

bool Jkr::EventManager::IsMouseWithinAtTopOfStack(uint32_t inId, uint32_t inDepthValue) {
    for (auto &mapElement : mBoundRect2Ds) {
        if (mapElement.first < inDepthValue) {
            for (auto &vecElement : mBoundRect2Ds[mapElement.first]) {
                if (vecElement.IsPointWithin(mMousePos, mOffscreenByWindowSize)) return false;
            }
        }
    }
    return (mBoundRect2Ds.contains(inDepthValue)) &&
           mBoundRect2Ds[inDepthValue][inId].IsPointWithin(mMousePos, mOffscreenByWindowSize);
}
bool Jkr::EventManager::IsMouseWithin(uint32_t inId, uint32_t inDepthValue) {
    return mBoundRect2Ds[inDepthValue][inId].IsPointWithin(mMousePos, mOffscreenByWindowSize);
}
uint32_t Jkr::EventManager::SetBoundedRect(glm::vec2 inXy, glm::vec2 inWh, uint32_t inDepthValue) {
    if (not mBoundRect2Ds.contains(inDepthValue)) {
        mBoundRect2Ds[inDepthValue] = std::vector<BoundRect2D>();
    }
    uint32_t ret_val = mBoundRect2Ds[inDepthValue].size();
    mBoundRect2Ds[inDepthValue].push_back(BoundRect2D{.mXy = inXy, .mWh = inWh});
    return ret_val;
}

void Jkr::EventManager::UpdateBoundedRect(uint32_t inId,
                                          glm::vec2 inXy,
                                          glm::vec2 inWh,
                                          uint32_t inDepthValue) {
    auto br                           = BoundRect2D{.mXy = inXy, .mWh = inWh};
    mBoundRect2Ds[inDepthValue][inId] = br;
}
void Jkr::EventManager::SetEventCallBack(const std::function<void()> &inEventCallBack) {
    mEventCallBack.push_back(inEventCallBack);
}

bool Jkr::EventManager::ShouldQuit() const { return should_quit; }
SDL_Event Jkr::EventManager::GetEventHandle() const { return mEvent; }
glm::ivec2 Jkr::EventManager::GetMousePos() const { return mMousePos; }
glm::ivec2 Jkr::EventManager::GetRelativeMousePos() const { return mRelativePos; }
int Jkr::EventManager::GetMouseButtonValue() const { return mCurrentPushedMouseButton; }

bool Jkr::EventManager::IsKeyPressedContinous(int inScanCode) const {
    return (bool)mKeyboardState[inScanCode];
}
bool Jkr::EventManager::IsKeyReleased(SDL_Keycode inKey) {
    return mEvent.type == SDL_KEYUP and mEvent.key.keysym.sym == inKey;
}
bool Jkr::EventManager::IsKeyPressed(SDL_Keycode inKey) {
    return mEvent.type == SDL_KEYDOWN and mEvent.key.keysym.sym == inKey;
}
bool Jkr::EventManager::IsLeftButtonPressedContinous() const {
    return (SDL_BUTTON(SDL_BUTTON_LEFT) bitand this->GetMouseButtonValue()) != 0;
}
bool Jkr::EventManager::IsRightButtonPressedContinous() const {
    return (SDL_BUTTON(SDL_BUTTON_RIGHT) bitand this->GetMouseButtonValue()) != 0;
}
bool Jkr::EventManager::IsLeftButtonPressed() const {
    return (mEvent.type == SDL_MOUSEBUTTONDOWN) && (mEvent.button.button == SDL_BUTTON_LEFT);
}
bool Jkr::EventManager::IsRightButtonPressed() const {
    return (mEvent.type == SDL_MOUSEBUTTONDOWN) && (mEvent.button.button == SDL_BUTTON_RIGHT);
}
bool Jkr::EventManager::IsCloseWindowEvent() const { return mCloseWindowEvent; }
bool Jkr::EventManager::IsFingerDownEvent() const { return mEvent.type == SDL_FINGERDOWN; }
bool Jkr::EventManager::IsFingerUpEvent() const { return mEvent.type == SDL_FINGERUP; }

glm::vec3 Jkr::EventManager::GetTouchLocation() const {
    return glm::vec3(mEvent.tfinger.x * mWindowSize.x * mOffscreenByWindowSize.x,
                     mEvent.tfinger.y * mWindowSize.y * mOffscreenByWindowSize.y,
                     mEvent.tfinger.pressure);
}
glm::vec3 Jkr::EventManager::GetTouchRelativeLocation() const {
    return glm::vec3(mEvent.tfinger.dx * mWindowSize.x * mOffscreenByWindowSize.x,
                     mEvent.tfinger.dy * mWindowSize.y * mOffscreenByWindowSize.y,
                     mEvent.tfinger.pressure);
}

bool Jkr::EventManager::IsFingerMotionEvent() const { return mEvent.type == SDL_FINGERMOTION; }

uint32_t Jkr::EventManager::GetTouchId() const { return mEvent.tfinger.touchId; }

bool Jkr::EventManager::IsMultiGestureEvent() const { return mEvent.type == SDL_MULTIGESTURE; }

glm::vec4 Jkr::EventManager::GetMultiGesture() const {
    return glm::vec4(
         mEvent.mgesture.x, mEvent.mgesture.y, mEvent.mgesture.dDist, mEvent.mgesture.dTheta);
}
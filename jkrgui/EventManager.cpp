#include "EventManager.hpp"
#include "Window.hpp"
void Jkr::EventManager::ProcessEventsEXT(Window &inWindow) {
    mWindowSize            = inWindow.GetWindowDimension();
    mFrameSize             = inWindow.GetOffscreenFrameSize();
    mOffscreenByWindowSize = glm::vec2(mFrameSize.x / mWindowSize.x, mFrameSize.y / mWindowSize.y);
    while (SDL_PollEvent(&mEvent)) {
        mEventCallBack();
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
    while (SDL_PollEvent(&mEvent)) {
        mEventCallBack();
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
    mEventCallBack = inEventCallBack;
}
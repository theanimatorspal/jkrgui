#include "EventManager.hpp"
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
    for (auto& mapElement : mBoundRect2Ds) {
        if (mapElement.first < inDepthValue) {
            for (auto& vecElement : mBoundRect2Ds[mapElement.first]) {
                if (vecElement.IsPointWithin(mMousePos)) return false;
            }
        }
    }
    return (mBoundRect2Ds.contains(inDepthValue)) &&
           mBoundRect2Ds[inDepthValue][inId].IsPointWithin(mMousePos);
}
bool Jkr::EventManager::IsMouseWithin(uint32_t inId, uint32_t inDepthValue) {
    return mBoundRect2Ds[inDepthValue][inId].IsPointWithin(mMousePos);
}
uint32_t Jkr::EventManager::SetBoundedRect(glm::vec2 inXy, glm::vec2 inWh, uint32_t inDepthValue) {
    if (not mBoundRect2Ds.contains(inDepthValue)) {
        mBoundRect2Ds[inDepthValue] = std::vector<BoundRect2D>();
    }
    uint32_t ret_val = mBoundRect2Ds[inDepthValue].size();
    mBoundRect2Ds[inDepthValue].push_back(BoundRect2D{.mXy = inXy, .mWh = inWh});
    return ret_val;
}

void Jkr::EventManager::UpdateBoundRect(uint32_t inId,
                                        glm::uvec2 inXy,
                                        glm::uvec2 inWh,
                                        uint32_t inDepthValue) {
    auto br                           = BoundRect2D{.mXy = inXy, .mWh = inWh};
    mBoundRect2Ds[inDepthValue][inId] = br;
}
void Jkr::EventManager::SetEventCallBack(const std::function<void()>& inEventCallBack) {
    mEventCallBack = inEventCallBack;
}
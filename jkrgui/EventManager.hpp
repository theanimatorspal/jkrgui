#pragma once
#include "Window.hpp"

namespace Jkr {
struct BoundRect2D {
    glm::uvec2 mXy;
    glm::uvec2 mWh;
    constexpr bool IsPointWithin(glm::uvec2 inXy)
    {
        return ((mXy.x < inXy.x) && (inXy.x < mXy.x + mWh.x))
            && ((mXy.y < inXy.y) && (inXy.y < mXy.y + mWh.y));
    }
};
class EventManager {
public:
    EventManager()
    {
        mBoundRect2Ds.reserve(100);
    }
    void ProcessEvents()
    {
        while (SDL_PollEvent(&mEvent)) {
            mEventCallBack(nullptr);
            switch (mEvent.type) {
            case SDL_QUIT:
                should_quit = true;
            default:
                break;
            }
            SDL_GetRelativeMouseState(&mRelativePos.x, &mRelativePos.y);
        }
        mCurrentPushedMouseButton = SDL_GetMouseState(&mMousePos.x, &mMousePos.y);
    }

    GETTER GetDepthValue() { return mCurrentDepthValue; }
    GETTER MoveDepthValueTowardsTheCamera() { mCurrentDepthValue--; }
    GETTER MoveDepthValueAwayFromTheCamera() { mCurrentDepthValue++; }
    GETTER ShouldQuit() const { return should_quit; }
    GETTER GetEventHandle() const { return mEvent; }
    GETTER GetMousePos() const { return mMousePos; }
    GETTER GetRelativeMousePos() const { return mRelativePos; }
    GETTER GetMouseButtonValue() const { return mCurrentPushedMouseButton; }
    GETTER IsLeftButtonPressed() const
    {
        return (SDL_BUTTON(SDL_BUTTON_LEFT) bitand this->GetMouseButtonValue()) != 0;
    }
    GETTER IsRightButtonPressed() const
    {
        return (SDL_BUTTON(SDL_BUTTON_RIGHT) bitand this->GetMouseButtonValue()) != 0;
    }
    void StartTextInput()
    {
        SDL_StartTextInput();
    }
    void StopTextInput()
    {
        SDL_StopTextInput();
    }
    bool IsMouseWithinAtTopOfStack(uint32_t inId, uint32_t inDepthValue)
    {
        for (auto& mapElement : mBoundRect2Ds) {
            if (mapElement.first < inDepthValue) {
                for (auto& vecElement : mBoundRect2Ds[mapElement.first]) {
                    if (vecElement.IsPointWithin(mMousePos))
                        return false;
                }
            }
        }
        return (mBoundRect2Ds.contains(inDepthValue)) && mBoundRect2Ds[inDepthValue][inId].IsPointWithin(mMousePos);
    }

    bool IsMouseWithin(uint32_t inId, uint32_t inDepthValue)
    {
        return mBoundRect2Ds[inDepthValue][inId].IsPointWithin(mMousePos);
    }
    [[nodiscard]] uint32_t SetBoundedRect(glm::uvec2 inXy, glm::uvec2 inWh, uint32_t inDepthValue)
    {
        if (!mBoundRect2Ds.contains(inDepthValue)) {
            mBoundRect2Ds[inDepthValue] = std::vector<BoundRect2D>();
        }

        uint32_t ret_val = mBoundRect2Ds[inDepthValue].size();
        mBoundRect2Ds[inDepthValue].push_back(BoundRect2D { .mXy = inXy, .mWh = inWh });
        return ret_val;
    }

    void UpdateBoundRect(uint32_t inDepthValue, uint32_t inId, BoundRect2D& inBoundRect)
    {
        mBoundRect2Ds[inDepthValue][inId] = inBoundRect;
    }

    void UpdateBoundRect(uint32_t inId, glm::uvec2 inXy, glm::uvec2 inWh, uint32_t inDepthValue)
    {
        auto br = BoundRect2D { .mXy = inXy, .mWh = inWh };
        mBoundRect2Ds[inDepthValue][inId] = br;
    }

    void SetEventCallBack(const std::function<void(void*)>& inEventCallBack)
    {
        mEventCallBack = inEventCallBack;
    }

private:
    SDL_Event mEvent;
    glm::ivec2 mMousePos;
    glm::ivec2 mRelativePos;
    int mCurrentPushedMouseButton;
    uint32_t mCurrentDepthValue = 80;
    bool should_quit = false;

private:
    std::function<void(void*)> mEventCallBack = [](void*) {};
    std::unordered_map<uint32_t, std::vector<BoundRect2D>> mBoundRect2Ds;
};
}

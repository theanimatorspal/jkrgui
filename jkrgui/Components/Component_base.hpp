#pragma once
#include <EventManager.hpp>
#include <Renderers/2d.hpp>
#include <Window.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define SETTER inline void

namespace Jkr::Component {
using namespace Jkr::Renderer;
class Component_base {
public:
    Component_base(const Component_base&) = default;
    Component_base(Component_base&&) = default;
    Component_base& operator=(const Component_base&) = default;
    Component_base& operator=(Component_base&&) = default;

public:
    GETTER& GetFastTexts() { return mFastTexts; }
    GETTER& GetLines() { return mLines; }
    GETTER& GetShapes() { return mShapes; }

private:
    std::vector<glm::uvec2> mFastTexts;
    std::vector<glm::uvec2> mLines;
    std::vector<glm::uvec2> mShapes;

public:
    Component_base(_2d& inR, EventManager& inE)
        : r(inR)
        , e(inE)
    {
        mPosition = { INT_MAX, INT_MAX };
        mDimension = { INT_MAX, INT_MAX };
        mDepthValue = INT_MAX;
        mWindow = nullptr;
        mWh = INT_MAX;
        mWw = INT_MAX;
    }
    void CheckExpectations()
    {
        assert(mPosition.x != static_cast<float>(INT_MAX));
        assert(mPosition.y != static_cast<float>(INT_MAX));
        assert(mDimension.x != static_cast<float>(INT_MAX));
        assert(mDimension.y != static_cast<float>(INT_MAX));
        assert(mDepthValue != static_cast<float>(INT_MAX));
        assert(mWh != INT_MAX);
        assert(mWw != INT_MAX);
        assert(mWindow != nullptr);
    }
    void SetDefaultBoundedRectangle()
    {
        mBoundedRectangleId = e.SetBoundedRect(mPosition, mDimension, mDepthValue);
    }
    void UpdateDefaultBoundedRectangle()
    {
        Jkr::BoundRect2D Rect { .mXy = mPosition, .mWh = mDimension };
        e.UpdateBoundRect(mDepthValue, mBoundedRectangleId, Rect);
    }
    GETTER GetDimension() const { return mDimension; }
    GETTER GetPosition() const { return mPosition; }
    SETTER SetDimension(glm::vec2 inVec) { mDimension = inVec; }
    SETTER SetPosition(glm::vec2 inVec)
    {
        mPosition = inVec;
        mTransformMatrix = glm::identity<glm::mat4>();
        mTransformMatrix = glm::translate(mTransformMatrix, glm::vec3(mPosition.x, mPosition.y, 0));
    }
    SETTER SetDepthValue(uint32_t inDepthValue) { mDepthValue = inDepthValue; }
    GETTER GetDepthValue() { return mDepthValue; }
    GETTER IsFocused() const { return isFocused; }
    GETTER IsMouseOnTop() const { return e.IsMouseWithinAtTopOfStack(mBoundedRectangleId, mDepthValue); }
    GETTER IsMouseWithin() const { return e.IsMouseWithin(mBoundedRectangleId, mDepthValue); }
    SETTER ToggleFocus() { isFocused = !isFocused; }
    SETTER SetFocus(bool inFocus) { isFocused = inFocus; }
    GETTER GetTranslationMatrix() const
    {
        return mTransformMatrix;
    }
    glm::vec2 FromComponent(glm::vec2 inComp)
    {
        return glm::vec2(inComp) + mPosition;
    }
    glm::vec2 FromWorld(glm::vec2 inWorld)
    {
        return glm::vec2(inWorld) - mPosition;
    }
    glm::vec2 ToCentralizedCoords(glm::uvec2 mBoxDimension, glm::uvec2 mBox)
    {
    }

protected:
    _2d& r;
    EventManager& e;
    Window* mWindow = nullptr;
    uint32_t mWh = 0, mWw = 0;

public:
    SETTER SetWindow(Window* inWindow, uint32_t inW, uint32_t inH)
    {
        mWindow = inWindow;
        mWw = inW;
        mWh = inH;
    }
    SETTER SetWindowDimensions(float inW, float inH)
    {
        mWw = inW;
        mWh = inH;
    }
    GETTER GetWindowWidth() const
    {
        return mWw;
    }
    GETTER GetWindowHeight() const
    {
        return mWh;
    }
    GETTER& GetWindow()
    {
        return mWindow;
    }

private:
    glm::vec2 mDimension;
    glm::vec2 mPosition;
    glm::mat4 mTransformMatrix = glm::identity<glm::mat4>();
    uint32_t mDepthValue;
    uint32_t mBoundedRectangleId;
    bool isFocused = false;
};
}

/*

void draw();

*/

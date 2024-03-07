#pragma once
#include "../Renderer_base.hpp"
#include "Line_base.hpp"

namespace Jkr::Renderer {

class Line : public Line_base, Renderer_base {
    using rb = Renderer_base;
    using lb = Line_base;

public:
    Line(const Instance& inInstance, Window& inCompatibleWindow, PainterCache& inPainterCache);
    void AddLine(glm::vec2 inFirstPoint, glm::vec2 inSecondPoint, float inDepthValue, uint32_t& outId);
    int AddEXT(glm::vec3 inFirstPoint, glm::vec3 inSecondPoint);
    void UpdateLine(uint32_t inId, glm::vec2 inFirstPoint, glm::vec2 inSecondPoint, float inDepthValue);
    void UpdateEXT(uint32_t inId, glm::vec3 inFirstPoint, glm::vec3 inSecondPoint);
    void Dispatch(Window& inWindow);
    void Bind(Window& inWindow);
    void Draw(Window& inWindow, glm::vec4 inColor, uint32_t inWindowW, uint32_t inWindowH, uint32_t inStartLineId, uint32_t inEndLineId, glm::mat4 inMatrix);
    void DrawAll(Window& inWindow, glm::vec4 inColor, uint32_t inWindowW, uint32_t inWindowH, glm::mat4 inMatrix)
    {
        Draw(inWindow, inColor, inWindowW, inWindowH, 0, lb::GetCurrentLineOffsetAbsolute(), inMatrix);
    }

private:
    struct PushConstant {
        glm::mat4 mMatrix;
        glm::vec4 mColor;
    };
    uint32_t mTotalNoOfLinesRendererCanHold = rb::InitialRendererElementArraySize;

private:
    void CheckAndResize(const Instance& inInstance, uint32_t inNewSizeNeeded);
    const Instance& mInstance;
    Up<Painter> mPainter;
    Up<Primitive> mPrimitive;

private:
    glm::vec2 mSizeFactor = { 1, 1 };

public:
    SETTER SetSizeFactor(glm::vec2 inFactor)
    {
        mSizeFactor = inFactor;
    }
};

inline int Line::AddEXT(glm::vec3 inFirstPoint, glm::vec3 inSecondPoint) {
        uint32_t i;
        AddLine(inFirstPoint, inSecondPoint, inFirstPoint.z, i);
        return i;
}

inline void Line::UpdateEXT(uint32_t inId, glm::vec3 inFirstPoint, glm::vec3 inSecondPoint) {
    UpdateLine(inId, inFirstPoint, inSecondPoint, inFirstPoint.z);
}

}
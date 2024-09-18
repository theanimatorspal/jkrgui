#pragma once
#include "../Renderer_base.hpp"
#include "Line_base.hpp"

namespace Jkr::Renderer {

class Line : public Line_base, Renderer_base {
    using rb = Renderer_base;
    using lb = Line_base;

    public:
    Line(Instance &inInstance, Window_base &inCompatibleWindow, PainterCache &inPainterCache);
    void
    AddLine(glm::vec2 inFirstPoint, glm::vec2 inSecondPoint, float inDepthValue, uint32_t &outId);
    int AddEXT(glm::vec3 inFirstPoint, glm::vec3 inSecondPoint);
    void
    UpdateLine(uint32_t inId, glm::vec2 inFirstPoint, glm::vec2 inSecondPoint, float inDepthValue);
    void UpdateEXT(uint32_t inId, glm::vec3 inFirstPoint, glm::vec3 inSecondPoint);
    void Dispatch(Window_base &inWindow);
    void Bind(Window_base &inWindow);
    void Draw(Window_base &inWindow,
              glm::vec4 inColor,
              uint32_t inWindowW,
              uint32_t inWindowH,
              uint32_t inStartLineId,
              uint32_t inEndLineId,
              glm::mat4 inMatrix);
    void DrawAll(Window_base &inWindow,
                 glm::vec4 inColor,
                 uint32_t inWindowW,
                 uint32_t inWindowH,
                 glm::mat4 inMatrix) {
        Draw(inWindow,
             inColor,
             inWindowW,
             inWindowH,
             0,
             lb::GetCurrentLineOffsetAbsolute(),
             inMatrix);
    }

    void DrawEXT(Window_base &inWindow,
                 glm::vec4 inColor,
                 uint32_t inStartLineId,
                 uint32_t inEndLineId,
                 glm::mat4 inMatrix);

    private:
    struct PushConstant {
        glm::mat4 mMatrix;
        glm::vec4 mColor;
    };
    uint32_t mTotalNoOfLinesRendererCanHold = rb::InitialRendererElementArraySize;

    private:
    void CheckAndResize(Instance &inInstance, uint32_t inNewSizeNeeded);
    Instance &mInstance;
    Up<Painter> mPainter;
    Up<Primitive> mPrimitive;

    private:
    glm::vec2 mSizeFactor = {1, 1};

    public:
    SETTER SetSizeFactor(glm::vec2 inFactor) { mSizeFactor = inFactor; }
};

inline int Line::AddEXT(glm::vec3 inFirstPoint, glm::vec3 inSecondPoint) {
    uint32_t i;
    AddLine(inFirstPoint, inSecondPoint, inFirstPoint.z, i);
    return i;
}

inline void Line::UpdateEXT(uint32_t inId, glm::vec3 inFirstPoint, glm::vec3 inSecondPoint) {
    UpdateLine(inId, inFirstPoint, inSecondPoint, inFirstPoint.z);
}

inline void Line::DrawEXT(Window_base &inWindow,
                          glm::vec4 inColor,
                          uint32_t inStartLineId,
                          uint32_t inEndLineId,
                          glm::mat4 inMatrix) {
    PushConstant Push;
    Push.mColor  = inColor;
    Push.mMatrix = inMatrix;

    mPainter->Draw_EXT<PushConstant>(
         Push, inWindow, 2 * (inEndLineId - inStartLineId + 1), 1, inStartLineId * 2, 0);
}

} // namespace Jkr::Renderer
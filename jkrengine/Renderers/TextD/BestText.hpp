
#pragma once
#include <glm/common.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Renderer_base.hpp"
#include "BestText_base.hpp"
#include "Painter.hpp"

#ifdef JKR_USE_VARIABLE_DES_INDEXING

namespace Jkr::Renderer {

class BestText : public BestText_base, Renderer_base {
    using bb = BestText_base;
    using rb = Renderer_base;

    public:
    BestText(Instance &inInstance,
             Window &inCompatibleWindow,
             const PainterCache &inCache,
             ui inVarDesCount = 5000);
    bb::TextDimensions AddText(
         const std::string_view inText, ui inX, ui inY, ui inDepthValue, ui &outId, ui &outLength);
    bb::TextDimensions
    UpdateText(ui inId, const std::string_view inText, ui inX, ui inY, ui inDepthValue);
    void Dispatch(Window &inWindow);
    void Bind(Window &inWindow, CmdParam inParam = CmdParam::UI);
    void Draw(Window &inWindow,
              glm::vec4 inColor,
              ui inWindowW,
              ui inWindowH,
              ui inStartTextId,
              ui inNoOfChars,
              glm::mat4 inMatrix);
    void CheckAndResize(size_t inNewSize);

    private:
    ui mCurrentFontFaceId = 0;

    public:
    GETTER GetCurrentFontFace() { return mCurrentFontFaceId; }
    SETTER SetCurrentFontFace(ui inFontFaceId) { mCurrentFontFaceId = inFontFaceId; }

    private:
    Instance &mInstance;
    struct PushConstant {
        glm::mat4 mMatrix;
        glm::vec4 mColor;
    };
    ui mTotalNoOfCharsRendererCanHold = rb::InitialRendererElementArraySize;
    std::unordered_map<ui, ui> mCodepointKeyTextureMap;

    private:
    Up<Primitive> mPrimitive;
    Up<Painter> mPainter;
    Up<UniformType> mUniformBuffer;
    std::vector<Up<ImageType>> mGlyphImages;

#ifndef JKR_NO_STAGING_BUFFERS
    Up<StagingBuffer> mStagingBuffer;
#endif
};

} // namespace Jkr::Renderer
#else

namespace Jkr::Renderer {

class BestText : public BestText_base, Renderer_base {
    using bb = BestText_base;
    using rb = Renderer_base;

    public:
    BestText(Instance &inInstance,
             Window_base &inCompatibleWindow,
             const PainterCache &inCache,
             ui inVarDesCount = 5000)
        : mInstance(inInstance) {}
    bb::TextDimensions
    AddText(const sv inText, ui inX, ui inY, ui inDepthValue, ui &outId, ui &outLength) {
        return {};
    }
    bb::TextDimensions UpdateText(ui inId, const sv inText, ui inX, ui inY, ui inDepthValue) {
        return {};
    }
    void Dispatch(Window_base &inWindow) {}
    void Bind(Window_base &inWindow) {}
    void Draw(Window_base &inWindow,
              glm::vec4 inColor,
              ui inWindowW,
              ui inWindowH,
              ui inStartTextId,
              ui inNoOfChars,
              glm::mat4 inMatrix) {}
    void CheckAndResize(size_t inNewSize) {}

    private:
    ui mCurrentFontFaceId = 0;

    public:
    GETTER GetCurrentFontFace() { return mCurrentFontFaceId; }
    SETTER SetCurrentFontFace(ui inFontFaceId) { mCurrentFontFaceId = inFontFaceId; }

    private:
    Instance &mInstance;
    struct PushConstant {
        glm::mat4 mMatrix;
        glm::vec4 mColor;
    };
    ui mTotalNoOfCharsRendererCanHold = rb::InitialRendererElementArraySize;
    std::unordered_map<ui, ui> mCodepointKeyTextureMap;

    private:
    Up<Primitive> mPrimitive;
    Up<Painter> mPainter;
    Up<UniformType> mUniformBuffer;
    v<Up<ImageType>> mGlyphImages;

#ifndef JKR_NO_STAGING_BUFFERS
    Up<StagingBuffer> mStagingBuffer;
#endif
};

} // namespace Jkr::Renderer

#endif

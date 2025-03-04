#pragma once
#include "Instance.hpp"
#include "TextD/BestText_base.hpp"
#include "TwoD/Shape.hpp"
#include <Renderers/Renderer_base.hpp>
#include <Vendor/Tracy/tracy/Tracy.hpp>
#include <Vendor/stbi/stb_image_write.h>

namespace Jkr::Renderer {

///@note An alternate renderer of the Best Text Renderer for platforms that don't support variable
/// des indexing. @warning The other renderer i.e. BestText Renderer is obsolete for now, it is not
/// going to be used This implementation renders the text into an image and uses the Shape Renderer
/// to display into a rectangle
///@warning this Is not that efficient since it does a queue submit

struct BestText_Alt {
    struct ImageId {
        ui mRectId;
        ui mImgId;
    };
    BestText_Alt(Instance &inInstance, Shape &inSh, BestText_base &inBestText)
        : sh(inSh), bt(inBestText), i(inInstance) {}

    ImageId AddEXT(ui inFontId, glm::vec3 inPos, const sv inText);
    void UpdateEXT(ImageId inId, ui inFontId, glm::vec3 inPos, const s inText);
    void UpdatePosOnlyEXT(ImageId inId, ui inFontId, glm::vec3 inPos, const sv inText);
    void DrawEXT(ImageId inId,
                 Window_base &inWindow,
                 glm::vec4 inColor,
                 glm::mat4 inMatrix,
                 CmdParam inParam);
    ///@warning This is a simple wrapping, which doesn't implement any space breaking algorithms and
    /// stuff like those
    v<s> WrapToTextVector(const sv inText, ui inFontId, glm::vec2 inDimensionTobeWrappedTo);

    private:
    Shape &sh;
    BestText_base &bt;
    Instance &i;
};
} // namespace Jkr::Renderer

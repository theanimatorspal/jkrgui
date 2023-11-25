#pragma once
#include <Components/Area_base.hpp>

namespace Jkr::Component {
class FocusProxy {
protected:
    void FocusEvent(Area_base& inAreaBase, bool isLeftButtonPressed, bool isMouseOver, bool isMouseOnTop)
    {
        if (not mFocusOnHover) {
            if (isLeftButtonPressed and isMouseOver and !inAreaBase.IsFocused())
                inAreaBase.ToggleFocus();
            if (inAreaBase.IsFocused() and !isMouseOver and isLeftButtonPressed)
                inAreaBase.ToggleFocus();
        } else {
            if (isMouseOver and !inAreaBase.IsFocused())
                inAreaBase.ToggleFocus();
            if (inAreaBase.IsFocused() and !isMouseOver)
                inAreaBase.ToggleFocus();
        }

        if (inAreaBase.IsFocused()) {
            mOutLineColor = mOutLineColorFocused;
        } else {
            mOutLineColor = mOutLineColorUnFocused;
        }
    }

    void FocusEvent(Area_base& inAreaBase, bool isMouseOver, bool isMouseOnTop)
    {
        if (isMouseOver and !inAreaBase.IsFocused())
            inAreaBase.ToggleFocus();
        if (inAreaBase.IsFocused() and !isMouseOver)
            inAreaBase.ToggleFocus();
        if (inAreaBase.IsFocused()) {
            mOutLineColor = mOutLineColorFocused;
        } else {
            mOutLineColor = mOutLineColorUnFocused;
        }
    }

    void FocusEvent(Area_base& inAreaBase, bool isLeftButtonPressed)
    {
        if (isLeftButtonPressed and inAreaBase.IsMouseWithin() and !inAreaBase.IsFocused())
            inAreaBase.ToggleFocus();
        if (inAreaBase.IsFocused() and !inAreaBase.IsMouseWithin() and isLeftButtonPressed)
            inAreaBase.ToggleFocus();

        if (inAreaBase.IsFocused()) {
            mOutLineColor = mOutLineColorFocused;
        } else {
            mOutLineColor = mOutLineColorUnFocused;
        }
    }
    GETTER GetFocusColor() { return mOutLineColor; }
    SETTER SetFocusOnHover(bool inBool) { mFocusOnHover = inBool; }

public:
    SETTER SetOutlineColorFocused(glm::vec4 inColor) { mOutLineColorFocused = inColor; }
    SETTER SetOutlineColorUnFocused(glm::vec4 inColor) { mOutLineColorUnFocused = inColor; }

private:
    glm::vec4 mOutLineColorFocused = glm::vec4(1.0f, 1.0f, 1.0f, 0.8f);
    glm::vec4 mOutLineColorUnFocused = glm::vec4(1.0f, 1.0f, 1.0f, 0.1f);
    glm::vec4 mOutLineColor = mOutLineColorUnFocused;
    bool mFocusOnHover = false;
};
}
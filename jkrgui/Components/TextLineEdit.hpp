#pragma once
#include "Area_base.hpp"
#include "BTextButtonRect.hpp"
#include "FocusProxy.hpp"
#include <Vendor/Tracy/tracy/Tracy.hpp>

namespace Jkr::Component {
class TextLineEdit : public Area_base, public FocusProxy {
    using ab = Area_base;

public:
    TextLineEdit(_2d& inR, EventManager& inE)
        : Area_base(inR, inE)
        , FocusProxy()
        , mTextCursor(inR, inE)
    {
    }

    void Load();
    void Event();
    void DrawBestTexts()
    {
        r.bt.Draw(*this->GetWindow(), FocusProxy::GetFocusColor(), this->GetWindowWidth(), this->GetWindowHeight(), mStringViewOutId, mStringViewLength, this->GetTranslationMatrix());
    }
    void DrawOutlines()
    {
        auto line = this->GetLines().front();
        r.ln.Draw(*this->GetWindow(), FocusProxy::GetFocusColor(), this->GetWindowWidth(), this->GetWindowHeight(), line.x, line.y, this->GetTranslationMatrix());
        mTextCursor.DrawLines(*this->GetWindow(), FocusProxy::GetFocusColor(), this->GetWindowWidth(), this->GetWindowHeight());
    }
    void Update();
    SETTER SetInitialString(const std::string_view inString)
    {
        assert(inString.length() > mMinimumInitialLengthOfString && "This String is also used for calculating the maximum width, so put some spaces \n or longer word");
        mCurrentString = inString;
    }

    GETTER GetCurrentStringView ( )
    {
        return std::string_view(mCurrentString.begin(), mCurrentString.end() + mRightEditingPosition);
    }

private:
    struct Cursor : Component_base {
        Cursor(_2d& inR, EventManager& inE)
            : Component_base(inR, inE)
        {
        }
        void Load()
        {
            r.ln.AddLine(glm::vec2(0, 0), this->GetDimension(), this->GetDepthValue(), mCursorLineId);
        }
        void DrawLines(Window& inWindow, glm::vec4 inColor, uint32_t inWindowWidth, uint32_t inWindowHeight)
        {
            r.ln.Draw(inWindow, inColor, inWindowWidth, inWindowHeight, mCursorLineId, mCursorLineId, this->GetTranslationMatrix());
        }
        void Update()
        {
            r.ln.UpdateLine(mCursorLineId, glm::vec2(0, 0), { 0, this->GetDimension().y }, this->GetDepthValue());
        }

    private:
        uint32_t mCursorLineId;
    };
    Cursor mTextCursor;

private:
    uint32_t mMaxNoOfGlyphs = 15;
    std::string mCurrentString = "                              ";
    uint32_t mStringViewLength;
    uint32_t mStringViewOutId;
    uint32_t mRightEditingPosition = 0;
    uint32_t mLeftEditingPosition = 0;
    bool mHasEditingStarted = false;
    bool mShouldUpdateText = false;
    bool mShouldResetString = true;
    bool mShouldIncreaseLeftEditingPosition = false;

private:
    uint32_t mVPadding = 5;
    uint32_t mHPadding = 5;

private:
    const uint32_t mMinimumInitialLengthOfString = 5;
};
}
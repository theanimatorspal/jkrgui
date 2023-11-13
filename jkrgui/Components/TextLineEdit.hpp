#include "Area_base.hpp"
#include "BTextButtonRect.hpp"
#include "FocusProxy.hpp"

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
    void Load()
    {
        auto TextDimensions = r.bt.AddText(mCurrentString, 0, 0, ab::GetDepthValue(), mStringViewOutId, mStringViewLength);
        ab::SetDimension(glm::vec2(TextDimensions.mWidth + 2 * mHPadding, TextDimensions.mHeight + 2 * mVPadding));
        this->SetDefaultBoundedRectangle();
        ab::Load();
        mTextCursor.SetDimension(glm::vec2(0, TextDimensions.mHeight + 2 * mVPadding));
        mTextCursor.SetDepthValue(ab::GetDepthValue() - 1);
        mTextCursor.Load();
        mShouldUpdateText = true;
    }
    void Event()
    {
        auto ev = e.GetEventHandle();
        bool IsLeftButtonPressed = (SDL_BUTTON(SDL_BUTTON_LEFT) bitand e.GetMouseButtonValue()) != 0;
        auto IsMouseOn = this->IsMouseOnTop();
        auto IsMouseOver = this->IsMouseWithin();
        FocusProxy::FocusEvent(*this, IsLeftButtonPressed, IsMouseOver, IsMouseOn);

        if (!mHasEditingStarted and this->IsFocused()) {
            std::fill(mCurrentString.begin(), mCurrentString.end(), ' ');
            r.bt.UpdateText(mStringViewOutId, mCurrentString, mHPadding, mVPadding, ab::GetDepthValue() - 1);
            e.StartTextInput();
            mHasEditingStarted = true;
        } else if (mHasEditingStarted and !this->IsFocused()) {
            // mCurrentString = mInitialString;
            // r.bt.UpdateText(mStringViewOutId, mStringViewOfString, mHPadding, mVPadding, ab::GetDepthValue() - 1);
            mEditingPosition = 0;
            e.StopTextInput();
            mHasEditingStarted = false;
        }

        if (ev.type == SDL_KEYDOWN) {
            if (ev.key.keysym.sym == SDLK_BACKSPACE and mCurrentString.length() > 0 and mEditingPosition > 0) {
                std::fill(mCurrentString.begin() + mEditingPosition - 1, mCurrentString.end(), ' ');
                mEditingPosition--;
                mShouldUpdateText = true;
            } else if (ev.key.keysym.sym == SDLK_c and SDL_GetModState() bitand KMOD_CTRL) {
                SDL_SetClipboardText(mCurrentString.c_str());
                mShouldUpdateText = true;
            } else if (ev.key.keysym.sym == SDLK_v and SDL_GetModState() bitand KMOD_CTRL) {
                mCurrentString = SDL_GetClipboardText();
                mShouldUpdateText = true;
            }
        } else if (ev.type == SDL_TEXTINPUT) {
            bool NotCopyAndPasteKeys = not(SDL_GetModState() bitand KMOD_ALT and (ev.text.text[0] == 'C' || ev.text.text[0] == 'c' || ev.text.text[0] == 'v' || ev.text.text[0] == 'V'));

            if (NotCopyAndPasteKeys) {
                if (mEditingPosition == 0) {
                    mCurrentString = "";
                }
                mShouldUpdateText = true;
                mCurrentString.append(ev.text.text);
                mEditingPosition++;
                mShouldUpdateText = true;
            }
        }
    }
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
    void Update()
    {
        ab::Update();
		ab::UpdateDefaultBoundedRectangle();
        if (mShouldUpdateText) {
            auto TextDimensions = r.bt.UpdateText(mStringViewOutId, mCurrentString, mHPadding, mVPadding, ab::GetDepthValue() - 1);
            this->UpdateCursorPosition();
            mShouldUpdateText = false;
            ab::SetDimension(glm::vec2(ab::GetDimension().x, TextDimensions.mHeight + 2 * mVPadding));
        }
    }
    SETTER SetInitialString(const std::string_view inString)
    {
        assert(inString.length() > mMinimumInitialLengthOfString && "This String is also used for calculating the maximum width, so put some spaces \n or longer word");
        mCurrentString = inString;
        mInitialString = inString;
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

    private:
        uint32_t mCursorLineId;
    };
    Cursor mTextCursor;
    void UpdateCursorPosition()
    {
        auto sv = static_cast<std::string_view>(mCurrentString).substr(0, mEditingPosition);
        auto td = r.bt.GetTextDimensions(sv, r.bt.GetCurrentFontFace());
        mTextCursor.SetPosition(
            glm::vec2(this->GetPosition().x + td.mWidth,
                this->GetPosition().y));
        mTextCursor.SetDimension(glm::vec2(0, td.mHeight));
    }

private:
    uint32_t mMaxLength;
    std::string mCurrentString = "टङ्क्यतामत्र कृपया...     ";
    std::string mInitialString = mCurrentString;
    uint32_t mStringViewLength;
    uint32_t mStringViewOutId;
    uint32_t mEditingPosition = 0;
    bool mHasEditingStarted = false;
    bool mShouldUpdateText = false;

private:
    uint32_t mVPadding = 5;
    uint32_t mHPadding = 5;

private:
    const uint32_t mMinimumInitialLengthOfString = 5;
};
}
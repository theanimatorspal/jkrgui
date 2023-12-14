#include "TextLineEdit.hpp"
#include <cassert>

void Jkr::Component::TextLineEdit::Load()
{
    mMaxNoOfGlyphs = 18;

    r.bt.SetTextProperty(Jkr::Renderer::BestText_base::TextProperty { .H = Jkr::Renderer::BestText_base::AlignH::Left, .V = Jkr::Renderer::BestText_base::AlignV::Top });
    auto TextDimensions = r.bt.AddText(mCurrentString, 0, this->GetDimension().y - mVPadding, ab::GetDepthValue(), mStringViewOutId, mStringViewLength);
    this->SetDefaultBoundedRectangle();
    r.bt.UpdateText(mStringViewOutId, mCurrentString, mHPadding, this->GetDimension().y - mVPadding, ab::GetDepthValue() - 1);
    ab::Load();
    mTextCursor.SetDimension(glm::vec2(0, this->GetDimension().y));
    mTextCursor.SetPosition(
        glm::vec2(this->GetPosition().x,
            this->GetPosition().y));
    mTextCursor.SetDepthValue(ab::GetDepthValue() - 1);
    mTextCursor.Load();
    mTextCursor.Update();
    e.StopTextInput();
    mShouldUpdateText = true;
    mRightEditingPosition = 0;
    mLeftEditingPosition = 0;
}

void Jkr::Component::TextLineEdit::Event()
{
    r.bt.SetTextProperty(Jkr::Renderer::BestText_base::TextProperty { .H = Jkr::Renderer::BestText_base::AlignH::Left, .V = Jkr::Renderer::BestText_base::AlignV::Top });
    auto ev = e.GetEventHandle();
    bool IsLeftButtonPressed = (SDL_BUTTON(SDL_BUTTON_LEFT) bitand e.GetMouseButtonValue()) != 0;
    auto IsMouseOn = this->IsMouseOnTop();
    auto IsMouseOver = this->IsMouseWithin();
    FocusProxy::FocusEvent(*this, IsLeftButtonPressed, IsMouseOver, IsMouseOn);

    if (!mHasEditingStarted and this->IsFocused()) {
        if (mShouldResetString)
            std::fill(mCurrentString.begin(), mCurrentString.end(), ' ');
        mShouldUpdateText = true;
        e.StartTextInput();
        mHasEditingStarted = true;
    } else if (mHasEditingStarted and !this->IsFocused()) {
        if (mShouldResetString) {
            std::fill(mCurrentString.begin(), mCurrentString.end(), ' ');
            mRightEditingPosition = 0;
        }
        mShouldUpdateText = true;
       // e.StopTextInput();
        mHasEditingStarted = false;
    }

    if (this->IsFocused()) {
        if (ev.type == SDL_KEYDOWN) {
            if (ev.key.keysym.sym == SDLK_BACKSPACE and mCurrentString.length() > 0) {
                if (mRightEditingPosition > 0) {
                    std::fill(mCurrentString.begin() + mRightEditingPosition, mCurrentString.end(), ' ');
                    mCurrentString[mRightEditingPosition - 1] = ' ';
                    mRightEditingPosition--;
                }
                if (mLeftEditingPosition > 0) {
                    mLeftEditingPosition--;
                }
                mShouldUpdateText = true;
            } else if (ev.key.keysym.sym == SDLK_c and SDL_GetModState() bitand KMOD_CTRL) {
                SDL_SetClipboardText(mCurrentString.c_str());
                mShouldUpdateText = true;
            } else if (ev.key.keysym.sym == SDLK_v and SDL_GetModState() bitand KMOD_CTRL) {
                mCurrentString = SDL_GetClipboardText();
                mShouldUpdateText = true;
            } else if (ev.key.keysym.sym == SDLK_RETURN) {
                this->SetFocus(false);
                e.StopTextInput();
                mShouldResetString = false;
            }
        } else if (ev.type == SDL_TEXTINPUT) {
            bool NotCopyAndPasteKeys = not(SDL_GetModState() bitand KMOD_ALT and (ev.text.text[0] == 'C' || ev.text.text[0] == 'c' || ev.text.text[0] == 'v' || ev.text.text[0] == 'V'));

            if (NotCopyAndPasteKeys) {
                if (mRightEditingPosition == 0) {
                    mCurrentString = "";
                }
                mCurrentString.resize(mRightEditingPosition);
                mShouldUpdateText = true;
                mCurrentString.append(ev.text.text);
                mRightEditingPosition++;
                mShouldUpdateText = true;
                mShouldIncreaseLeftEditingPosition = true;
            }
        }
    }
}

void Jkr::Component::TextLineEdit::Update()
{
    r.bt.SetTextProperty(Jkr::Renderer::BestText_base::TextProperty { .H = Jkr::Renderer::BestText_base::AlignH::Left, .V = Jkr::Renderer::BestText_base::AlignV::Top });
    ab::Update();
    ab::UpdateDefaultBoundedRectangle();
    auto StringView = std::string_view { mCurrentString.begin() + mLeftEditingPosition, mCurrentString.begin() + mRightEditingPosition };

    if (mShouldUpdateText) {
        if (mRightEditingPosition == 0) {
            StringView = " ";
        }

        auto TextDimensions = r.bt.UpdateText(mStringViewOutId, StringView, mHPadding, this->GetDimension().y - mVPadding, ab::GetDepthValue() - 1);

        bool TextWidthGreaterThanWidth = TextDimensions.mWidth + 2 * mHPadding >= this->GetDimension().x;
        bool IsGreaterThanMaxNoOfGlyphs = (mRightEditingPosition - mLeftEditingPosition) >= mMaxNoOfGlyphs;

        if (IsGreaterThanMaxNoOfGlyphs and mShouldIncreaseLeftEditingPosition) {
            mLeftEditingPosition++;
            mShouldIncreaseLeftEditingPosition = false;
        }

        if (mLeftEditingPosition <= mRightEditingPosition) {
            auto NewStringStr = std::string { mCurrentString.begin() + mLeftEditingPosition, mCurrentString.begin() + mRightEditingPosition};
            auto Tdims = r.bt.GetTextDimensions(NewStringStr, r.bt.GetCurrentFontFace());
            mTextCursor.SetPosition(
                glm::vec2(this->GetPosition().x + Tdims.mWidth + mHPadding,
                    this->GetPosition().y));
            mTextCursor.SetDimension(glm::vec2(0, this->GetDimension().y));
            mTextCursor.Update();
        } else {
            assert("Error" && false);
        }
        mShouldUpdateText = false;
    }
}

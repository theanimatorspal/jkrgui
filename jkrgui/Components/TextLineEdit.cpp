#include "TextLineEdit.hpp"

void Jkr::Component::TextLineEdit::Load()
{
    ZoneScoped;
    auto TextDimensions = r.bt.AddText(mCurrentString, 0, 0, ab::GetDepthValue(), mStringViewOutId, mStringViewLength);
    this->SetDefaultBoundedRectangle();
    r.bt.UpdateText(mStringViewOutId, mCurrentString, mHPadding, mVPadding, ab::GetDepthValue() - 1);
    ab::Load();
    mTextCursor.SetDimension(glm::vec2(0, TextDimensions.mHeight + 2 * mVPadding));
    mTextCursor.SetDepthValue(ab::GetDepthValue() - 1);
    mTextCursor.Load();
    e.StopTextInput();
    mShouldUpdateText = true;
}

void Jkr::Component::TextLineEdit::Event()
{
    ZoneScoped;
    auto ev = e.GetEventHandle();
    bool IsLeftButtonPressed = (SDL_BUTTON(SDL_BUTTON_LEFT) bitand e.GetMouseButtonValue()) != 0;
    auto IsMouseOn = this->IsMouseOnTop();
    auto IsMouseOver = this->IsMouseWithin();
    FocusProxy::FocusEvent(*this, IsLeftButtonPressed, IsMouseOver, IsMouseOn);

    if (!mHasEditingStarted and this->IsFocused()) {
        if (mShouldResetString)
            std::fill(mCurrentString.begin(), mCurrentString.end(), ' ');
        r.bt.UpdateText(mStringViewOutId, mCurrentString, mHPadding, mVPadding, ab::GetDepthValue() - 1);
        e.StartTextInput();
        mHasEditingStarted = true;
    } else if (mHasEditingStarted and !this->IsFocused()) {
        if (mShouldResetString) {
            mCurrentString = mInitialString;
            mRightEditingPosition = 0;
        }
        r.bt.UpdateText(mStringViewOutId, mCurrentString, mHPadding, mVPadding, ab::GetDepthValue() - 1);
        e.StopTextInput();
        mHasEditingStarted = false;
    }

    if (ev.type == SDL_KEYDOWN) {
        if (ev.key.keysym.sym == SDLK_BACKSPACE and mCurrentString.length() > 0) {
            if (mRightEditingPosition > 0) {
                // std::fill(mCurrentString.begin() + mRightEditingPosition - 1, mCurrentString.end(), ' ');
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
            mShouldUpdateText = true;
            mCurrentString.append(ev.text.text);
            mRightEditingPosition++;
            mShouldUpdateText = true;
        }
    }
}

void Jkr::Component::TextLineEdit::Update()
{
    ab::Update();
    ab::UpdateDefaultBoundedRectangle();
    if (mShouldUpdateText) {
        auto NewString = std::string_view { mCurrentString.begin() + mLeftEditingPosition, mCurrentString.begin() + mRightEditingPosition };
        if (mRightEditingPosition == 0) {
            NewString = " ";
        }

        auto TextDimensions = r.bt.UpdateText(mStringViewOutId, NewString, mHPadding, mVPadding, ab::GetDepthValue() - 1);
        if (TextDimensions.mWidth + 2 * mHPadding >= this->GetDimension().x) {
            mLeftEditingPosition++;
        }
        // auto Dimesiony = TextDimensions.mHeight + 2 * mVPadding;
        // ab::SetDimension(glm::vec2(ab::GetDimension().x, TextDimensions.mHeight == 0 ? ab::GetDimension().y : Dimesiony));

        if (mLeftEditingPosition <= mRightEditingPosition) {
			auto NewStringStr = std::string { mCurrentString.begin() + mLeftEditingPosition, mCurrentString.begin() + mRightEditingPosition };
            auto Tdims = r.bt.GetTextDimensions(NewStringStr, r.bt.GetCurrentFontFace());
            mTextCursor.SetPosition(
                glm::vec2(this->GetPosition().x + Tdims.mWidth + mHPadding,
                    this->GetPosition().y));
            mTextCursor.SetDimension(glm::vec2(0, this->GetDimension().y));
            mTextCursor.Update();
            this->UpdateCursorPosition();
        }
        mShouldUpdateText = false;
    }
}

void Jkr::Component::TextLineEdit::UpdateCursorPosition()
{
}

#include "BTextButtonRect.hpp"
#include "HLayout.hpp"
#include "ImageButtonRect.hpp"
#include "TextLineEdit.hpp"
#include "VLayout.hpp"

namespace Jkr::Component {

class ContextMenuList : public Area_base {
    using ab = Area_base;

public:
    ContextMenuList(_2d& inR, EventManager& inE)
        : ab(inR, inE)
    {
    }

    void Load();

    void DrawOutlines()
    {
        mTextLineEdit->DrawOutlines();
    }

    void DrawShapes()
    {
        for (auto& u : mMenuItemsTextButtons)
            u->DrawShapes();
    }

    void Update()
    {
        mHLayout->SetPosition(this->GetPosition());
        mHLayout->ResetPositionsAndDimensions({ 0.2f, 0.8f });

        mImageButton->Update();
        mTextLineEdit->Update();

        if (mSelectedMenuItem.has_value()) {
            for (auto& u : mMenuItemsTextButtons) {
                u->SetNormalColor(glm::vec4(0.2f, 0.2f, 0.2f, 0.5f));
            }
            mMenuItemsTextButtons[mSelectedMenuItem.value()]->SetNormalColor(glm::vec4(0.4f, 0.4f, 0.4f, 0.5f));
        }
    }

    void DrawBestTexts()
    {
        for (auto& u : mMenuItemsTextButtons)
            u->DrawBestTexts();
        mTextLineEdit->DrawBestTexts();
    }

    void Event()
    {
        mTextLineEdit->Event();
        for (auto& u : mMenuItemsTextButtons) {
            u->Event();
        }

        auto ev = e.GetEventHandle();

        if (ev.type == SDL_KEYDOWN) {
            switch (ev.key.keysym.sym) {
            case SDLK_DOWN:
                if (not mSelectedMenuItem.has_value()) {
                    mSelectedMenuItem = 0;
                } else if (mSelectedMenuItem.value() < mNumVisibleItems - 1) {
                    mSelectedMenuItem.value()++;
                } else {
                    MoveCurrentItemViewPositionDown();
                }
                break;
            case SDLK_UP:
                if (not mSelectedMenuItem.has_value()) {
                    mSelectedMenuItem = mNumVisibleItems - 1;
                } else if (mSelectedMenuItem.value() > 0) {
                    mSelectedMenuItem.value()--;
                } else {
                    MoveCurrentItemViewPositionUp();
                }
                break;
            }
        }
    }

    void DrawImages(Window& inWindow, uint32_t inX, uint32_t inY)
    {
        mImageButton->DrawImages(inWindow, inX, inY);
    }

    SETTER SetMenuItemStrings(const std::vector<std::string_view>& inString)
    {
        mStrings = std::move(inString);
    }

    SETTER ResetSelection()
    {
        if (mSelectedMenuItem.has_value()) {
            mSelectedMenuItem.reset();
        }
    }

private:
    SETTER SetSelection(uint32_t inSelection)
    {
        mSelectedMenuItem = inSelection;
    }
    static constexpr uint32_t mNumVisibleItems = 5;
    uint32_t mPadding = 5;
    uint32_t mWidthOfMenu = 142;
    uint32_t mHeightOfMenuItem = 32;
    glm::vec2 mImageButtonDimension = { 50, 10 };
    std::array<float, 2> mMenuItemLayoutSizeFactor = { 0.2, 0.8 };

private:
    void MoveCurrentItemViewPositionDown()
    {
        if (mCurrentItemViewPosition.r < mCurrentItemViewPosition.g and mCurrentItemViewPosition.g < mStrings.size()) {
            mCurrentItemViewPosition.r++;
            mCurrentItemViewPosition.g++;

            int j = 0;
            for (int i = mCurrentItemViewPosition.r; i <= mCurrentItemViewPosition.g and mCurrentItemViewPosition.g < mStrings.size(); i++) {
                auto& button = mMenuItemsTextButtons[j];
                button->SetText(mStrings[i]);
                button->Update();
                j++;
            }
        }
    }
    void MoveCurrentItemViewPositionUp()
    {
        if (mCurrentItemViewPosition.r < mCurrentItemViewPosition.g and mCurrentItemViewPosition.g >= mMaxNumberOfMenuItems) {
            mCurrentItemViewPosition.r--;
            mCurrentItemViewPosition.g--;

            int j = 0;
            for (int i = mCurrentItemViewPosition.r; i <= mCurrentItemViewPosition.g; i++) {
                auto& button = mMenuItemsTextButtons[j];
                button->SetText(mStrings[i]);
                button->Update();
                j++;
            }
        }
    }
    uint32_t mMaxNumberOfMenuItems = 5;
    glm::uvec2 mCurrentItemViewPosition = { 0, 4 };
    std::vector<std::string_view> mStrings;

private:
    using VerticalLayoutType = VerticalLayout<mNumVisibleItems>;
    using MenuItemHorizontalLayoutType = HorizontalLayout<2>;

    std::vector<Sp<ImageButtonRect>> mMenuItemsImages;
    std::vector<Sp<BTextButtonRect>> mMenuItemsTextButtons;
    std::vector<Sp<MenuItemHorizontalLayoutType>> mMenuItemsHorizontalLayout;
    Up<VerticalLayoutType> mItemsVerticalLayout;

private:
    Sp<HorizontalLayout<2>> mHLayout;
    Sp<ImageButtonRect> mImageButton;
    Sp<TextLineEdit> mTextLineEdit;
    std::optional<uint32_t> mSelectedMenuItem;
};

}
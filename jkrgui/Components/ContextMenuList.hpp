#include "BTextButtonRect.hpp"
#include "HLayout.hpp"
#include "ImageButtonRect.hpp"
#include "TextLineEdit.hpp"
#include "VLayout.hpp"
#include "FocusProxy.hpp"

namespace Jkr::Component {

class ContextMenuList : public Area_base, FocusProxy {
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
        UpdateDefaultBoundedRectangle();
        mHLayout->SetPosition(this->GetPosition());
        mHLayout->SetDimension(this->GetDimension());
        mHLayout->ResetPositionsAndDimensions(mMenuItemLayoutSizeFactor);
        mTextLineEdit->SetDimension({ mWidthOfMenu, mHeightOfMenuItem });
        mTextLineEdit->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
        mImageButton->SetDimension({ 50, 10 });
        mImageButton->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
        mHLayout->SetDimension(mTextLineEdit->GetDimension() + glm::vec2(50, 0));
        mHLayout->ResetPositionsAndDimensions(mMenuItemLayoutSizeFactor);
        auto PositionOfItemList = mHLayout->GetPosition() + glm::vec2(0, mHLayout->GetDimension().y);
        mItemsVerticalLayout->SetPosition(PositionOfItemList);
        mItemsVerticalLayout->SetDimension({ mWidthOfMenu + 50, mHeightOfMenuItem * mMaxNumberOfMenuItems });
        mItemsVerticalLayout->ResetPositionsAndDimensions();
        for (int i = 0; i < mNumVisibleItems; i++) {
            auto& layout = mMenuItemsHorizontalLayout[i];
            auto& button = mMenuItemsTextButtons[i];
            layout->ResetPositionsAndDimensions(mMenuItemLayoutSizeFactor);
            button->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
        }

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
    void Focus()
    {
        mTextLineEdit->SetFocus(true);
    }
    void UnFocus()
    {
        mTextLineEdit->SetFocus(false);
    }
    void Event()
    {
        using namespace std;
        FocusEvent(*this, this->IsMouseWithin(), this->IsMouseOnTop());
        mTextLineEdit->Event();
        optional<int> SelectedButtonIndex = nullopt;
        for (int i = 0; i < mMenuItemsTextButtons.size(); i++) {
            auto& u = mMenuItemsTextButtons[i];
            u->Event();
            if (u->IsBeingPushed())
                SelectedButtonIndex = i;
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
            case SDLK_RETURN:
                if (mSelectedMenuItem.has_value()) {
                    mChosenItem = mSelectedMenuItem.value() + mCurrentItemViewPosition.r;
                }
                mSelectedMenuItem = 0;
                break;
            }
        }

        if (e.GetEventHandle().type == SDL_MOUSEWHEEL and this->IsFocused()) {
            bool down = e.GetEventHandle().wheel.y > 0;
            bool up = e.GetEventHandle().wheel.y < 0;
            if (up) {
                MoveCurrentItemViewPositionDown();
            } else if (down) {
                MoveCurrentItemViewPositionUp();
            }
        }

        if (e.IsLeftButtonPressed() and SelectedButtonIndex.has_value() and this->IsFocused()) {
            mChosenItem = SelectedButtonIndex.value() + mCurrentItemViewPosition.r;
        }
    }
    void DrawImages(Window& inWindow, uint32_t inX, uint32_t inY)
    {
        mImageButton->DrawImages(inWindow, inX, inY);
    }

public:
    SETTER SetMenuItemStrings(const std::vector<std::string_view>& inString)
    {
        mStrings = std::move(inString);
    }
    GETTER GetSelectedItem() { return mChosenItem; }
    GETTER ResetSelectedItem() { mChosenItem.reset(); }

private:
    std::optional<uint32_t> mChosenItem;
    static constexpr uint32_t mNumVisibleItems = 5;
    uint32_t mPadding = 5;
    uint32_t mWidthOfMenu = 142;
    uint32_t mHeightOfMenuItem = 32;
    glm::vec2 mImageButtonDimension = { 50, 10 };
    std::array<float, 2> mMenuItemLayoutSizeFactor = { 0.2, 0.8 };

private:
    void MoveCurrentItemViewPositionDown()
    {
        if (mCurrentItemViewPosition.r < mCurrentItemViewPosition.g and mCurrentItemViewPosition.g < mStrings.size() - 1) {
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

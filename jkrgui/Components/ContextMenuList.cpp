#include "ContextMenuList.hpp"

using namespace Jkr::Component;

void ContextMenuList::Load()
{
    mHLayout = MakeSH<HorizontalLayout<2>>(r, e);
    mImageButton = MakeSH<ImageButtonRect>(r, e);
    mTextLineEdit = MakeSH<TextLineEdit>(r, e);

    mHLayout->SetPosition(this->GetPosition());
    mHLayout->SetDimension(this->GetDimension());
    mHLayout->SetHPadding(5);
    mHLayout->SetVPadding(5);
    mHLayout->AddComponent(mImageButton);
    mHLayout->AddComponent(mTextLineEdit);
    mHLayout->ResetPositionsAndDimensions(mMenuItemLayoutSizeFactor);

    mTextLineEdit->SetDimension({ mWidthOfMenu, mHeightOfMenuItem });
    mTextLineEdit->SetDepthValue(this->GetDepthValue());
    mTextLineEdit->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
    mTextLineEdit->Load();

    mImageButton->SetDimension({ 50, 10 });
    mImageButton->SetDepthValue(this->GetDepthValue());
    mImageButton->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
    mImageButton->SetFilePath("C:\\Users\\sansk\\OneDrive\\Desktop\\Project\\jkrengine\\Resources\\Icons\\search.png");

    mHLayout->SetDimension(mTextLineEdit->GetDimension() + glm::vec2(50, 0));
    mHLayout->ResetPositionsAndDimensions(mMenuItemLayoutSizeFactor);
    mImageButton->Load();

    auto PositionOfItemList = mHLayout->GetPosition() + glm::vec2(0, mHLayout->GetDimension().y);
    mItemsVerticalLayout = MakeUp<VerticalLayoutType>(r, e);
    mItemsVerticalLayout->SetPosition(PositionOfItemList);
    mItemsVerticalLayout->SetDimension({ mWidthOfMenu + 50, mHeightOfMenuItem * mMaxNumberOfMenuItems });
    mItemsVerticalLayout->SetVPadding(0);
    mItemsVerticalLayout->SetHPadding(0);

    for (int i = 0; i < mNumVisibleItems; i++) {
        mMenuItemsTextButtons.push_back(MakeSH<BTextButtonRect>(r, e));
        mMenuItemsImages.push_back(MakeSH<ImageButtonRect>(r, e));
        mMenuItemsHorizontalLayout.push_back(MakeSH<MenuItemHorizontalLayoutType>(r, e));
        mMenuItemsHorizontalLayout[i]->AddComponent(mMenuItemsImages[i]);
        mMenuItemsHorizontalLayout[i]->AddComponent(mMenuItemsTextButtons[i]);
        mItemsVerticalLayout->AddComponent(mMenuItemsHorizontalLayout[i]);
    }

    mItemsVerticalLayout->ResetPositionsAndDimensions();

    assert(mStrings.size() > mNumVisibleItems);
    /* Here is an assumption that mStrings is actually big enough TODO */
    e.MoveDepthValueTowardsTheCamera();

    for (int i = 0; i < mNumVisibleItems; i++) {
        auto& button = mMenuItemsTextButtons[i];
        auto& image = mMenuItemsImages[i];
        auto& text = mStrings[i];
        auto& layout = mMenuItemsHorizontalLayout[i];
        layout->SetHPadding(0);
        layout->SetVPadding(0);
        layout->ResetPositionsAndDimensions(mMenuItemLayoutSizeFactor);
        button->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
        button->SetText(text);
        button->SetDepthValue(this->GetDepthValue());
        layout->ResetPositionsAndDimensions(mMenuItemLayoutSizeFactor);
        button->Load();
    }

    for (auto& u : mMenuItemsTextButtons) {
        u->SetNormalColor(glm::vec4(0.2f, 0.2f, 0.2f, 0.5f));
    }

    ab::Load();
}

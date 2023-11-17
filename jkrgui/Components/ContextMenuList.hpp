#include "BTextButtonRect.hpp"
#include "ImageButtonRect.hpp"
#include "TextLineEdit.hpp"
#include "VLayout.hpp"
#include "HLayout.hpp"

namespace Jkr::Component {

class ContextMenuList : public Area_base 
{
    using ab = Area_base;
public:
    ContextMenuList(_2d& inR, EventManager& inE)
        : ab(inR, inE)
    {
    }

    /*
    @brief Expects Position and Dimensions Set beforehand though the Dimensions are modified after this function call.
    */
    void Load ( )
    {
        mHLayout = MakeUp<HorizontalLayout<2>>(r, e);
        mImageButton = MakeSH<ImageButtonRect>(r, e);
        mTextLineEdit = MakeSH<TextLineEdit>(r, e);

        mHLayout->SetPosition(this->GetPosition());
        mHLayout->SetDimension(this->GetDimension());
        mHLayout->SetHPadding(5);
        mHLayout->SetVPadding(5);
        mHLayout->AddComponent(mImageButton);
        mHLayout->AddComponent(mTextLineEdit);
        mHLayout->ResetPositionsAndDimensions({ 0.2f, 0.8f });

        mTextLineEdit->SetDimension({140, 32});
        mTextLineEdit->SetDepthValue(this->GetDepthValue());
        mTextLineEdit->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
        mTextLineEdit->Load();

        mImageButton->SetDimension({ 50, 10 });
        mImageButton->SetDepthValue(this->GetDepthValue());
        mImageButton->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
        mImageButton->SetFilePath("C:\\Users\\sansk\\OneDrive\\Desktop\\Project\\jkrengine\\Resources\\Icons\\search.png");

        mHLayout->SetDimension(mTextLineEdit->GetDimension() + glm::vec2(50, 0));
        mHLayout->ResetPositionsAndDimensions({ 0.2f, 0.8f });
        mImageButton->Load();
        ab::Load();
    }

    void DrawOutlines ( )
    {
        mTextLineEdit->DrawOutlines();
    }

    void Update ( )
    {
        mHLayout->SetPosition(this->GetPosition());
        mHLayout->ResetPositionsAndDimensions({ 0.2f, 0.8f });
        mImageButton->Update();
        mTextLineEdit->Update();
    }

    void DrawBestTexts ( )
    {
        mTextLineEdit->DrawBestTexts();
    }

    void Event() { 
        mTextLineEdit->Event();
    }

    void DrawImages (Window& inWindow, uint32_t inX, uint32_t inY)
    {
        mImageButton->DrawImages(inWindow, inX, inY);
    }

private:
    uint32_t mMaxNumberOfListViews;
    std::vector<Sp<BTextButtonRect>> mButtons;
    Up<HorizontalLayout<2>> mHLayout;
    Sp<ImageButtonRect> mImageButton;
    Sp<TextLineEdit> mTextLineEdit;
};

}
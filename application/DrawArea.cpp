#include "DrawArea.hpp"

App::DrawArea::DrawArea(_2d& inR, EventManager& inE, Window* inWindow, uint32_t inW, uint32_t inH)
    : Component::Area_base(inR, inE)
    , mNode__iAdd([](std::vector<DataType> in) { return std::get<int>(in[0]) + std::get<int>(in[1]); }, 2)
    , mNode__iSub([](std::vector<DataType> in) { return std::get<int>(in[0]) + std::get<int>(in[1]); }, 2)
    , mNode__iMul([](std::vector<DataType> in) { return std::get<int>(in[0]) + std::get<int>(in[1]); }, 2)
    , mNode__iDiv([](std::vector<DataType> in) { return std::get<int>(in[0]) + std::get<int>(in[1]); }, 2)
    , mNode__Int(0)
    , mNode__Float(0.0f)

{
    this->SetWindow(inWindow, inW, inH);
    this->SetDimension(glm::uvec2(100, 100));
    this->SetPosition(
        glm::uvec2(
            this->GetWindowWidth() - this->GetDimension().x - 10,
            this->GetWindowHeight() - this->GetDimension().y - 10));
    this->SetDepthValue(e.GetDepthValue());
    this->Load();

    r.bt.AddFontFace("font.ttf", 4, mFontFaceId);
    r.bt.SetCurrentFontFace(mFontFaceId);

    /*1. Setup Layouts*/
    mVerticalLayout = MakeUp<Component::VerticalLayout<2>>(r, e);
    mVerticalLayout->SetPosition(glm::vec2(0, 0));
    mVerticalLayout->SetDimension(glm::vec2(this->GetWindowWidth(), this->GetWindowHeight()));

    mLayout = MakeSH<Component::HorizontalLayout<2>>(r, e);
    mBottomHorizontalLayout = MakeSH<Component::HorizontalLayout<1>>(r, e);
    mVerticalLayout->AddComponent(mLayout);
    mVerticalLayout->AddComponent(mBottomHorizontalLayout);
    mVerticalLayout->ResetPositionsAndDimensions({ 0.8f, 0.2f });

    mBottomScrollArea = MakeSH<Component::ScrollableRect>(r, e);
    mBottomHorizontalLayout->AddComponent(mBottomScrollArea);
    mBottomHorizontalLayout->ResetPositionsAndDimensions({ 1.0f });

    e.MoveDepthValueTowardsTheCamera();
    mGridSheet = MakeSH<App::GridSheet>(r, e);
    mScrollArea = MakeSH<Component::ScrollableRect>(r, e);
    mLayout->AddComponent(mScrollArea);
    mLayout->AddComponent(mGridSheet);
    mLayout->ResetPositionsAndDimensions({ 0.2f, 0.8f });

    /*2. Specify Settings For Components*/
    mGridSheet->SetDepthValue(e.GetDepthValue());
    mGridSheet->SetWindow(mWindow, this->GetWindowWidth(), this->GetWindowHeight());
    mScrollArea->SetDepthValue(e.GetDepthValue());
    mScrollArea->SetWindow(mWindow, this->GetWindowWidth(), this->GetWindowHeight());
    mScrollArea->SetVerticalScrollBarHeigthRatio(0.1f);
    mBottomScrollArea->SetDepthValue(e.GetDepthValue());
    mBottomScrollArea->SetWindow(mWindow, this->GetWindowWidth(), this->GetWindowHeight());
    mBottomScrollArea->SetVerticalScrollBarHeigthRatio(0.01f);

    /*3. Load the Components*/
    mGridSheet->Load();
    mScrollArea->Load();
    mBottomScrollArea->Load();
    e.MoveDepthValueAwayFromTheCamera();
    mLayout->SetVPadding(0);
    mLayout->SetHPadding(5);
    mVerticalLayout->SetVPadding(5);
    mVerticalLayout->SetHPadding(0);
    mBottomHorizontalLayout->SetVPadding(0);
    mBottomHorizontalLayout->SetHPadding(5);


    mContextMenu = MakeUp<Component::ContextMenuList>(r, e);

    mContextMenu->SetMenuItemStrings(ContextMenuItems);
    mContextMenu->SetPosition(mGridSheet->FromComponent(glm::vec2(100, 100)));
    mTextButtonScrollTestPosition = mScrollArea->FromComponent(glm::vec2(100, 100));
    mContextMenu->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
    mContextMenu->SetDepthValue(e.GetDepthValue());
    mContextMenu->SetDimension({ 500, 500 });
    mContextMenu->Load();
}
#pragma once
#include "GridSheet.hpp"
#include <Components/Area_base.hpp>
#include <Components/BTextButtonRect.hpp>
#include <Components/ButtonRect_base.hpp>
#include <Components/Colors.hpp>
#include <Components/ContextMenuList.hpp>
#include <Components/HLayout.hpp>
#include <Components/ScrollableRect.hpp>
#include <Components/TextLineEdit.hpp>
#include <Components/VLayout.hpp>
#include "PropertySheet.hpp"

namespace App {
using namespace Jkr::Renderer;
class DrawArea : public Component::Area_base {
    using ab = Component::Area_base;
    Node mNode__iAdd;
    Node mNode__iSub;
    Node mNode__iMul;
    Node mNode__iDiv;
    Node mNode__Int;
    Node mNode__Float;

public:
    DrawArea(_2d& inR, EventManager& inE, Window* inWindow, uint32_t inW, uint32_t inH);

private:
    Sp<Component::ScrollableRect> mScrollArea;
    Sp<App::GridSheet> mGridSheet;
    Sp<Component::HorizontalLayout<2>> mLayout;
    Sp<Component::HorizontalLayout<1>> mBottomHorizontalLayout;
    Sp<App::PropertySheet> mBottomScrollArea;
    Up<Component::VerticalLayout<2>> mVerticalLayout;
    Up<Component::ContextMenuList> mContextMenu;
    uint32_t mFontFaceId;

public:
    void Update(Window* inWindow, uint32_t inW, uint32_t inH)
    {
        this->SetWindow(inWindow, inW, inH);
        this->SetPosition(
            glm::uvec2(
                this->GetWindowWidth() - this->GetDimension().x - 10,
                this->GetWindowHeight() - this->GetDimension().y - 10));

        mVerticalLayout->SetPosition(glm::vec2(0, 0));
        mVerticalLayout->SetDimension(glm::vec2(this->GetWindowWidth(), this->GetWindowHeight()));
        mVerticalLayout->ResetPositionsAndDimensions({ 0.8, 0.2 });

        mBottomHorizontalLayout->ResetPositionsAndDimensions({ 1.0f });
        mLayout->ResetPositionsAndDimensions({ 0.2, 0.8 });
        mScrollArea->Update(inWindow, this->GetWindowWidth(), this->GetWindowHeight());
        mBottomScrollArea->Update(inWindow, this->GetWindowWidth(), this->GetWindowHeight());
        mGridSheet->Update(inWindow, this->GetWindowWidth(), this->GetWindowHeight());

        if (mIsContextMenuVisible) {
            mContextMenu->SetPosition(mGridSheet->FromComponent(glm::vec2(100, 100)));
            mContextMenu->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
            mContextMenu->Update();
        }
    }

private:
    glm::vec2 mTextButtonScrollTestPosition;
    bool mIsContextMenuVisible = false;

public:
    std::vector<Node*> mNodesList = {
        &mNode__iAdd,
        &mNode__iSub,
        &mNode__iMul,
        &mNode__iDiv,
        &mNode__Int,
        &mNode__Float
    };
    std::vector<std::string_view> ContextMenuItems = {
        "mAdd",
        "mSub",
        "mMul",
        "mDiv",
        "tInt",
        "tFloat"
    };

    void AddNodeViewByIndex(uint32_t inIndex, glm::vec2 inPos, std::string_view inName)
    {
        mGridSheet->AddNodeView(*mNodesList[inIndex], inPos, inName);
    }
    void Event()
    {
        mScrollArea->Event();
        mGridSheet->Event();
        mBottomScrollArea->Event();
        mContextMenu->Event();
        bool isRightButtonPressed = (SDL_BUTTON(SDL_BUTTON_RIGHT) & e.GetMouseButtonValue()) != 0;

        auto ev = e.GetEventHandle();
        if (ev.type == SDL_KEYDOWN) {
            if (ev.key.keysym.sym == SDLK_RETURN or ev.key.keysym.sym == SDLK_ESCAPE) {
                mIsContextMenuVisible = false;
                mContextMenu->UnFocus();
            } else if (ev.key.keysym.sym == SDLK_SPACE) {
                mIsContextMenuVisible = true;
                mContextMenu->Focus();
            }
            if (ev.key.keysym.sym == SDLK_RETURN) {
                auto s = mContextMenu->GetSelectedItem();
                if (s.has_value()) {
                    AddNodeViewByIndex(s.value(), glm::vec2(0, 0), ContextMenuItems[s.value()]);
                }
            }
        }

        if (isRightButtonPressed and not mGridSheet->IsSelectionEmpty())
        {
            mBottomScrollArea->SetCurrentNodeView((*mGridSheet->GetSelection()).get());
        }

    }

    void Draw()
    {
        r.ln.Bind(*mWindow);
        mScrollArea->DrawOutlines();
        mBottomScrollArea->DrawOutlines();

        if (mIsContextMenuVisible) {
            mContextMenu->DrawOutlines();
        }

        /* GRIDSHEET */
        {
            mGridSheet->DrawOutline();
            mGridSheet->SetScissor();
            mGridSheet->DrawLines();
            mGridSheet->ResetScissor();
        }

        r.sh.BindShapes(*this->GetWindow());
        r.sh.BindFillMode(Jkr::Renderer::FillType::Fill, *this->GetWindow());
        if (mIsContextMenuVisible) {
            mContextMenu->DrawShapes();
        }
        mGridSheet->SetScissor();
        mGridSheet->DrawShapes();
        mGridSheet->ResetScissor();
        mScrollArea->DrawFillShapes();
        mBottomScrollArea->DrawFillShapes();

        r.sh.BindFillMode(Jkr::Renderer::FillType::Image, *this->GetWindow());
        mGridSheet->SetScissor();
        if (mIsContextMenuVisible)
            mContextMenu->DrawImages(*this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());

        r.bt.Bind(*this->GetWindow());
        if (mIsContextMenuVisible)
            mContextMenu->DrawBestTexts();
        mGridSheet->SetScissor();
        mGridSheet->DrawBestTexts();
        mBottomScrollArea->SetScissor();
        mBottomScrollArea->DrawBestTexts();
    }
};
}
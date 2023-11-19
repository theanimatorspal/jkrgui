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

namespace App {
using namespace Jkr::Renderer;
class DrawArea : public Component::Area_base {
    using ab = Component::Area_base;

public:
    DrawArea(_2d& inR, EventManager& inE, Window* inWindow, uint32_t inW, uint32_t inH);

private:
    Sp<Component::ScrollableRect> mScrollArea;
    Sp<App::GridSheet> mGridSheet;
    Sp<Component::HorizontalLayout<2>> mLayout;
    Sp<Component::HorizontalLayout<1>> mBottomHorizontalLayout;
    Sp<Component::ScrollableRect> mBottomScrollArea;
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
    void Event()
    {
        mScrollArea->Event();
        mGridSheet->Event();
        mBottomScrollArea->Event();
        mContextMenu->Event();

        auto ev = e.GetEventHandle();
        if (ev.type == SDL_KEYDOWN) {
            if (ev.key.keysym.sym == SDLK_RETURN or ev.key.keysym.sym == SDLK_ESCAPE) {
                mIsContextMenuVisible = false;
                mContextMenu->UnFocus();
            } else if (ev.key.keysym.sym == SDLK_SPACE ) {
                mIsContextMenuVisible = true;
                mContextMenu->Focus();
            }
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

        r.sh.BindShapes(*mWindow);
        r.sh.BindFillMode(Jkr::Renderer::FillType::Fill, *this->GetWindow());
        if (mIsContextMenuVisible) {
            mContextMenu->DrawShapes();
        }
        mScrollArea->DrawFillShapes();
        mBottomScrollArea->DrawFillShapes();

        r.sh.BindFillMode(Jkr::Renderer::FillType::Image, *this->GetWindow());
        if (mIsContextMenuVisible)
            mContextMenu->DrawImages(*this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());

        r.bt.Bind(*this->GetWindow());
        if (mIsContextMenuVisible)
            mContextMenu->DrawBestTexts();
    }
};
}
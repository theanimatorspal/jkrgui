#include "NodeView.hpp"
#include <Components/BTextButtonRect.hpp>
#include <Components/ScrollableRect.hpp>

namespace App {
using namespace Jkr::Component;
using namespace Jkr::Renderer;

class PropertySheet : public ScrollableRect {
public:
    using sr = ScrollableRect;
    PropertySheet(_2d& in2d, EventManager& inManager)
        : ScrollableRect(in2d, inManager)
    {
    }

    void Load()
    {
        sr::Load();

        mTextButtonRect = MakeUp<BTextButtonRect>(r, e);
        mTextButtonRect->SetText("Update");
        Reset();
        mTextButtonRect->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
        mTextButtonRect->SetPosition(this->FromComponent(mCurrentPosition));
        mTextButtonRect->SetDimension(mColumnDimension);
        e.MoveDepthValueTowardsTheCamera();
        mTextButtonRect->SetDepthValue(e.GetDepthValue());
        MoveDown();

        e.MoveDepthValueAwayFromTheCamera();

        mTextButtonRect->Load();
    }

    void Event()
    {
        sr::Event();
        if (mCurrentNodeView != nullptr) {
            if (mCurrentNodeView->GetInputCount() == 0 and mCurrentNodeView->IsHardSelected()) {
                mShouldShowTextButton = true;
            } else {
                mShouldShowTextButton = false;
            }
        } else {
            mShouldShowTextButton = false;
        }
    }

    void Update(Jkr::Window* inWindow, uint32_t inW, uint32_t inH)
    {
        sr::Update(inWindow, inW, inH);
        if (mShouldShowTextButton) {
            mTextButtonRect->SetWindow(inWindow, inW, inH);
            Reset();
            mTextButtonRect->SetPosition(this->FromComponent(mCurrentPosition));
            mTextButtonRect->SetPosition(mTextButtonRect->GetPosition() + this->GetScrollOffsetPosition());
            mTextButtonRect->Update();
        }
    }

    SETTER SetCurrentNodeView(NodeView* inNodeView)
    {
        mCurrentNodeView = inNodeView;
    }

    void DrawBestTexts()
    {
        if (mShouldShowTextButton) {
            mTextButtonRect->DrawBestTexts();
        }
    }

    void DrawFillShapes()
    {
        sr::DrawFillShapes();
        if (mShouldShowTextButton) {
            this->SetScissor();
            mTextButtonRect->DrawShapes();
        }
        this->ResetScissor();
    }

    void DrawOutlines()
    {
        sr::DrawOutlines();
    }

private:
    NodeView* mCurrentNodeView = nullptr;
    bool mShouldShowTextLineEdit = false;
    bool mShouldShowTextButton = false;

private:
    Up<BTextButtonRect> mTextButtonRect;
    glm::vec2 mCurrentPosition = glm::vec2(0, 0);

private:
    glm::vec2 mColumnDimension = { 142, 32 };
    uint32_t mGap = 10;

private:
    void MoveDown()
    {
        mCurrentPosition.y += mColumnDimension.y + mGap;
    }
    void Reset()
    {
        mCurrentPosition.y = mGap;
        mCurrentPosition.x = mGap;
    }
};

}

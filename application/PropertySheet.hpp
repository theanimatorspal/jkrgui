#include "NodeView.hpp"
#include <Components/BTextButtonRect.hpp>
#include <Components/ScrollableRect.hpp>
#include <Components/TextLineEdit.hpp>

namespace App {
using namespace Jkr::Component;
using namespace Jkr::Renderer;

class PropertySheet : public ScrollableRect {
public:
    using sr = ScrollableRect;
    PropertySheet(_2d& in2d, EventManager& inManager)
        : ScrollableRect(in2d, inManager)
    {
        mTextButtonRect = MakeUp<BTextButtonRect>(r, e);
        mTextLineEdit = MakeUp<TextLineEdit>(r, e);
        mTextButtonRect_Status = MakeUp<BTextButtonRect>(r, e);
    }

    void Load()
    {
        sr::Load();

        mTextButtonRect->SetText("Update");
        Reset();
        mTextButtonRect->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
        mTextButtonRect->SetPosition(this->FromComponent(mCurrentPosition));
        mTextButtonRect->SetDimension(mColumnDimension);
        e.MoveDepthValueTowardsTheCamera();
        mTextButtonRect->SetDepthValue(e.GetDepthValue());
        MoveDown();
        mTextLineEdit->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
        mTextLineEdit->SetPosition(this->FromComponent(mCurrentPosition));
        mTextLineEdit->SetDimension(mColumnDimension);
        mTextLineEdit->SetDepthValue(e.GetDepthValue());
        MoveDown();
        mTextButtonRect_Status->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
        mTextButtonRect_Status->SetPosition(this->FromComponent(mCurrentPosition));
        mTextButtonRect_Status->SetDimension(mColumnDimension);
        mTextButtonRect_Status->SetDepthValue(e.GetDepthValue());
        mTextButtonRect_Status->SetNormalColor(glm::vec4(0.3f, 0.3f, 0.3f, 0.3f));
        mTextButtonRect_Status->SetHoveredColor(glm::vec4(0.3f, 0.3f, 0.3f, 0.3f));
        mTextButtonRect_Status->SetClickedColor(glm::vec4(0.3f, 0.3f, 0.3f, 0.3f));
        mTextButtonRect_Status->SetText(mInitialStatusText);
        e.MoveDepthValueAwayFromTheCamera();

        mTextButtonRect->Load();
        mTextLineEdit->Load();
        mTextButtonRect_Status->Load();
    }
    void Event()
    {
        sr::Event();
        if (mCurrentNodeView != nullptr) {
            if ((mCurrentNodeView->GetInputCount() == 0) and mCurrentNodeView->IsHardSelected()) {
                mTextButtonRect->Event();
                mTextLineEdit->Event();
                mShowUpdateButton = true;
                mShouldShowTextLineEdit = true;
                mShowStatus = true;
            } else {
				mTextLineEdit->SetPosition(glm::vec2(std::numeric_limits<float>::max()));
                mTextButtonRect->Event();
                mShowUpdateButton = true;
                mShowStatus = true;
                mShouldShowTextLineEdit = false;
            }
        }

        if (mTextButtonRect->IsBeingPushed()) {
            this->UpdateNodeFromPropertySheet();
        }
    }
    void Update(Jkr::Window* inWindow, uint32_t inW, uint32_t inH)
    {
        sr::Update(inWindow, inW, inH);
		Reset();

        if (mShowUpdateButton) {
            mTextButtonRect->SetWindow(inWindow, inW, inH);
            mTextButtonRect->SetPosition(this->FromComponent(mCurrentPosition));
            mTextButtonRect->SetPosition(mTextButtonRect->GetPosition() + this->GetScrollOffsetPosition());
            mTextButtonRect->Update();
        }
        if (mShouldShowTextLineEdit) {
            MoveDown();
            mTextLineEdit->SetWindow(inWindow, inW, inH);
            mTextLineEdit->SetPosition(this->FromComponent(mCurrentPosition));
            mTextLineEdit->SetPosition(mTextLineEdit->GetPosition() + this->GetScrollOffsetPosition());
            mTextLineEdit->Update();
        }
        if (mShowStatus) {
            MoveDown();
            mTextButtonRect_Status->SetWindow(this->GetWindow(), this->GetWindowWidth(), this->GetWindowHeight());
            mTextButtonRect_Status->SetPosition(this->FromComponent(mCurrentPosition));
            mTextButtonRect_Status->SetDimension(mColumnDimension);
        }
    }
    void DrawBestTexts()
    {
        if (mShowUpdateButton)
            mTextButtonRect->DrawBestTexts();
        if (mShouldShowTextLineEdit)
            mTextLineEdit->DrawBestTexts();
        if (mShowStatus)
            mTextButtonRect_Status->DrawBestTexts();
    }
    void DrawFillShapes()
    {
        sr::DrawFillShapes();
        if (mShowUpdateButton) {
            this->SetScissor();
            mTextButtonRect->DrawShapes();
        }
        this->ResetScissor();
    }
    void DrawOutlines()
    {
        if (mShouldShowTextLineEdit) {
            mTextLineEdit->DrawOutlines();
        }
        sr::DrawOutlines();
    }

    void UpdateNodeFromPropertySheet()
    {
        using namespace std;
        if (mCurrentNodeView->GetInputCount() == 0) {
            if (holds_alternative<int>(mCurrentNodeView->GetOutput())) {
                auto current_value = mTextLineEdit->GetCurrentString();
                int value = stoi(current_value);
                mCurrentNodeView->SetOutputValue(value);
                mTextButtonRect_Status->SetText(to_string(value));
                mTextButtonRect_Status->Update();
            }
        } else {
            if (holds_alternative<int>(mCurrentNodeView->GetOutput())) {
                auto Value = mCurrentNodeView->GetOutput();
                mTextButtonRect_Status->SetText(to_string(get<int>(Value)));
                mTextButtonRect_Status->Update();
            }
        }
    }
    SETTER SetCurrentNodeView(NodeView* inNodeView) { mCurrentNodeView = inNodeView; }

private:
    NodeView* mCurrentNodeView = nullptr;
    bool mShouldShowTextLineEdit = false;
    bool mShowUpdateButton = false;
    bool mShowStatus = false;

private:
    Up<BTextButtonRect> mTextButtonRect;
    Up<TextLineEdit> mTextLineEdit;
    Up<BTextButtonRect> mTextButtonRect_Status;
    std::string mInitialStatusText = "Status";
    glm::vec2 mCurrentPosition = glm::vec2(0, 0);

private:
    glm::vec2 mColumnDimension = { 142, 32 };
    uint32_t mGap = 10;

private:
    void MoveDown() { mCurrentPosition.y += mColumnDimension.y + mGap; }
    void MoveRight() { mCurrentPosition.x += mColumnDimension.x + mGap; }
    void MoveUp() { mCurrentPosition.y -= (mColumnDimension.y + mGap); }
    void MoveLeft() { mCurrentPosition.x -= (mColumnDimension.x + mGap); }
    void Reset()
    {
        mCurrentPosition.y = mGap;
        mCurrentPosition.x = mGap;
    }
};

}

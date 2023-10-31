#include "NodeView.hpp"

void App::NodeView::PushConnection(Jkr::GUI2DRenderer& inR, Jkr::Font::FastTextRenderer& inFr, const Button& inCircleButton, DataType inDataType, Data inData)
{
	Node::PushConnection(inDataType, inData);
	Button b(inR, inFr);
	b.RegisterExisting<Button::Type::CircleInRectangle>(inCircleButton, 0, 0, 100, 100, glm::vec3(1.0f, 1.0f, 1.0f));
	mInputButtons.push_back(b);
}

App::NodeView& App::NodeView::Register(const Button& inRectButton, const Button& inCircleButton, Jkr::GUI2DRenderer& inR, Jkr::Font::FastTextRenderer& inFr)
{
	mRectTextButton = MakeUp<TextButton>(inR, inFr);
	mRectTextButton->RegisterExisting(inRectButton, mName.c_str(), 100, 100, 25);
	mOutputButton = MakeUp<Button>(inR, inFr);
	mOutputButton->RegisterExisting<Button::Type::CircleInRectangle>(inCircleButton, 0, 0, 100, 100, glm::vec3(1.0f, 1.0f, 1.0f));

	return *this;
}

void App::NodeView::Update(Jkr::GUI2DRenderer& inR, glm::vec2 inXy, uint32_t inDepthValue)
{
	mFeatures = mRectTextButton->UpdateExisting(false, mName.c_str(), glm::uvec2(inXy.x, inXy.y), 25);
	mOutputButton->UpdateExisting(inXy.x + mFeatures.mWidth / 2 - 5, inXy.y + 25, 20, 20, 3);
	for (int i = 0; i < mInputButtons.size(); i++)
	{
		mInputButtons[i].UpdateExisting(inXy.x + i * 20, inXy.y - 20, 20, 20, 3);
	}
}

void App::NodeView::Draw(Jkr::GUI2DRenderer& inR, uint32_t inWindowW, uint32_t inWindowH, const Jkr::GUIWindow& inWindow)
{
	glm::vec3 inputColor = mInputColor;
	glm::vec3 outputColor = mOutputColor;

	mRectTextButton->Draw(inWindow, inWindowW, inWindowH, mNodeRectColor, mNodeRectColor * 1.5f);

	if (GetCurrentSelectedButton() == -1) outputColor = { 1.0f, 0.0f, 0.0f };
	mOutputButton->Draw(inWindow, inWindowW, inWindowH, outputColor, outputColor * 2.0f);
	outputColor = mOutputColor;

	for (int i = 0; i < mInputButtons.size(); i++)
	{
		if (GetCurrentSelectedButton() == i) inputColor = { 1.0f, 0.0f,0.0f };
		mInputButtons[i].Draw(inWindow, inWindowW, inWindowH, inputColor, inputColor * 2.0f);
		inputColor = mInputColor;
	}
}

void App::NodeView::EventCall(Jkr::GUIWindow& inWindow, std::any inEventInfo)
{
	mRectTextButton->EventCall(inWindow, inEventInfo);
	mOutputButton->EventCall(inWindow, inEventInfo);

	for (int i = 0; i < mInputButtons.size(); i++)
	{
		mInputButtons[i].EventCall(inWindow, inEventInfo);
	}

	for (int i = 0; i < mInputButtons.size(); i++)
	{
		if (mInputButtons[i].GetIsButtonPushed()) {
			mCurrentSelectedButton = i;
		}
	}

	if (mOutputButton->GetIsButtonPushed()) mCurrentSelectedButton = -1;
}


#include "FastTextButton.hpp"

void Jkr::Component::FastTextButton::LoadText(const std::string_view inString)
{
	mDimensions = r.ft.AddText(
		inString,
		Button::GetPosition().x + mPadding.x,
		Button::GetPosition().y + mPadding.y,
		Button::GetDepthValue(),
		mTextId
	);

	Button::SetDimension(
		glm::uvec2(
			mPadding.x * 2 + mDimensions.mWidth,
			mPadding.y * 2 + mDimensions.mHeight
		)
	);
	mNoOfChars = inString.size();
}

void Jkr::Component::FastTextButton::DrawTxt(Window& inWindow, uint32_t inW, uint32_t inH, glm::mat4& inMatrix)
{
	r.ft.Draw(inWindow, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), inW, inH, mTextId, mNoOfChars, inMatrix);
}

void Jkr::Component::FastTextButton::UpdateText(const std::string_view inString)
{
	r.ft.UpdateText(mTextId, inString,
		Button::GetPosition().x + mPadding.x,
		Button::GetPosition().y + mPadding.y,
		Button::GetDepthValue()
	);
}

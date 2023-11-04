#include "FastTextButton.hpp"

void Jkr::Component::FastTextButton::LoadText(const std::string_view inString)
{
	e.MoveDepthValueTowardsTheCamera();
	mDimensions = r.ft.AddText(
		inString,
		Button::GetPos().x + mPadding.x,
		Button::GetPos().y + mPadding.y,
		e.GetDepthValue(),
		mTextId
	);
	e.MoveDepthValueAwayFromTheCamera();

	Button::SetDimensions(
		glm::uvec2(
			mPadding.x * 2 + mDimensions.mWidth,
			mPadding.y * 2 + mDimensions.mHeight
		)
	);
	mNoOfChars = inString.size();
}

void Jkr::Component::FastTextButton::DrawText(Window& inWindow, uint32_t inW, uint32_t inH, glm::mat4& inMatrix)
{
	r.ft.Draw(inWindow, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), inW, inH, mTextId, mNoOfChars, inMatrix);
}

void Jkr::Component::FastTextButton::UpdateText(const std::string_view inString)
{
	r.ft.UpdateText(mTextId, inString,
		Button::GetPos().x + mPadding.x,
		Button::GetPos().y + mPadding.y,
		Button::GetDepthLevel()
	);
}

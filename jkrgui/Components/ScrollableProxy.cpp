#include "ScrollableProxy.hpp"

void Jkr::Component::ScrollableProxy::BeginDraw()
{
	mVerticalButton->BeginDraw();
}

void Jkr::Component::ScrollableProxy::EndDraw()
{
	mVerticalButton->EndDraw();
}

void Jkr::Component::ScrollableProxy::Load(uint32_t inW, uint32_t inH)
{
	Jkr::Generator Rect(Jkr::Shapes::Rectangle, cb::GetDimension());
	r.sh.Add(Rect, cb::GetPosition().x, cb::GetPosition().y, cb::GetDepthValue(), mBackgroundShapeId);
	mVerticalButton = MakeUp<Button>(r, e);
	auto VerticalScrollBarPosition = cb::GetPosition() + glm::vec2(cb::GetDimension().x, 0) - glm::vec2(width, 0);
	e.MoveDepthValueTowardsTheCamera();
	Jkr::Generator RectL(Jkr::Shapes::Rectangle, glm::uvec2(width, cb::GetDimension().y));
	r.sh.Add(RectL, VerticalScrollBarPosition.x, VerticalScrollBarPosition.y, e.GetDepthValue(), mVerticalScrollBarBackgroundShapeId);

	e.MoveDepthValueTowardsTheCamera();
	mVerticalButton->SetDepthValue(e.GetDepthValue());
	mVerticalButton->SetPosition(VerticalScrollBarPosition);
	mVerticalButton->SetDimension(glm::vec2(width, height));
	e.MoveDepthValueAwayFromTheCamera();

	e.MoveDepthValueAwayFromTheCamera();

	mVerticalButton->LoadShape();
}

void Jkr::Component::ScrollableProxy::Update(uint32_t inW, uint32_t inH)
{
}

void Jkr::Component::ScrollableProxy::DrawLines(Window& inWindow, uint32_t inW, uint32_t inH)
{
}

void Jkr::Component::ScrollableProxy::DrawShapes(Window& inWindow, uint32_t inW, uint32_t inH)
{
	r.sh.Draw(inWindow, mBackgroundColor, inW, inH, mBackgroundShapeId, mBackgroundShapeId, mTransform);
	mVerticalButton->DrawShape(inWindow, inW, inH, mTransform);
	r.sh.Draw(inWindow, mScrollBarBackgroundColor, inW, inH, mVerticalScrollBarBackgroundShapeId, mVerticalScrollBarBackgroundShapeId, mTransform);
}

void Jkr::Component::ScrollableProxy::Dispatch()
{
}

void Jkr::Component::ScrollableProxy::Event(Window& inWindow, uint32_t inW, uint32_t inH)
{
	mVerticalButton->Event();
	int numKeys;
	auto keystate = SDL_GetKeyboardState(&numKeys);

	bool isLeftButtonPressed = (SDL_BUTTON(SDL_BUTTON_LEFT) & e.GetMouseButtonValue()) != 0;
	bool isVerticalButtonPushed = mVerticalButton->IsPushed();

	bool shouldScroll = isLeftButtonPressed && isVerticalButtonPushed;

	if (shouldScroll) ScrollBarBeingScrolled = true;
	if (!isLeftButtonPressed) ScrollBarBeingScrolled = false;
	if (ScrollBarBeingScrolled || shouldScroll)
	{
		auto& vButtonPos = mVerticalButton->GetPosition();
		auto& vButtonDim = mVerticalButton->GetDimension();
		auto& cbPos = cb::GetPosition();
		auto& cbDim = cb::GetDimension();
		vButtonPos.y += e.GetRelativeMousePos().y;
#undef max
		vButtonPos.y = std::max(cbPos.y, std::min(vButtonPos.y, cbPos.y + cbDim.y - vButtonDim.y));
	}

}


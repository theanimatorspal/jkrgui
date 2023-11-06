#include "GridSheet.hpp"
using namespace Jkr::Component;



void GridSheet::Load(uint32_t inW, uint32_t inH)
{
	mDepthValue = e.GetDepthValue();
	e.MoveDepthValueTowardsTheCamera();
	e.SetBoundedRect(glm::uvec2(0, 0), glm::uvec2(100, 100), mDepthValue);
	const glm::uvec2 origin = { 0, 0 };
	mGridStartId = r.ln.GetCurrentNewLineId();

	int gW = 500 * mNumLineFactor;
	int gH = 500 * mNumLineFactor;
	ToWc towc(inW, inH);

	for (int i = -gW; i <= gW; i += mGridSpacing) {
		r.ln.AddLine(
			towc(glm::vec2(i, -gH)),
			towc(glm::vec2(i, gH)),
			mDepthValue,
			mGridEndId
		);
	}

	for (int i = -gH; i <= gH; i += mGridSpacing) {
		r.ln.AddLine(
			towc(glm::vec2(-gW, i)),
			towc(glm::vec2(gW, i)),
			mDepthValue,
			mGridEndId
		);
	}

	r.ln.AddLine(
		towc(glm::vec2(0, -gH)),
		towc(glm::vec2(0, gH)),
		mDepthValue,
		mGridMainLinesId
	);

	uint32_t id;
	r.ln.AddLine(
		towc(glm::vec2(-gW, 0)),
		towc(glm::vec2(gW, 0)),
		mDepthValue, id);

	Jkr::Generator Rect(Jkr::Shapes::Rectangle, glm::uvec2(5, 5));
	auto xy = towc(glm::vec2(0, 0));
	r.sh.Add(Rect, xy.x, xy.y, mDepthValue, mCenterIndicatorId);
	e.MoveDepthValueTowardsTheCamera();
}

void GridSheet::Update(uint32_t inW, uint32_t inH)
{
}

void GridSheet::DrawLines(Window& inWindow, uint32_t inW, uint32_t inH)
{
	r.ln.Draw(inWindow, glm::vec4(1.0f, 1.0f, 1.0f, 0.04f), inW, inH, mGridStartId, mGridEndId, mTmatrix);
	r.ln.Draw(inWindow, glm::vec4(1.0f, 1.0f, 1.0f, 0.2f), inW, inH, mGridMainLinesId, mGridMainLinesId + 1, mTmatrix);
}

void GridSheet::DrawShapes(Window& inWindow, uint32_t inW, uint32_t inH)
{
	r.sh.Draw(inWindow, glm::vec4(1.0f, 1.0f, 1.0f, 0.5f), inW, inH, mCenterIndicatorId, mCenterIndicatorId, mTmatrix);
}

void GridSheet::Dispatch()
{

}

void GridSheet::Event(Window& inWindow, uint32_t inW, uint32_t inH)
{
	TogWc togwc(inW, inH);
	ToWc towc(inW, inH);
	int numKeys;
	auto keystate = SDL_GetKeyboardState(&numKeys);

	int delx, dely;
	// auto mousestate = SDL_GetRelativeMouseState(&delx, &dely);
	auto delxy = e.GetRelativeMousePos();

	bool isAltKeyPressed = keystate[SDL_SCANCODE_LALT];
	bool isLeftButtonPressed = (SDL_BUTTON(SDL_BUTTON_LEFT) & e.GetMouseButtonValue()) != 0;

	if (isLeftButtonPressed && isAltKeyPressed)
	{
		mOffset2D[0] += delxy.x;
		mOffset2D[1] += delxy.y;
	}
}

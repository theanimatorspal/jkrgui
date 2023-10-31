#include "Button.hpp"

void Button::EventCall(Jkr::GUIWindow& inWindow, std::any inEvent)
{
	auto Event = std::any_cast<SDL_Event*>(inEvent);
	struct {
		int x, y;
	} Mouse;
	auto MouseState = SDL_GetMouseState(&Mouse.x, &Mouse.y);
	struct {
		int w, h;
	} Window;
	auto wd = inWindow.GetWindowSize();
	Window.w = wd.first;
	Window.h = wd.second;

	{
		auto x1 = mButtonMinMax[0];
		auto x2 = mButtonMinMax[2];
		auto y1 = mButtonMinMax[1];
		auto y2 = mButtonMinMax[3];
		bool CursorInsideButton =
			((x1 < Mouse.x) && (x2 > Mouse.x)) &&
			((y1 < Mouse.y) && (y2 > Mouse.y));
		if (CursorInsideButton)
		{
			isBeingHovered = true;
			if ((SDL_BUTTON(MouseState) == 1))
			{
				isButtonPushed = true;
			}
			else {
				isButtonPushed = false;
			}
		}
		else {
			isBeingHovered = false;
			isButtonPushed = false;
		}
	}

}

void Button::Update(uint32_t inX, uint32_t inY, uint32_t inW, uint32_t inH, uint32_t inDepthIndex)
{
	mRenderer.UpdateDynamicQuad(mButtonId, inX, inY, inW, inH, inDepthIndex);
	mButtonMinMax = glm::vec4(inX, inY, inW + inX, inY + inH);
}

void Button::UpdateExisting(uint32_t inX, uint32_t inY, uint32_t inW, uint32_t inH, uint32_t inDepthIndex)
{
	mRenderer.UpdateDynamicQuadWithExistingTexture(mButtonId, inX, inY, inW, inH, inDepthIndex);
	mButtonMinMax = glm::vec4(inX, inY, inW + inX, inY + inH);
}

template<>
void Button::Dispatch<Button::Type::Rectangle>(glm::vec3 inColorNotHovered, glm::vec3 inColorHovered)
{
	if (isBeingHovered && mTheColorIsDefault)
	{
		mRenderer.DrawToQuadTextureDispatch(
			mButtonId,
			Jkr::GUI2DRenderer::Shape::Rectangle,
			glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
			glm::vec4(inColorHovered, 1.0f),
			glm::vec4(0.5f)
		);
		mTheColorIsDefault = false;
	}
	else if (!mTheColorIsDefault && !isBeingHovered) {
		mRenderer.DrawToQuadTextureDispatch(
			mButtonId,
			Jkr::GUI2DRenderer::Shape::Rectangle,
			glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
			glm::vec4(inColorNotHovered, 1.0f),
			glm::vec4(0.5f)
		);
		mTheColorIsDefault = true;
	}
}

template<>
void Button::Dispatch<Button::Type::CircleInRectangle>(glm::vec3 inColorNotHovered, glm::vec3 inColorHovered)
{
	if (isBeingHovered && mTheColorIsDefault)
	{
		mRenderer.DrawToQuadTextureDispatch(
			mButtonId,
			Jkr::GUI2DRenderer::Shape::Circle,
			glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
			glm::vec4(inColorHovered, 1.0f),
			glm::vec4(0.5f)
		);
		mTheColorIsDefault = false;
	}
	else if (!mTheColorIsDefault && !isBeingHovered) {
		mRenderer.DrawToQuadTextureDispatch(
			mButtonId,
			Jkr::GUI2DRenderer::Shape::Circle,
			glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
			glm::vec4(inColorNotHovered, 1.0f),
			glm::vec4(0.5f)
		);
		mTheColorIsDefault = true;
	}

}

bool Button::DecideFinalColor(glm::vec3& FinalColor, const glm::vec3& inColorHovered, const glm::vec3& inColorNotHovered)
{
}


void Button::Draw(const Jkr::GUIWindow& inWindow, uint32_t inWindowW, uint32_t inWindowH, glm::vec3 inColorNotHovered, glm::vec3 inColorHovered)
{
	if (isBeingHovered)
	{
		mRenderer.DrawQuadWithExistingTextureCoordinate(mButtonId, inColorHovered, inWindowW, inWindowH, inWindow);
		mTheColorIsDefault = false;
	}
	else {
		mRenderer.DrawQuadWithExistingTextureCoordinate(mButtonId, inColorNotHovered, inWindowW, inWindowH, inWindow);
		mTheColorIsDefault = true;
	}
}


template<>
void Button::Register<Button::Type::Rectangle>(uint32_t inX, uint32_t inY, uint32_t inW, uint32_t inH, glm::vec3 inColor, float inRoundedRectRadius)
{
	mRenderer.RegisterDynamicQuad(mButtonId, [=](Jkr::GUI2DRenderer& inG, ksai::kstd::ui32 inId)
		{
			inG.DrawToQuadTextureDispatch(
				inId, Jkr::GUI2DRenderer::Shape::Fill,
				glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
				glm::vec4(inColor.x, inColor.y, inColor.z, 1.0f),
				glm::vec4(inRoundedRectRadius)
			);
		}, inW, inH
	);
	mButtonMinMax = glm::vec4(inX, inY, inW + inX, inY + inH);
}

template<>
void Button::Register<Button::Type::CircleInRectangle>(uint32_t inX, uint32_t inY, uint32_t inW, uint32_t inH, glm::vec3 inFillColor, glm::vec3 inCircleFillColor, float inRoundedRectRadius, float inCircleRadius)
{
	mRenderer.RegisterDynamicQuad(mButtonId, [=](Jkr::GUI2DRenderer& inG, ksai::kstd::ui32 inId)
		{
			inG.DrawToQuadTextureDispatch(
				inId, Jkr::GUI2DRenderer::Shape::Rectangle,
				glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
				glm::vec4(inFillColor.x, inFillColor.y, inFillColor.z, 1.0f),
				glm::vec4(inRoundedRectRadius)
			);
			inG.DrawToQuadTextureDispatch(
				inId, Jkr::GUI2DRenderer::Shape::Circle,
				glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
				glm::vec4(inCircleFillColor.x, inCircleFillColor.y, inCircleFillColor.z, 1.0f),
				glm::vec4(inCircleRadius)
			);
		}, inW, inH
	);
	mButtonMinMax = glm::vec4(inX, inY, inW + inX, inY + inH);
}

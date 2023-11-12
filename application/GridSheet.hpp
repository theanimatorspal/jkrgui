#pragma once
#include <Components/Area_base.hpp>

namespace App
{
	using namespace Jkr::Component;
	class GridSheet : public Area_base
	{
		using ab = Area_base;
	public:
		GridSheet(_2d& inR, EventManager& inE) : Area_base(inR, inE) { }
		void Load()
		{
			ab::Load();
			e.SetBoundedRect(this->GetPosition(), this->GetDimension(), this->GetDepthValue());
			auto mDepthValue = this->GetDepthValue();
			e.MoveDepthValueTowardsTheCamera();
			mBoundedRectId = e.SetBoundedRect(glm::uvec2(0, 0), glm::uvec2(100, 100), mDepthValue);
			const glm::uvec2 origin = { 0, 0 };
			mGridStartId = r.ln.GetCurrentNewLineId();

			int gW = 500 * mNumLineFactor;
			int gH = 500 * mNumLineFactor;


			for (int i = -gW; i <= gW; i += mGridSpacing) {
				r.ln.AddLine(
					glm::vec2(i, -gH),
					glm::vec2(i, gH),
					mDepthValue,
					mGridEndId
				);
			}

			for (int i = -gH; i <= gH; i += mGridSpacing) {
				r.ln.AddLine(
					glm::vec2(-gW, i),
					glm::vec2(gW, i),
					mDepthValue,
					mGridEndId
				);
			}

			r.ln.AddLine(
				glm::vec2(0, -gH),
				glm::vec2(0, gH),
				mDepthValue,
				mGridMainLinesId
			);

			uint32_t id;
			r.ln.AddLine(
				glm::vec2(-gW, 0),
				glm::vec2(gW, 0),
				mDepthValue, id);

			Jkr::Generator Rect(Jkr::Shapes::Rectangle, glm::uvec2(5, 5));
			auto xy = (glm::vec2(0, 0));
			r.sh.Add(Rect, xy.x, xy.y, mDepthValue, mCenterIndicatorId);
			e.MoveDepthValueTowardsTheCamera();
			mLinesTranslation = glm::identity<glm::mat4>();
			mLinesTranslation = glm::translate(mLinesTranslation, glm::vec3(ab::GetPosition() + ab::GetDimension()/2.0f, 0.0f));
		}

		constexpr void DrawOutline() {
			r.ln.Draw(*mWindow, glm::vec4(1.0f, 1.0f, 1.0f, 0.04f), ab::GetWindowWidth(), ab::GetWindowHeight(), this->GetLines()[0].x, this->GetLines()[0].y, ab::GetTranslationMatrix());
		}
		constexpr void SetScissor()
		{
			mWindow->SetScissor(this->GetScissor());
		}
		constexpr void DrawLines()
		{
			r.ln.Draw(*mWindow, glm::vec4(1.0f, 1.0f, 1.0f, 0.04f), ab::GetWindowWidth(), ab::GetWindowHeight(), mGridStartId, mGridEndId, mLinesTranslation);
			r.ln.Draw(*mWindow, glm::vec4(1.0f, 1.0f, 1.0f, 0.2f), ab::GetWindowWidth(), ab::GetWindowHeight(), mGridMainLinesId, mGridMainLinesId + 1, mLinesTranslation);
		}
		constexpr void ResetScissor()
		{
			mWindow->ResetScissor();
		}
		void Update(Window* inWindow, uint32_t inW, uint32_t inH)
		{
			ab::Update();
			this->SetWindow(inWindow, inW, inH);
		}

		void Event()
		{
			Jkr::BoundRect2D Rect{ .mXy = ab::GetPosition(), .mWh = ab::GetDimension() };
			e.UpdateBoundRect(ab::GetDepthValue(), mBoundedRectId, Rect);
			int numKeys;
			auto keystate = SDL_GetKeyboardState(&numKeys);

			int delx, dely;
			// auto mousestate = SDL_GetRelativeMouseState(&delx, &dely);
			auto delxy = e.GetRelativeMousePos();

			bool isAltKeyPressed = keystate[SDL_SCANCODE_LALT];
			bool isLeftButtonPressed = (SDL_BUTTON(SDL_BUTTON_LEFT) & e.GetMouseButtonValue()) != 0;

			if (isLeftButtonPressed && isAltKeyPressed && e.IsMouseWithinAtTopOfStack(mBoundedRectId, ab::GetDepthValue()))
			{
				glm::vec2 Offset2D(delxy.x, delxy.y);
				mLinesTranslation = glm::translate(mLinesTranslation, glm::vec3(Offset2D, 0.0f));
			}

			if (e.GetEventHandle().type == SDL_WINDOWEVENT_RESIZED || e.GetEventHandle().type == SDL_WINDOWEVENT_MAXIMIZED || e.GetEventHandle().type == SDL_WINDOWEVENT_MINIMIZED)
			{
				mLinesTranslation = glm::identity<glm::mat4>();
				//mLinesTranslation = glm::translate(mLinesTranslation, glm::vec3(ab::GetDimension() / 2.0f, 0.0f));
			}
		}

	private:
		uint32_t mGridSpacing = 10;
		uint32_t mNumLineFactor = 1;
		uint32_t mGridStartId;
		uint32_t mGridEndId;
		uint32_t mGridMainLinesId;
		uint32_t mCenterIndicatorId;
		uint32_t mBoundedRectId;
		glm::mat4 mLinesTranslation = glm::identity<glm::mat4>();
	protected:
		class ToWc
		{
		public:
			constexpr ToWc(uint32_t inW, uint32_t inH) : mW(inW), mH(inH) {}
			constexpr glm::vec2 operator()(glm::vec2 inC) {
				return inC + glm::vec2(mW / 2.f, mH / 2.f);
			}
		private:
			uint32_t mW, mH;
		};
		class TogWc
		{
		public:
			constexpr TogWc(uint32_t inW, uint32_t inH) : mW(inW), mH(inH) {}
			constexpr glm::vec2 operator()(glm::vec2 inC) {
				return inC - glm::vec2(mW / 2.f, mH / 2.f);
			}
		private:
			uint32_t mW, mH;
		};
	};
}
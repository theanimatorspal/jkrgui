#pragma once
#include <Components/Area_base.hpp>
#include <Components/ButtonRect_base.hpp>
#include <Components/Colors.hpp>

namespace App
{
	using namespace Jkr::Renderer;
	class DrawArea : public Component::Area_base
	{
		using ab = Component::Area_base;
	public:
		DrawArea(_2d& inR, EventManager& inE) : Component::Area_base(inR, inE)
		{
			this->SetDimension(glm::uvec2(100, 100));
			this->SetPosition(
				glm::uvec2(
					this->GetWindowWidth() - this->GetDimension().x - 10,
					this->GetWindowHeight()  - this->GetDimension().y - 10
				)
			);
			this->SetDepthValue(e.GetDepthValue());
			this->Load();

			e.MoveDepthValueTowardsTheCamera();
			mButton = MakeUp<Component::ButtonRect_base>(r, e);
			mButton->SetDimension(glm::uvec2(50, 10));
			mButton->SetPosition(this->ToComponent(glm::vec2(5, 5)));
			mButton->SetDepthValue(e.GetDepthValue());
			mButton->Load();

		}
	private:
		Up<Component::ButtonRect_base> mButton;
	public:

		void Update(Window* inWindow, uint32_t inW, uint32_t inH)
		{
			this->SetWindow(inWindow, inW, inH);
			this->SetPosition(
				glm::uvec2(
					this->GetWindowWidth() - this->GetDimension().x - 10, 
					this->GetWindowHeight() - this->GetDimension().y - 10
				)
			);
			mButton->SetPosition(this->ToWindow(glm::vec2(-10, 5)));
		}

		void Draw()
		{
			r.ln.Bind(*mWindow);
			glm::uvec2 BatchLines = { 0, 0 };
			BatchLines[1] = ab::GetLines().empty() ? 0 : ab::GetLines()[0].y;
			r.ln.Draw(*mWindow, glm::vec4(color_WHITESMOKE, 1.0), ab::GetWindowWidth(), ab::GetWindowHeight(), BatchLines[0], BatchLines[1], this->GetTranslationMatrix());
			
			r.sh.BindShapes(*mWindow);
			r.sh.BindFillMode(Jkr::Renderer::FillType::Fill, *mWindow);
			for (auto u : mButton->GetShapes())
			{
				mWindow->SetScissor(ab::GetScissor());
				r.sh.Draw(*mWindow, mButton->GetColor(), ab::GetWindowWidth(), ab::GetWindowHeight(), u.x, u.y, mButton->GetTranslationMatrix());
				mWindow->ResetScissor();
			}
		}

	};
}
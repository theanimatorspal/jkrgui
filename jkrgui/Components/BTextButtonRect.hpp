#pragma once
#include "ButtonRect_base.hpp"

namespace Jkr::Component
{
	class BTextButtonRect : public ButtonRect_base
	{
		using bb = ButtonRect_base;
	public:
		BTextButtonRect(_2d& inR, EventManager& inE) : ButtonRect_base(inR, inE) 
		{ }
		void Load()
		{
			auto TextDimensions = r.bt.AddText(mString, 0, 0, bb::GetDepthValue() - 1, mId, mLength);
			bb::SetDimension(glm::vec2(TextDimensions.mWidth + 2 * mHPadding, TextDimensions.mHeight + 2 * mVPadding));
			r.bt.UpdateText(mId, mString, mHPadding, mVPadding, bb::GetDepthValue() - 1);
			ButtonRect_base::Load();
		}

		void DrawBestTexts()
		{
			r.bt.Draw(*bb::GetWindow(), glm::vec4(1.0f, 1.0f, 0.9f, 1.0f), bb::GetWindowWidth(), bb::GetWindowHeight(), mId, mLength, bb::GetTranslationMatrix());
		}

		void DrawShapes()
		{
			const auto Shape = bb::GetShapes()[0];
			r.sh.Draw(*bb::GetWindow(), bb::GetColor(), bb::GetWindowWidth(), bb::GetWindowHeight(), Shape[0], Shape[1], bb::GetTranslationMatrix());
		}

		SETTER SetText(std::string_view inString) { mString = inString; }
	private:
		std::string_view mString;
		uint32_t mLength;
		uint32_t mId;
	private:
		uint32_t mVPadding = 5;
		uint32_t mHPadding = 5;

	};
}

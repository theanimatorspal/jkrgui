#include "Button.hpp"

namespace Jkr::Component
{
	using namespace Jkr::Renderer;
	class FastTextButton : public Button
	{
	public:
		FastTextButton(_2d& inRenderer2D, EventManager& inManager, const std::string_view inString, glm::uvec2 inPadding, glm::uvec2 inPos, uint32_t inDepthValue) : Button(inRenderer2D, inManager)
		{
			Button::SetPosition(inPos);
			Button::SetDepthValue(inDepthValue);
			SetPadding(inPadding);
			LoadText(inString);
			Button::LoadShape();
		}
		void LoadText(const std::string_view inString);
		void DrawTxt(Window& inWindow, uint32_t inW, uint32_t inH, glm::mat4& inMatrix);
		void UpdateText(const std::string_view inString);
		SETTER SetPadding(glm::uvec2 inPadding) { mPadding = inPadding; };
	private:
		FastText::TextDimensions mDimensions;
		glm::uvec2 mPadding;
		uint32_t mTextId;
		uint32_t mNoOfChars;
	};
};
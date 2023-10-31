#include "TextButton.hpp"

ksai::Font::FastText::TextFeatures TextButton::Register(const char* inString, uint32_t inX, uint32_t inY, uint32_t inH, glm::vec3 inColor, uint32_t inPaddingX, uint32_t inPaddingY, uint32_t inTextDepthValue)
{
	auto Width = mTextRenderer.GetTextFeatures(inString, 0.5f).mWidth;
	mTextRenderer.RegisterDynamicText(
		50, mTextId
	);

	Button::Register<Button::Type::Rectangle>(inX, inY, 1, 1, inColor, 0.0f);
	mPosDimenOfContainer = glm::ivec4(inX, inY, Width + inPaddingX * 2, inH);
	mPosDimenOfText = glm::ivec4(inX + inPaddingX, inY + inPaddingY, 0, 0);
	mDepthValue = inTextDepthValue + 1;
	mString = inString;
	return ksai::Font::FastText::TextFeatures{ .mWidth = Width + inPaddingX * 2, .mHeight = 0.0 };
}

ksai::Font::FastText::TextFeatures TextButton::RegisterExisting(const Button& inButton, const char* inString, uint32_t inX, uint32_t inY, uint32_t inH, glm::vec3 inColor, uint32_t inPaddingX, uint32_t inPaddingY, uint32_t inTextDepthValue)
{
	auto Width = mTextRenderer.GetTextFeatures(inString, 0.5f).mWidth;
	mTextRenderer.RegisterDynamicText(
		50, mTextId
	);

	Button::RegisterExisting<Button::Type::Rectangle>(inButton, inX, inY, 1, 1, inColor, 0.0f);
	mPosDimenOfContainer = glm::ivec4(inX, inY, Width + inPaddingX * 2, inH);
	mPosDimenOfText = glm::ivec4(inX + inPaddingX, inY + inPaddingY, 0, 0);
	mDepthValue = inTextDepthValue + 1;
	mString = inString;
	return ksai::Font::FastText::TextFeatures{ .mWidth = Width + inPaddingX * 2, .mHeight = 0.0 };
}

void TextButton::Update(bool inShouldHide)
{
	if (!inShouldHide)
	{
		mTextRenderer.UpdateDynamicText(mTextId,
			mString.c_str(), mPosDimenOfText[0], mPosDimenOfText[1],
			Jkr::Font::FastTextRenderer::FeatureFlags{
				.h = Jkr::Font::FastTextRenderer::HorizontalAlignment::Left,
				.inSizeFactor = 0.5f,
				.v = Jkr::Font::FastTextRenderer::VerticalAlignment::Bottom
			}, mDepthValue - 1);
		Button::Update(mPosDimenOfContainer[0], mPosDimenOfContainer[1], mPosDimenOfContainer[2], mPosDimenOfContainer[3], mDepthValue);
	}
	else {
		mTextRenderer.UpdateDynamicText(mTextId,
			mString.c_str(), 0, 0,
			Jkr::Font::FastTextRenderer::FeatureFlags{
				.h = Jkr::Font::FastTextRenderer::HorizontalAlignment::Left,
				.inSizeFactor = std::numeric_limits<float>::max(),
				.v = Jkr::Font::FastTextRenderer::VerticalAlignment::Bottom
			}, mDepthValue - 1);
		Button::Update(0, 0, 0, 0, mDepthValue);
	}

}

ksai::Font::FastText::TextFeatures TextButton::UpdateExisting(bool inShouldHide, const std::string_view inString, glm::uvec2 inPosition, uint32_t inH, uint32_t inPaddingX, uint32_t inPaddingY, uint32_t inTextDepthValue)
{
	auto Features = mTextRenderer.GetTextFeatures(inString.data(), 0.5f);
	auto Width = Features.mWidth;
	mPosDimenOfContainer = glm::ivec4(inPosition[0], inPosition[1], Width + inPaddingX * 2, inH);
	mPosDimenOfText = glm::ivec4(inPosition[0] + inPaddingX, inPosition[1] + inPaddingY, 0, 0);

	if (!inShouldHide)
	{
		mTextRenderer.UpdateDynamicText(mTextId,
			inString.data(), mPosDimenOfText[0], mPosDimenOfText[1],
			Jkr::Font::FastTextRenderer::FeatureFlags{
				.h = Jkr::Font::FastTextRenderer::HorizontalAlignment::Left,
				.inSizeFactor = 0.5f,
				.v = Jkr::Font::FastTextRenderer::VerticalAlignment::Bottom
			}, mDepthValue - 1);
		Button::UpdateExisting(mPosDimenOfContainer[0], mPosDimenOfContainer[1], mPosDimenOfContainer[2], mPosDimenOfContainer[3], mDepthValue);
	}
	else {
		mTextRenderer.UpdateDynamicText(mTextId,
			inString.data(), 0, 0,
			Jkr::Font::FastTextRenderer::FeatureFlags{
				.h = Jkr::Font::FastTextRenderer::HorizontalAlignment::Left,
				.inSizeFactor = std::numeric_limits<float>::max(),
				.v = Jkr::Font::FastTextRenderer::VerticalAlignment::Bottom
			}, mDepthValue - 1);
		Button::UpdateExisting(0, 0, 0, 0, mDepthValue);
	}
	return Features;
}

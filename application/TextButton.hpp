#pragma once
#include "Button.hpp"

class TextButton : public Button
{
public:
	TextButton(Jkr::GUI2DRenderer& inRenderer, Jkr::Font::FastTextRenderer& inTextRenderer) : Button(inRenderer, inTextRenderer) {}
	ksai::Font::FastText::TextFeatures Register(const char* inString, uint32_t inX, uint32_t inY, uint32_t inH = 0, glm::vec3 inColor = glm::vec3(0.0f), uint32_t inPaddingX = 5, uint32_t inPaddingY = 3, uint32_t inTextDepthValue = 1);
	ksai::Font::FastText::TextFeatures RegisterExisting(const Button& inButton, const char* inString, uint32_t inX, uint32_t inY, uint32_t inH = 0, glm::vec3 inColor = glm::vec3(0.0f), uint32_t inPaddingX = 5, uint32_t inPaddingY = 3, uint32_t inTextDepthValue = 1);
	void Update(bool inShouldHide = false);
	ksai::Font::FastText::TextFeatures UpdateExisting(bool inShouldHide = false, const std::string_view inString = "Hello", glm::uvec2 inPosition = glm::uvec2(100, 100), uint32_t inH = 0, uint32_t inPaddingX = 5, uint32_t inPaddingY = 3, uint32_t inTextDepthValue = 1);
private:
	uint32_t mTextId;
	uint32_t mDepthValue = 0;
	std::string mString;
	glm::ivec4 mPosDimenOfContainer;
	glm::ivec4 mPosDimenOfText;
};


#pragma once
#include "GUIWindow.hpp"
#include "GUI2DRenderer.hpp"
#include "FastTextFrontEnd.hpp"
#include "LuaConfig.hpp"

class Button
{
public:
	enum class Type {
		Rectangle,
		CircleInRectangle
	};
	Button(Jkr::GUI2DRenderer& inRenderer, Jkr::Font::FastTextRenderer& inTextRenderer)
		: mRenderer(inRenderer), mTextRenderer(inTextRenderer)
	{}
	void EventCall(Jkr::GUIWindow& inWindow, std::any inEvent);
	void Update(uint32_t inX, uint32_t inY, uint32_t inW, uint32_t inH, uint32_t inDepthIndex);
	void UpdateExisting(uint32_t inX, uint32_t inY, uint32_t inW, uint32_t inH, uint32_t inDepthIndex);
public:
	template<Type inType>
	void Register(uint32_t inX, uint32_t inY, uint32_t inW, uint32_t inH, glm::vec3 inColor = glm::vec3(1.0f, 1.0f, 1.0f), float inRoundedRectRadius = 0.3f) ;

	template<Type inType>
	void RegisterExisting(const Button& inButton, uint32_t inX, uint32_t inY, uint32_t inW, uint32_t inH, glm::vec3 inColor = glm::vec3(1.0f, 1.0f, 1.0f), float inRoundedRectRadius = 0.3f)
	{
		mRenderer.RegisterDynamicQuadWithExistingTexture(mButtonId, inButton.GetButtonId(), inX, inY, inW, inH, 0);
		mButtonMinMax = glm::vec4(inX, inY, inW + inX, inY + inH);
	}

	template<Type inType>
	void Register(uint32_t inX, uint32_t inY, uint32_t inW, uint32_t inH, glm::vec3 inFillColor = glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3 inCircleFillColor = glm::vec3(1.0f, 1.0f, 1.0f), float inRoundedRectRadius = 0.3f, float inCircleRadius = 0.5f);

	template<Type inType>
	void Dispatch(glm::vec3 inColorNotHovered, glm::vec3 inColorHovered);
	void Draw(const Jkr::GUIWindow& inWindow, uint32_t inWindowW, uint32_t inWindowH, glm::vec3 inColorNotHovered, glm::vec3 inColorHovered);
public:
	GETTER GetIsBeingHovered() const { return isBeingHovered; }
	GETTER GetIsButtonPushed() const { return isButtonPushed; }
	GETTER GetButtonId() const { return mButtonId; }
protected:
	Jkr::GUI2DRenderer& mRenderer;
	Jkr::Font::FastTextRenderer& mTextRenderer;
	ksai::kstd::ui32 mButtonId;
	bool isBeingHovered = false;
	bool isButtonPushed = false;
	bool mTheColorIsDefault = true;
	glm::vec4 mButtonMinMax;
private:
	bool DecideFinalColor(glm::vec3& FinalColor, const glm::vec3& inColorHovered, const glm::vec3& inColorNotHovered);
};


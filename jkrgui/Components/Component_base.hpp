#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Window.hpp>
#include <Renderers/TwoD/2d.hpp>
#include <EventManager.hpp>
#define SETTER inline void

namespace Jkr::Component
{
	using namespace Jkr::Renderer;
	class Component_base
	{
	public:
		GETTER& GetFastTexts() { return mFastTexts; }
		GETTER& GetLines() { return mLines; }
		GETTER& GetShapes() { return mShapes; }
	private:
		std::vector<glm::uvec2> mFastTexts;
		std::vector<glm::uvec2> mLines;
		std::vector<glm::uvec2> mShapes;
	public:
		Component_base(_2d& inR, EventManager& inE) : r(inR), e(inE) {}
		GETTER& GetDimension() { return mDimension; }
		GETTER& GetPosition() { return mPosition; }
		SETTER SetDimension(glm::vec2 inVec) { mDimension = inVec; }
		SETTER SetPosition(glm::vec2 inVec) {
			mPosition = inVec;
			mTransformMatrix = glm::identity<glm::mat4>();
			mTransformMatrix = glm::translate(mTransformMatrix, glm::vec3(mPosition.x, mPosition.y, 0));
		}
		SETTER SetDepthValue(uint32_t inDepthValue) { mDepthValue = inDepthValue; }
		GETTER& GetDepthValue() { return mDepthValue; }
		GETTER GetTranslationMatrix() {
			return mTransformMatrix;
		}
		glm::vec2 ToComponent(glm::vec2 inCompCoords)
		{
			return glm::vec2(inCompCoords) - mPosition;
		}
		glm::vec2 ToWindow(glm::vec2 inWorldCoords)
		{
			return glm::vec2(inWorldCoords) + mPosition;
		}
		glm::vec2 ToCentralizedCoords(glm::uvec2 mBoxDimension, glm::uvec2 mBox)
		{

		}
	protected:
		_2d& r;
		EventManager& e;
		Window* mWindow = nullptr;
		uint32_t mWh = 0, mWw = 0;
	public:
		SETTER SetWindow(Window* inWindow, uint32_t inW, uint32_t inH) {
			mWindow = inWindow;
			mWw = inW;
			mWh = inH;
		}
		GETTER GetWindowWidth() {
			return mWw;
		}
		GETTER GetWindowHeight() {
			return mWh;
		}
	private:
		glm::vec2 mDimension;
		glm::vec2 mPosition;
		glm::mat4 mTransformMatrix = glm::identity<glm::mat4>();
		uint32_t mDepthValue;
	};
}

/*

void draw();

*/
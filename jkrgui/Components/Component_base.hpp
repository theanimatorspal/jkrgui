#pragma once
#include <glm/glm.hpp>
#include <Window.hpp>
#define SETTER inline void

namespace Jkr::Component
{
	class Component_base
	{

	public:
		GETTER& GetDimension() { return mDimension; }
		GETTER& GetPosition() { return mPosition; }
		SETTER SetDimension(glm::vec2 inVec) { mDimension = inVec; }
		SETTER SetPosition(glm::vec2 inVec) { mPosition = inVec; }
		SETTER SetDepthValue(uint32_t inDepthValue) { mDepthValue = inDepthValue; }
		GETTER& GetDepthValue() { return mDepthValue; }
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
	private:
		glm::vec2 mDimension;
		glm::vec2 mPosition;
		uint32_t mDepthValue;
	};

}
#pragma once
#include "ResourceManager.hpp"
#include "Renderer_base.hpp"
#include "Shape_base.hpp"
#include <unordered_map>

namespace Jkr::Renderer
{
	class Shape : public Shape_base, Renderer_base
	{
		using sb = Shape_base;
		using rb = Renderer_base;
	public:
		Shape(const Instance& inInstance, Window& inCompatibleWindow, std::unordered_map<FillType, Up<PainterCache>>& inPainterCaches);
		void Add(Jkr::Generator& inShape, uint32_t inX, uint32_t inY, uint32_t inZ, uint32_t& outId);
		void Update(uint32_t inId, Jkr::Generator& inShape, uint32_t inX, uint32_t inY, uint32_t inZ);
		void Dispatch(Window& inWindow);
		void DrawInit(Window& inWindow, FillType inFillType);
		void Draw(Window& inWindow, glm::vec4 inColor, uint32_t inWindowW, uint32_t inWindowH, uint32_t inStartShapeId, uint32_t inEndShapeId, glm::mat4 inMatrix);
	private:
		struct PushConstant {
			glm::mat4 mMatrix;
			glm::vec4 mColor;
			glm::vec4 mParams;
		};
		/*
		Here Primitive Elements is a factor
		*/
		uint32_t mTotalNoOfVerticesRendererCanHold = 10;
		uint32_t mTotalNoOfIndicesRendererCanHold = 10;
	private:
		void CheckAndResize(const Jkr::Generator& inShape);
		const Instance& mInstance;
		std::unordered_map<FillType, Up<PainterCache>>& mPainterCaches;
		std::unordered_map<FillType, Up<Painter>> mPainters;
		Up<Primitive> mPrimitive;
	};
}
#pragma once
#include "Painter.hpp"
#include "PainterCache.hpp"
#include "AllShaders.hpp"


namespace Jkr::Renderer
{
	enum class FillType
	{
		Fill,
		Image,
		WireFrame
	};
	class ResourceManager
	{
		std::string LineRendererCacheFileName = "LineRendererCache.bin";
		std::string FastTextRendererCacheFileName = "FastTextRendererCache.bin";
		std::string ShapeRendererCacheFileName_Fill = "ShapeRendererCache_Fill.bin";
		void Make(const Jkr::Instance& inInstance) {
			mLineRendererCache = MakeUp<Jkr::PainterCache>(inInstance, ksai::PipelinePropertiesContext::Line);
			mFastTextRendererCache = MakeUp<Jkr::PainterCache>(inInstance);
			mShapePainterCaches[FillType::Fill] = MakeUp<Jkr::PainterCache>(inInstance);
		}
	public:
		ResourceManager& Load(const Jkr::Instance& inInstance) {
			Make(inInstance);
			mLineRendererCache->Load(LineRendererCacheFileName);
			mFastTextRendererCache->Load(FastTextRendererCacheFileName);
			mShapePainterCaches[FillType::Fill]->Load(ShapeRendererCacheFileName_Fill);
			return *this;
		}
		ResourceManager& Store(const Jkr::Instance& inInstance) {
			Make(inInstance);
			ksai::Shader Line(LineRendererShaders::VertexShader, LineRendererShaders::FragmentShader);
			ksai::Shader LineCompute(8, LineRendererShaders::ComputeShader);
			ksai::Shader FastText(FontShader::VertexShader, FontShader::FragmentShader);
			ksai::Shader FastTextCompute(8, FontShader::ComputeShader);
			ksai::Shader Shape_Fill(ShapeRenderers_Fill::VertexShader, ShapeRenderers_Fill::FragmentShader);
			ksai::Shader Shape_FillCompute(8, ShapeRenderers_Fill::ComputeShader);
			mLineRendererCache->Store(LineRendererCacheFileName, Line.GetVertexShader().str(), Line.GetFragmentShader().str(), LineCompute.GetComputeShader().str());
			mFastTextRendererCache->Store(FastTextRendererCacheFileName, FastText.GetVertexShader().str(), FastText.GetFragmentShader().str(), FastTextCompute.GetComputeShader().str());
			mShapePainterCaches[FillType::Fill]->Store(ShapeRendererCacheFileName_Fill, Shape_Fill.GetVertexShader().str(), Shape_Fill.GetFragmentShader().str(), Shape_FillCompute.GetComputeShader().str());
			return *this;
		}
		GETTER& GetLineRendererCache() { return *mLineRendererCache; }
		GETTER& GetFastTextRendererCache() { return *mFastTextRendererCache; }
		GETTER& GetShapePainterCaches() { return mShapePainterCaches; }
	private:
		Up<Jkr::PainterCache> mLineRendererCache;
		Up<Jkr::PainterCache> mFastTextRendererCache;
		std::unordered_map<FillType, Up<Jkr::PainterCache>> mShapePainterCaches;
	};
}

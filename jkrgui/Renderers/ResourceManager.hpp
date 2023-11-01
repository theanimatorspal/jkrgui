#pragma once
#include "Painter.hpp"
#include "PainterCache.hpp"
#include "AllShaders.hpp"

namespace Jkr::Renderer
{
	class ResourceManager
	{
		std::string LineRendererCacheFileName = "LineRendererCache.bin";
		std::string FastTextRendererCacheFileName = "FastTextRendererCache.bin";
		void Make(const Jkr::Instance& inInstance) {
			mLineRendererCache = MakeUp<Jkr::PainterCache>(inInstance, ksai::PipelinePropertiesContext::Line);
			mFastTextRendererCache = MakeUp<Jkr::PainterCache>(inInstance);
		}
	public:
		ResourceManager& Load(const Jkr::Instance& inInstance) {
			Make(inInstance);
			mLineRendererCache->Load(LineRendererCacheFileName);
			mFastTextRendererCache->Load(FastTextRendererCacheFileName);
			return *this;
		}
		ResourceManager& Store(const Jkr::Instance& inInstance) {
			Make(inInstance);
			ksai::Shader Line(LineRendererShaders::VertexShader, LineRendererShaders::FragmentShader);
			ksai::Shader LineCompute(8, LineRendererShaders::ComputeShader);
			ksai::Shader FastText(FontShader::VertexShader, FontShader::FragmentShader);
			ksai::Shader FastTextCompute(8, FontShader::ComputeShader);
			mLineRendererCache->Store(LineRendererCacheFileName, Line.GetVertexShader().str(), Line.GetFragmentShader().str(), LineCompute.GetComputeShader().str());
			mFastTextRendererCache->Store(FastTextRendererCacheFileName, FastText.GetVertexShader().str(), FastText.GetFragmentShader().str(), FastTextCompute.GetComputeShader().str());
			return *this;
		}
		GETTER& GetLineRendererCache() { return *mLineRendererCache; }
		GETTER& GetFastTextRendererCache() { return *mFastTextRendererCache; }
	private:
		Up<Jkr::PainterCache> mLineRendererCache;
		Up<Jkr::PainterCache> mFastTextRendererCache;
	};
}

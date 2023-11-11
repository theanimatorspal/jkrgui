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
		ContinousLine
	};
	class ResourceManager
	{
		std::string LineRendererCacheFileName = "LineRendererCache.bin";
		std::string FastTextRendererCacheFileName = "FastTextRendererCache.bin";
		std::string ShapeRendererCacheFileName_Fill = "ShapeRendererCache_Fill.bin";

#ifndef JKR_USE_VARIABLE_DES_INDEXING
		std::string ShapeRendererCacheFileName_Image = "ShapeRendererCache_Image.bin";
#else
		std::string ShapeRendererCacheFileName_Image = "ShapeRendererCache_ImageVarDes.bin";
		std::string BestTextRendererCacheFilename = "BestTextRendererCache.bin";
#endif
		std::string ShapeRendererCacheFileName_ContinousLine = "ShapeRendererCache_ContinousLine.bin";



		void Make(const Jkr::Instance& inInstance) {
			mLineRendererCache = MakeUp<Jkr::PainterCache>(inInstance, ksai::PipelinePropertiesContext::Line);
			mFastTextRendererCache = MakeUp<Jkr::PainterCache>(inInstance);
			mShapePainterCaches[FillType::Fill] = MakeUp<Jkr::PainterCache>(inInstance);
			mShapePainterCaches[FillType::Image] = MakeUp<Jkr::PainterCache>(inInstance);
			mShapePainterCaches[FillType::ContinousLine] = MakeUp<Jkr::PainterCache>(inInstance, ksai::PipelinePropertiesContext::LineStrip);
			mBestTextRendererCache = MakeUp<Jkr::PainterCache> ( inInstance );
			


		}
		



	public:



		ResourceManager& Load(const Jkr::Instance& inInstance) {
			Make(inInstance);
			mLineRendererCache->Load(LineRendererCacheFileName);
			mFastTextRendererCache->Load(FastTextRendererCacheFileName);
			mShapePainterCaches[FillType::Fill]->Load(ShapeRendererCacheFileName_Fill);

#ifndef JKR_USE_VARIABLE_DES_INDEXING
			mShapePainterCaches[FillType::Image]->Load(ShapeRendererCacheFileName_Image);
#else
			mShapePainterCaches[FillType::Image]->__var_des_index_Load_EXT(ShapeRendererCacheFileName_Image);
			mBestTextRendererCache->__var_des_index_Load_EXT ( BestTextRendererCacheFilename, 5000 );
#endif

			mShapePainterCaches[FillType::ContinousLine]->Load(ShapeRendererCacheFileName_ContinousLine);
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
			ksai::Shader BestTextS(BestText::VertexShader, BestText::FragmentShader);
			ksai::Shader BestText_Compute(8, BestText::ComputeShader);

			mLineRendererCache->Store(LineRendererCacheFileName, Line.GetVertexShader().str(), Line.GetFragmentShader().str(), LineCompute.GetComputeShader().str());
			mFastTextRendererCache->Store(FastTextRendererCacheFileName, FastText.GetVertexShader().str(), FastText.GetFragmentShader().str(), FastTextCompute.GetComputeShader().str());
			mShapePainterCaches[FillType::Fill]->Store(ShapeRendererCacheFileName_Fill, Shape_Fill.GetVertexShader().str(), Shape_Fill.GetFragmentShader().str(), Shape_FillCompute.GetComputeShader().str());


			ksai::Shader Shape_Image(ShapeRenderers_Image::VertexShader, ShapeRenderers_Image::FragmentShader);
			ksai::Shader Shape_ImageCompute(8, ShapeRenderers_Image::ComputeShader);
#ifndef JKR_USE_VARIABLE_DES_INDEXING
			mShapePainterCaches[FillType::Image]->Store(ShapeRendererCacheFileName_Image, Shape_Image.GetVertexShader().str(), Shape_Image.GetFragmentShader().str(), Shape_ImageCompute.GetComputeShader().str());
#else
			mShapePainterCaches[FillType::Image]->__var_des_index_Store_EXT(ShapeRendererCacheFileName_Image, Shape_Image.GetVertexShader().str(), Shape_Image.GetFragmentShader().str(), Shape_ImageCompute.GetComputeShader().str());
			mBestTextRendererCache->__var_des_index_Store_EXT ( BestTextRendererCacheFilename, BestTextS.GetVertexShader ( ).str ( ), BestTextS.GetFragmentShader ( ).str ( ), BestText_Compute.GetComputeShader ( ).str ( ), 5000);
#endif

			mShapePainterCaches[FillType::ContinousLine]->Store(ShapeRendererCacheFileName_ContinousLine, Shape_Fill.GetVertexShader().str(), Shape_Fill.GetFragmentShader().str(), Shape_FillCompute.GetComputeShader().str());

			return *this;
		}







		GETTER& GetLineRendererCache() { return *mLineRendererCache; }
		GETTER& GetFastTextRendererCache() { return *mFastTextRendererCache; }
		GETTER& GetShapePainterCaches() { return mShapePainterCaches; }
		GETTER& GetBestTextRendererCache ( ) { return *mBestTextRendererCache; }
	private:
		Up<Jkr::PainterCache> mLineRendererCache;
		Up<Jkr::PainterCache> mFastTextRendererCache;
		Up<Jkr::PainterCache> mBestTextRendererCache;
		std::unordered_map<FillType, Up<Jkr::PainterCache>> mShapePainterCaches;
	};
}

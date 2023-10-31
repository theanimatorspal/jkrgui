#include "Application.hpp"

void LoadCaches(Jkr::GUIPainterCache& FontPainterCache, Jkr::GUIPainterCache& GUI2DPainterCache, Jkr::GUIPainterCache& LineRendererCache)
{
	const ksai::Shader FontShader(FontShader::VertexShader, FontShader::FragmentShader);
	const ksai::Shader FontComputeShader(8, FontShader::ComputeShader);
	const ksai::Shader GUI2DShaderer(GUI2DShaders::VertexShader, GUI2DShaders::FragmentShader);
	const ksai::Shader GUI2DCompute(8, GUI2DShaders::ComputeShader);
	const ksai::Shader LineRenderer(LineRendererShaders::VertexShader, LineRendererShaders::FragmentShader);
	FontPainterCache.Load("FontCache.bin",
		FontShader.GetVertexShader().str(),
		FontShader.GetFragmentShader().str(),
		FontComputeShader.GetComputeShader().str()
	);

	GUI2DPainterCache.Load_EXT("2DRendererCache.bin",
		GUI2DShaderer.GetVertexShader().str(),
		GUI2DShaderer.GetFragmentShader().str(),
		GUI2DCompute.GetComputeShader().str()
	);

	LineRendererCache.Load(
		"LineRendererCache.bin",
		LineRenderer.GetVertexShader().str(),
		LineRenderer.GetFragmentShader().str(),
		FontComputeShader.GetComputeShader().str()
	);

}


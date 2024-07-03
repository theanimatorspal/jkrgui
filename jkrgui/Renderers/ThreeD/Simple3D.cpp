#include "Simple3D.hpp"
#include "Pipeline/VulkanPipelineLayout.hpp"

using namespace Jkr::Renderer::_3D;

void Simple3D::Bind(Window& inWindow, CmdParam inParam) {
    mPainter->BindDrawPipeline(inWindow, inParam);
}

void Simple3D::BindByCommandBuffer(VulkanCommandBuffer& inBuffer) {
    mPainter->BindDrawPipeline(inBuffer);
}

void Simple3D::BindCompute(Window& inWindow, CmdParam inParam) {
    mPainter->BindComputePipeline(inWindow, inParam);
}

void Simple3D::Compile(Jkr::Instance& inInstance,
                       Jkr::Window& inCompatibleWindow,
                       std::string_view inFileName,
                       std::string_view inVertexShader,
                       std::string_view inFragmentShader,
                       std::string_view inComputeShader,
                       bool inShouldLoad) {
    mPainterCache = mu<PainterCache>(inInstance);
    using namespace std;
    if (not inShouldLoad) {
        mPainterCache->Store(string(inFileName),
                             string(inVertexShader),
                             string(inFragmentShader),
                             string(inComputeShader));
    } else {
        mPainterCache->Load(string(inFileName));
    }
    mPainter = mu<Painter>(inInstance, inCompatibleWindow, *mPainterCache, mPipelineContext);
}

void Simple3D::CompileForShadowOffscreen(Jkr::Instance& inInstance,
                                         Jkr::WindowMulT& inCompatibleWindow,
                                         std::string_view inFilename,
                                         std::string_view inVertexShader,
                                         std::string_view inFragmentShader,
                                         std::string_view inComputeShader,
                                         bool inShouldLoad

) {
    mPipelineContext = ksai::PipelineContext::Shadow;
    CompileWithCustomRenderPass(inInstance,
                                inCompatibleWindow,
                                inCompatibleWindow.GetShadowPass().GetRenderPass(),
                                inFilename,
                                inVertexShader,
                                inFragmentShader,
                                inComputeShader,
                                inShouldLoad,
                                mPipelineContext);
}

void Simple3D::CompileWithCustomRenderPass(Jkr::Instance& inInstance,
                                           Jkr::WindowMulT& inCompatibleWindow,
                                           Jkr::VulkanRenderPassBase& inRenderPass,
                                           std::string_view inFilename,
                                           std::string_view inVertexShader,
                                           std::string_view inFragmentShader,
                                           std::string_view inComputeShader,
                                           bool inShouldLoad,
                                           PipelineContext inPipelineContext) {

    mPipelineContext = inPipelineContext;
    mPainterCache    = mu<PainterCache>(inInstance);
    using namespace std;
    if (not inShouldLoad) {
        mPainterCache->Store(string(inFilename),
                             string(inVertexShader),
                             string(inFragmentShader),
                             string(inComputeShader));
    } else {
        mPainterCache->Load(string(inFilename));
    }
    mPainter = mu<Painter>(inInstance, inRenderPass, *mPainterCache, mPipelineContext);
}

void Simple3D::CompileForDeferredOffscreen(Jkr::Instance& inInstance,
                                           Jkr::WindowMulT& inCompatibleWindow,
                                           std::string_view inFilename,
                                           std::string_view inVertexShader,
                                           std::string_view inFragmentShader,
                                           std::string_view inComputeShader,
                                           bool inShouldLoad) {

    mPipelineContext = PipelineContext::DefaultSingleSampled; // TODO Modify for multi sampeld
    mPainterCache    = mu<PainterCache>(inInstance);
    using namespace std;
    if (not inShouldLoad) {
        mPainterCache->Store(string(inFilename),
                             string(inVertexShader),
                             string(inFragmentShader),
                             string(inComputeShader));
    } else {
        mPainterCache->Load(string(inFilename));
    }
    mPainter = mu<Painter>(inInstance,
                           inCompatibleWindow.GetDeferredPass().GetRenderPass(),
                           *mPainterCache,
                           mPipelineContext);
}
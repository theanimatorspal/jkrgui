#include "Simple3D.hpp"
#include "Pipeline/VulkanPipelineLayout.hpp"

using namespace Jkr::Renderer::_3D;

void Simple3D::Bind(Window_base &inWindow, CmdParam inParam) {
    mPainter->BindDrawPipeline(inWindow, inParam);
}

void Simple3D::BindByCommandBuffer(VulkanCommandBuffer &inBuffer) {
    mPainter->BindDrawPipeline(inBuffer);
}

void Simple3D::BindCompute(Window_base &inWindow, CmdParam inParam) {
    mPainter->BindComputePipeline(inWindow, inParam);
}

void Simple3D::CompileDefault(Jkr::Instance &inInstance,
                              Jkr::Window_base &inCompatibleWindow,
                              std::string_view inFileName,
                              std::string_view inVertexShader,
                              std::string_view inFragmentShader,
                              std::string_view inComputeShader,
                              Jkr::Misc::File& inFile) {
    if (not mPainterCache) {
        mPainterCache = mu<PainterCache>(inInstance);
    }
    using namespace std;
    mPainterCache->Store(inFile, string(inFileName),
                            string(inVertexShader),
                            string(inFragmentShader),
                            string(inComputeShader));
    mPainter = mu<Painter>(inInstance, inCompatibleWindow, *mPainterCache, mPipelineContext);
}

void Simple3D::CompileForShadowOffscreen(Jkr::Instance &inInstance,
                                         Jkr::Window &inCompatibleWindow,
                                         std::string_view inFilename,
                                         std::string_view inVertexShader,
                                         std::string_view inFragmentShader,
                                         std::string_view inComputeShader,
                                         Jkr::Misc::File& inFile
) {
    mPipelineContext = ksai::PipelineContext::Shadow;
    CompileWithCustomRenderPass(inInstance,
                                inCompatibleWindow.GetShadowPass().GetRenderPass(),
                                inFilename,
                                inVertexShader,
                                inFragmentShader,
                                inComputeShader,
                                inFile,
                                mPipelineContext);
}

void Simple3D::CompileWithCustomRenderPass(Jkr::Instance &inInstance,
                                           Jkr::VulkanRenderPassBase &inRenderPass,
                                           std::string_view inFilename,
                                           std::string_view inVertexShader,
                                           std::string_view inFragmentShader,
                                           std::string_view inComputeShader,
                                           Jkr::Misc::File& inFile,
                                           PipelineContext inPipelineContext) {

    mPipelineContext = inPipelineContext;
    if (not mPainterCache) {
        mPainterCache = mu<PainterCache>(inInstance);
    }
    using namespace std;
    mPainterCache->Store(inFile, string(inFilename),
                            string(inVertexShader),
                            string(inFragmentShader),
                            string(inComputeShader));
    mPainter = mu<Painter>(inInstance, inRenderPass, *mPainterCache, mPipelineContext);
}

void Simple3D::CompileForDeferredOffscreen(Jkr::Instance &inInstance,
                                           Jkr::Window &inCompatibleWindow,
                                           std::string_view inFilename,
                                           std::string_view inVertexShader,
                                           std::string_view inFragmentShader,
                                           std::string_view inComputeShader,
                                           Jkr::Misc::File& inFile) {

    mPipelineContext = PipelineContext::DefaultSingleSampled; // TODO Modify for multi sampeld
    if (not mPainterCache) {
        mPainterCache = mu<PainterCache>(inInstance);
    }
    using namespace std;
    mPainterCache->Store(inFile, string(inFilename),
                            string(inVertexShader),
                            string(inFragmentShader),
                            string(inComputeShader));
    mPainter = mu<Painter>(inInstance,
                           inCompatibleWindow.GetDeferredPass().GetRenderPass(),
                           *mPainterCache,
                           mPipelineContext);
}

void Simple3D::CompileForDeferredCompositionOffscreen(Jkr::Instance &inInstance,
                                                      Jkr::Window &inCompatibleWindow,
                                                      std::string_view inFilename,
                                                      std::string_view inVertexShader,
                                                      std::string_view inFragmentShader,
                                                      std::string_view inComputeShader,
                                                      Jkr::Misc::File& inFile) {
    mPipelineContext = PipelineContext::DefaultSingleSampled;
    if (not mPainterCache) {
        mPainterCache = mu<PainterCache>(inInstance);
    }
    using namespace std;
    mPainterCache->Store(inFile, string(inFilename),
                            string(inVertexShader),
                            string(inFragmentShader),
                            string(inComputeShader));
    mPainter = mu<Painter>(inInstance,
                           inCompatibleWindow.GetDeferredPass().GetCompositionRenderPass(),
                           *mPainterCache,
                           mPipelineContext);
}

void Simple3D::Compile(Jkr::Instance &inInstance,
                       Jkr::Window &inCompatibleWindow,
                       std::string_view inFilename,
                       std::string_view inVertexShader,
                       std::string_view inFragmentShader,
                       std::string_view inComputeShader,
                       Jkr::Misc::File& inFile,
                       CompileContext inContext) {
    mCompileContext = inContext;
    switch (inContext) {
        case CompileContext::Default:
            CompileDefault(inInstance,
                           inCompatibleWindow,
                           inFilename,
                           inVertexShader,
                           inFragmentShader,
                           inComputeShader,
                           inFile);
            break;
        case CompileContext::ShadowPass:
            CompileForShadowOffscreen(inInstance,
                                      inCompatibleWindow,
                                      inFilename,
                                      inVertexShader,
                                      inFragmentShader,
                                      inComputeShader,
                                      inFile);
            break;
        case CompileContext::Deferred:
            CompileForDeferredOffscreen(inInstance,
                                        inCompatibleWindow,
                                        inFilename,
                                        inVertexShader,
                                        inFragmentShader,
                                        inComputeShader,
                                        inFile);
            break;
        case CompileContext::DeferredComposition:
            CompileForDeferredCompositionOffscreen(inInstance,
                                                   inCompatibleWindow,
                                                   inFilename,
                                                   inVertexShader,
                                                   inFragmentShader,
                                                   inComputeShader,
                                                   inFile);
            break;
        default:
            break;
    }

    if (inContext >= 0) {
        CompileWithCustomRenderPass(
             inInstance,
             *inCompatibleWindow.GetArbritaryPassses()[inContext]->mRenderpass,
             inFilename,
             inVertexShader,
             inFragmentShader,
             inComputeShader,
             inFile);
    }
}

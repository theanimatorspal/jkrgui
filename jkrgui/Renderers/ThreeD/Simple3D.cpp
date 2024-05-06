#include "Simple3D.hpp"
#include "Pipeline/VulkanPipelineLayout.hpp"

using namespace Jkr::Renderer::_3D;

void Simple3D::Bind(Window& inWindow, CmdParam inParam) {
    mPainter->BindDrawParamtersPipelineOnly_EXT(inWindow, inParam);
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
    mPipelineContext = PipelineContext::Shadow;
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
                           inCompatibleWindow.GetShadowPass().GetRenderPass(),
                           *mPainterCache,
                           mPipelineContext);
}

void Shape::Bind(Window& inWindow, ComPar inCompar) {
    Painter::BindDrawParamtersVertexAndIndexBuffersOnly_EXT(
         mInstance, *mPrimitive, inWindow, inCompar);
}

static std::mutex AddMutex;
ui Shape::AddEXT(Generator& inGenerator, glm::vec3 inPosition) {
    ui i;
    {
        std::lock_guard<std::mutex> Guard(AddMutex);
        Add(inGenerator, inPosition, i);
    }
    return i;
}

ui Shape::AddEXT(glTF_Model& inModel) {
    ui i;
    {
        std::lock_guard<std::mutex> Guard(AddMutex);
        Add(inModel, i);
    }
    return i;
}
#pragma once
#include "Instance.hpp"
#include "PainterCache.hpp"
#include "Renderers/Renderer_base.hpp"
#include "Shape3D.hpp"
#include "WindowMulT.hpp"
#include <string_view>

namespace Jkr::Renderer::_3D {
class Simple3D {
    public:
    GETTER& GetPainter() { return *mPainter; }
    GETTER& GetPainterCache() { return *mPainterCache; }

    void SetPipelineContext(PipelineContext inPipelineContext) {
        mPipelineContext = inPipelineContext;
    }
    // TODO Change this name to Build
    void Compile(Jkr::Instance& inInstance,
                 Jkr::Window& inCompatibleWindow,
                 std::string_view inFilename,
                 std::string_view inVertexShader,
                 std::string_view inFragmentShader,
                 std::string_view inComputeShader,
                 bool inShouldLoad);
    void CompileForShadowOffscreen(Jkr::Instance& inInstance,
                                   Jkr::WindowMulT& inCompatibleWindow,
                                   std::string_view inFilename,
                                   std::string_view inVertexShader,
                                   std::string_view inFragmentShader,
                                   std::string_view inComputeShader,
                                   bool inShouldLoad

    );
    void CompileWithCustomRenderPass(Jkr::Instance& inInstance,
                                     Jkr::WindowMulT& inCompatibleWindow,
                                     Jkr::VulkanRenderPassBase& inRenderPass,
                                     std::string_view inFilename,
                                     std::string_view inVertexShader,
                                     std::string_view inFragmentShader,
                                     std::string_view inComputeShader,
                                     bool inShouldLoad);
    template <typename T>
    void Draw(Jkr::Window& inWindow,
              Shape& inShape3D,
              T inPush,
              ui inFirstIndex,
              ui inIndexCount,
              ui inInstanceCount,
              CmdParam inParam);
    void Bind(Window& inWindow, CmdParam inParam);
    void BindCompute(Window& inWindow, CmdParam inParam);
    template <typename T>
    void Dispatch(Jkr::Window& inWindow, Shape& inShape3D, T inPush, int inX, int inY, int inZ);
    Simple3D(Jkr::Instance& inInstance, Jkr::Window& inCompatibleWindow) {}

    private:
    PipelineContext mPipelineContext = PipelineContext::Default;
    Up<Painter> mPainter;
    Up<PainterCache> mPainterCache;
};

template <typename T>
inline void
Simple3D::Dispatch(Jkr::Window& inWindow, Shape& inShape3D, T inPush, int inX, int inY, int inZ) {
    mPainter->Dispatch_EXT<T>(inWindow, inPush, inX, inY, inZ);
}

template <typename T>
inline void Simple3D::Draw(Jkr::Window& inWindow,
                           Shape& inShape3D,
                           T inPush,
                           ui inFirstIndex,
                           ui inIndexCount,
                           ui inInstanceCount,
                           CmdParam inParam) {
    mPainter->Draw_EXT<T>(inShape3D.GetPrimitive(),
                          inPush,
                          inWindow,
                          inIndexCount,
                          inInstanceCount,
                          inFirstIndex,
                          0,
                          inParam);
}

} // namespace Jkr::Renderer::_3D
#pragma once
#include "Instance.hpp"
#include "PainterCache.hpp"
#include "Renderers/Renderer_base.hpp"
#include "Shape3D.hpp"
#include "Window.hpp"
#include <string_view>

namespace Jkr::Misc {
class FileJkr;
}
namespace Jkr::Renderer::_3D {
class Simple3D {
    public:
    enum CompileContext : int {
        Default             = -5,
        ShadowPass          = -4,
        Deferred            = -3,
        DeferredShadow      = -2,
        DeferredComposition = -1,
        ArbritaryPass       = 0
    };
    GETTER &GetPainter() { return *mPainter; }
    GETTER &GetPainterCache() { return *mPainterCache; }
    GETTER &GetPainterCachePtr() { return mPainterCache; }
    GETTER GetCompileContext() const { return mCompileContext; }
    void SetPipelineContext(PipelineContext inPipelineContext) {
        mPipelineContext = inPipelineContext;
    }
    // TODO Change this name to Build
    void CompileDefault(Jkr::Instance &inInstance,
                        Jkr::Window_base &inCompatibleWindow,
                        std::string_view inFilename,
                        std::string_view inVertexShader,
                        std::string_view inFragmentShader,
                        std::string_view inComputeShader,
                        bool inShouldLoad);
    void CompileForShadowOffscreen(Jkr::Instance &inInstance,
                                   Jkr::Window &inCompatibleWindow,
                                   std::string_view inFilename,
                                   std::string_view inVertexShader,
                                   std::string_view inFragmentShader,
                                   std::string_view inComputeShader,
                                   bool inShouldLoad);
    void CompileForDeferredOffscreen(Jkr::Instance &inInstance,
                                     Jkr::Window &inCompatibleWindow,
                                     std::string_view inFilename,
                                     std::string_view inVertexShader,
                                     std::string_view inFragmentShader,
                                     std::string_view inComputeShader,
                                     bool inShouldLoad);
    void CompileForDeferredCompositionOffscreen(Jkr::Instance &inInstance,
                                                Jkr::Window &inCompatibleWindow,
                                                std::string_view inFilename,
                                                std::string_view inVertexShader,
                                                std::string_view inFragmentShader,
                                                std::string_view inComputeShader,
                                                bool inShouldLoad);

    void Compile(Jkr::Instance &inInstance,
                 Jkr::Window &inCompatibleWindow,
                 std::string_view inFilename,
                 std::string_view inVertexShader,
                 std::string_view inFragmentShader,
                 std::string_view inComputeShader,
                 bool inShouldLoad,
                 CompileContext inContext);

    void CompileWithCustomRenderPass(
         Jkr::Instance &inInstance,
         Jkr::VulkanRenderPassBase &inRenderPass,
         std::string_view inFilename,
         std::string_view inVertexShader,
         std::string_view inFragmentShader,
         std::string_view inComputeShader,
         bool inShouldLoad,
         PipelineContext inPipelineContext = PipelineContext::DefaultSingleSampled);

    template <typename T>
    void Draw(Jkr::Window_base &inWindow,
              Shape &inShape3D,
              T inPush,
              ui inFirstIndex,
              ui inIndexCount,
              ui inInstanceCount,
              CmdParam inParam);

    void BindByCommandBuffer(VulkanCommandBuffer &inCommandBuffer);
    template <typename T>
    void DrawByCommandBuffer(VulkanCommandBuffer &inCommandBuffer,
                             Shape &inShape3D,
                             T inPush,
                             ui inFirstIndex,
                             ui inIndexCount,
                             ui inInstanceCount);

    void Bind(Window_base &inWindow, CmdParam inParam);
    void BindCompute(Window_base &inWindow, CmdParam inParam);
    template <typename T>
    void
    Dispatch(Jkr::Window_base &inWindow, Shape &inShape3D, T inPush, int inX, int inY, int inZ);

    Simple3D() = default;
    ///@warning This is not needed, remove this.
    Simple3D(Jkr::Instance &inInstance, Jkr::Window_base &inCompatibleWindow) {}

    private:
    CompileContext mCompileContext   = CompileContext::Default;
    PipelineContext mPipelineContext = PipelineContext::Default;
    Up<Painter> mPainter;
    Up<PainterCache> mPainterCache;
};

template <typename T>
inline void Simple3D::Dispatch(
     Jkr::Window_base &inWindow, Shape &inShape3D, T inPush, int inX, int inY, int inZ) {
    mPainter->Dispatch_EXT<T>(inWindow, inPush, inX, inY, inZ);
}

template <typename T>
inline void Simple3D::Draw(Jkr::Window_base &inWindow,
                           Shape &inShape3D,
                           T inPush,
                           ui inFirstIndex,
                           ui inIndexCount,
                           ui inInstanceCount,
                           CmdParam inParam) {
    mPainter->Draw_EXT<T>(
         inPush, inWindow, inIndexCount, inInstanceCount, inFirstIndex, 0, inParam);
}

template <typename T>
inline void Simple3D::DrawByCommandBuffer(VulkanCommandBuffer &inCommandBuffer,
                                          Shape &inShape3D,
                                          T inPush,
                                          ui inFirstIndex,
                                          ui inIndexCount,
                                          ui inInstanceCount) {

    mPainter->Draw_EXT<T>(inPush, inCommandBuffer, inIndexCount, inInstanceCount, inFirstIndex, 0);
}

} // namespace Jkr::Renderer::_3D
#include "Instance.hpp"
#include "PainterCache.hpp"
#include "Renderers/Renderer_base.hpp"
#include "Shape3D.hpp"
#include "Window.hpp"
#include <string_view>

namespace Jkr::Renderer::_3D {
class Simple3D {
      public:
          GETTER& GetGlslVertexShaderHeaderString() const { return mGlslVertexShaderHeader; }
          GETTER& GetGlslFragmentShaderHeaderString() const { return mGLslFragmentShaderHeader; }
          GETTER& GetGlslComputeShaderHeaderString() const { return mGLslFragmentShaderHeader; }

          Simple3D(Jkr::Instance& inInstance, Jkr::Window& inCompatibleWindow);
          void Compile(Jkr::Instance& inInstance,
                       Jkr::Window& inCompatibleWindow,
                       std::string_view inFilename,
                       std::string_view inVertexShader,
                       std::string_view inFragmentShader,
                       std::string_view inComputeShader,
                       bool inShouldLoad);
          template <typename T> void Draw(Jkr::Window& inWindow, Shape& inShape3D, T inPush, ui inIndexCount, ui inInstanceCount, CmdParam inParam);
          template <typename T> void Dispatch(Jkr::Window& inWindow, Shape& inShape3D, T inPush) {}
          void Bind(Window& inWindow, CmdParam inParam);

      private:
          Up<Painter> mPainter;
          Up<PainterCache> mPainterCache;

          const std::string mGlslVertexShaderHeader = R"""(
#version 450
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inUV;
layout(location = 3) in vec3 inColor;
)""";

          const std::string mGLslFragmentShaderHeader = R"""(
#version 450
)""";
};

inline Simple3D::Simple3D(Jkr::Instance& inInstance, Jkr::Window& inCompatibleWindow) {}

template <typename T>
inline void Simple3D::Draw(Jkr::Window& inWindow, Shape& inShape3D, T inPush, ui inIndexCount, ui inInstanceCount, CmdParam inParam) {
          mPainter->Draw_EXT<T>(inShape3D.GetPrimitive(), inPush, inWindow, inIndexCount, inInstanceCount, 0, 0, inParam);
}

inline void Simple3D::Bind(Window& inWindow, CmdParam inParam) { mPainter->BindDrawParamtersPipelineOnly_EXT(inWindow, inParam); }

inline void Simple3D::Compile(Jkr::Instance& inInstance,
                              Jkr::Window& inCompatibleWindow,
                              std::string_view inFileName,
                              std::string_view inVertexShader,
                              std::string_view inFragmentShader,
                              std::string_view inComputeShader,
                              bool inShouldLoad) {

          mPainterCache = mu<PainterCache>(inInstance);
          using namespace std;
          if (not inShouldLoad) {
                    mPainterCache->Store(string(inFileName), string(inVertexShader), string(inFragmentShader), string(inComputeShader));
          } else {
                    mPainterCache->Load(string(inFileName));
          }
          mPainter = mu<Painter>(inInstance, inCompatibleWindow, *mPainterCache);
}

} // namespace Jkr::Renderer::_3D
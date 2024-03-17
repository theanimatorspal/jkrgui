#include "Instance.hpp"
#include "Shape3D.hpp"
#include "Window.hpp"

namespace Jkr::Renderer::_3D {
class Simple3D {
      public:
          GETTER& GetGlslVertexShaderHeaderString() const { return mGlslVertexShaderHeader; }
          GETTER& GetGlslFragmentShaderHeaderString() const { return mGLslFragmentShaderHeader; }
          GETTER& GetGlslComputeShaderHeaderString() const { return mGLslFragmentShaderHeader; }

          Simple3D(Jkr::Instance& inInstance, Jkr::Window& inCompatibleWindow);

      private:
          Up<Painter> mPainters;
          Up<PainterCache> mPainterCaches;

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
} // namespace Jkr::Renderer::_3D
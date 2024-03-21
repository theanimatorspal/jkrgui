#include "JkrLuaExe.hpp"
#include "Window.hpp"
#include "ksai_config.hpp"
#include "sol/sol.hpp"
#include <Renderers/ThreeD/Simple3D.hpp>

namespace JkrEXE {

struct DefaultPushConstant3D {
          glm::mat4 m1;
          glm::mat4 m2;
};

struct DefaultPushConstant3DCompute {
          glm::vec4 v1, v2;
};

void CreateRenderer3DBindings(sol::state& s) {
          auto Jkr = s["Jkr"].get_or_create<sol::table>();
          using namespace Jkr::Renderer::_3D;
          Jkr.new_usertype<Simple3D>("Simple3D",
                                     sol::factories([](Jkr::Instance& inI, Jkr::Window& inW) { return mu<Simple3D>(inI, inW); }),
                                     "Draw",
                                     &Simple3D::Draw<DefaultPushConstant3D>,
                                     "Dispatch",
                                     &Simple3D::Dispatch<DefaultPushConstant3DCompute>);
}
} // namespace JkrEXE
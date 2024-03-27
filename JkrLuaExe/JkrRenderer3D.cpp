#include "JkrLuaExe.hpp"
#include "sol/sol.hpp"
#include <string_view>

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
                                     sol::call_constructor,
                                     sol::factories([](Jkr::Instance& inI, Jkr::Window& inW) { return mu<Simple3D>(inI, inW); }),
                                     "Compile",
                                     &Simple3D::Compile,
                                     "Draw",
                                     &Simple3D::Draw<DefaultPushConstant3D>,
                                     "Dispatch",
                                     &Simple3D::Dispatch<DefaultPushConstant3DCompute>,
                                     "Bind",
                                     &Simple3D::Bind);

          Jkr.new_usertype<Jkr::Renderer::_3D::Shape>("Shape3D",
                                                      sol::call_constructor,
                                                      sol::factories([](const Jkr::Instance& inInstance, Jkr::Window& inCompatibleWindow) {
                                                                return mu<Jkr::Renderer::_3D::Shape>(inInstance, inCompatibleWindow);
                                                      }),
                                                      "Bind",
                                                      &Jkr::Renderer::_3D::Shape::Bind,
                                                      "Add",
                                                      &Jkr::Renderer::_3D::Shape::AddEXT,
                                                      "Dispatch",
                                                      &Jkr::Renderer::_3D::Shape::Dispatch);
}
} // namespace JkrEXE
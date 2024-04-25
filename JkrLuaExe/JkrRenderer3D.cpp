#include "JkrLuaExe.hpp"
#include "Pipeline/VulkanPipelineLayout.hpp"
#include "Renderers/ThreeD/glTF_Model.hpp"
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
    Jkr.new_usertype<DefaultPushConstant3D>("DefaultPushConstant3D",
                                            sol::call_constructor,
                                            sol::default_constructor,
                                            "m1",
                                            &DefaultPushConstant3D::m1,
                                            "m2",
                                            &DefaultPushConstant3D::m2);
    Jkr.new_usertype<DefaultPushConstant3DCompute>("DefaultPushConstant3DCompute",
                                                   sol::call_constructor,
                                                   sol::default_constructor,
                                                   "v1",
                                                   &DefaultPushConstant3DCompute::v1,
                                                   "v2",
                                                   &DefaultPushConstant3DCompute::v2);
    Jkr.new_enum<false>("PipelineContext",
                        "Default",
                        PipelineContext::Default,
                        "DefaultWithGeometry",
                        PipelineContext::DefaultWithGeometry,
                        "Skybox",
                        PipelineContext::Skybox);
    Jkr.new_usertype<Simple3D>("Simple3D",
                               sol::call_constructor,
                               sol::factories([](Jkr::Instance& inI, Jkr::Window& inW) {
                                   return mu<Simple3D>(inI, inW);
                               }),
                               "Compile",
                               &Simple3D::Compile,
                               "CompileForShadowOffscreen",
                               &Simple3D::CompileForShadowOffscreen,

                               "Draw",
                               &Simple3D::Draw<DefaultPushConstant3D>,
                               "Dispatch",
                               &Simple3D::Dispatch<DefaultPushConstant3DCompute>,
                               "Bind",
                               &Simple3D::Bind,
                               "SetPipelineContext",
                               &Simple3D::SetPipelineContext);

    Jkr.new_usertype<Jkr::Renderer::_3D::glTF_Model>(
         "glTF_Model",
         sol::call_constructor,
         sol::factories([](string_view inFileName) { return mu<glTF_Model>(inFileName); }),
         "GetVerticesSize",
         &Jkr::Renderer::_3D::glTF_Model::GetVerticesSize,
         "GetIndicesSize",
         &Jkr::Renderer::_3D::glTF_Model::GetIndicesSize,
         "GetImagesSize",
         &Jkr::Renderer::_3D::glTF_Model::GetImagesSize,
         "GetTexturesSize",
         &Jkr::Renderer::_3D::glTF_Model::GetTexturesSize,
         "GetNodesSize",
         &Jkr::Renderer::_3D::glTF_Model::GetNodesSize,
         "GetMaterialsSize",
         &Jkr::Renderer::_3D::glTF_Model::GetMaterialsSize,
         "GetSkinsSize",
         &Jkr::Renderer::_3D::glTF_Model::GetSkinsSize,
         "GetAnimationsSize",
         &Jkr::Renderer::_3D::glTF_Model::GetAnimationsSize,
         "GetActiveAnimation",
         &Jkr::Renderer::_3D::glTF_Model::GetActiveAnimation,
         "GetJointsCount",
         &Jkr::Renderer::_3D::glTF_Model::GetJointsCount,
         "GetNodeMatrixByIndex",
         &Jkr::Renderer::_3D::glTF_Model::GetNodeMatrixByIndex,
         "SetActiveAnimation",
         &Jkr::Renderer::_3D::glTF_Model::SetActiveAnimation,
         "GetNodeIndexByMeshIndex",
         &Jkr::Renderer::_3D::glTF_Model::GetNodeIndexByMeshIndex);
    Jkr.new_usertype<Jkr::Renderer::_3D::Shape>(
         "Shape3D",
         sol::call_constructor,
         sol::factories([](const Jkr::Instance& inInstance, Jkr::Window& inCompatibleWindow) {
             return mu<Jkr::Renderer::_3D::Shape>(inInstance, inCompatibleWindow);
         }),
         "Bind",
         &Jkr::Renderer::_3D::Shape::Bind,
         "Add",
         sol::overload(
              sol::resolve<glTF_Model&>(&Jkr::Renderer::_3D::Shape::AddEXT),
              sol::resolve<Jkr::Generator&, glm::vec3>(&Jkr::Renderer::_3D::Shape::AddEXT)),
         "Dispatch",
         &Jkr::Renderer::_3D::Shape::Dispatch,
         "GetVertexCount",
         &Jkr::Renderer::_3D::Shape::GetVertexCount,
         "GetIndexCount",
         &Jkr::Renderer::_3D::Shape::GetIndexCount);
}
} // namespace JkrEXE
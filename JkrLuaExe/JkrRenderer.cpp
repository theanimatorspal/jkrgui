#include "JkrLuaExe.hpp"
#include "Renderers/Generator.hpp"
#include "Renderers/TwoD/Shape.hpp"

namespace JkrEXE {

using namespace Jkr::Renderer;
struct ShapeRendererResources {
    void Add(Jkr::Instance& inInstance,
             FillType inFillType,
             PipelinePropertiesContext inContext,
             std::string_view inFileName,
             std::string_view inVertexShader,
             std::string_view inFragmentShader,
             std::string_view inComputeShader,
             bool inShouldLoad);
    std::unordered_map<FillType, up<PainterCache>> mCaches;
};

void CreateRendererBindings(sol::state& inState) {
    auto Jkr = inState["Jkr"].get_or_create<sol::table>();
    using namespace Jkr::Renderer;
    auto CmdParamEnumType       = Jkr.new_enum<false>("CmdParam",
                                                "UI",
                                                CmdParam::UI,
                                                "Background",
                                                CmdParam::Background,
                                                "None",
                                                CmdParam::None);

    auto FillTypeEnum           = Jkr.new_enum<false>("FillType",
                                            "Fill",
                                            FillType::Fill,
                                            "Image",
                                            FillType::Image,
                                            "ContinousLine",
                                            FillType::ContinousLine);

    auto PipelinePropertiesEnum = Jkr.new_enum<false>("PipelineProperties",
                                                      "Default",
                                                      Jkr::PipelinePropertiesContext::Default,
                                                      "Line",
                                                      Jkr::PipelinePropertiesContext::Line,
                                                      "LineStrip",
                                                      Jkr::PipelinePropertiesContext::LineStrip);

    Jkr.new_usertype<ShapeRendererResources>(
         "ShapeRendererResources",
         sol::call_constructor,
         []() { return ShapeRendererResources(); },
         "Add",
         &ShapeRendererResources::Add);

    Jkr.new_enum<false>("Shapes",
                        "EllipseWire",
                        Jkr::Shapes::EllipseWire,
                        "EllipseFill",
                        Jkr::Shapes::EllipseFill,
                        "RectangleWire",
                        Jkr::Shapes::RectangleWire,
                        "RectangleFill",
                        Jkr::Shapes::RectangleFill,
                        "Bezier2_8Wire",
                        Jkr::Shapes::Bezier2_8Wire,
                        "Cube3D",
                        Jkr::Shapes::Cube3D,
                        "Sphere3D",
                        Jkr::Shapes::Sphere3D,
                        "Cylinder3D",
                        Jkr::Shapes::Cylinder3D,
                        "Cone3D",
                        Jkr::Shapes::Cone3D,
                        "Torus3D",
                        Jkr::Shapes::Torus3D,
                        "Icosphere3D",
                        Jkr::Shapes::Icosphere3D);

    Jkr.new_usertype<Jkr::Renderer::Generator::Arguments>("Arguments");

    Jkr.new_usertype<Jkr::Renderer::Generator>(
         "Generator",
         sol::call_constructor,
         sol::factories([]() { return Jkr::Renderer::Generator(); },
                        [](Jkr::Shapes inShape, Jkr::Generator::Arguments inArg) {
                            return Jkr::Renderer::Generator(inShape, inArg);
                        }));
    Jkr.new_usertype<Jkr::Renderer::Shape>(
         "ShapeRenderer",
         sol::call_constructor,
         sol::factories([](Jkr::Instance& inInstance,
                           Jkr::WindowMulT& inCompatibleWindow,
                           ShapeRendererResources& inResources) {
             return mu<Jkr::Renderer::Shape>(inInstance, inCompatibleWindow, inResources.mCaches);
         }),
         "Add",
         &Jkr::Renderer::Shape::AddEXT,
         "Update",
         &Jkr::Renderer::Shape::UpdateEXT,
         "AddImage",
         sol::overload(sol::resolve<ui(const string_view)>(&Jkr::Renderer::Shape::AddImageEXT),
                       sol::resolve<ui(ui, ui)>(&Jkr::Renderer::Shape::AddImageEXT),
                       sol::resolve<ui(vector<uc>, ui, ui)>(&Jkr::Renderer::Shape::AddImageEXT)),
         "BindShapes",
         &Jkr::Renderer::Shape::BindShapes,
         "BindImage",
         &Jkr::Renderer::Shape::BindImage,
         "Draw",
         &Jkr::Renderer::Shape::DrawEXT,
         "Dispatch",
         &Jkr::Renderer::Shape::Dispatch,
         "Update",
         &Jkr::Renderer::Shape::Update,
         "BindFillMode",
         &Jkr::Renderer::Shape::BindFillMode,
         "CopyToImage",
         sol::resolve<uint32_t, CustomPainterImage&>(&Jkr::Renderer::Shape::CopyToImage));

    Jkr.new_usertype<Jkr::Renderer::Line>(
         "LineRenderer",
         sol::call_constructor,
         sol::factories(
              [](Jkr::Instance& inInstance, Jkr::WindowMulT& inWindow, Jkr::PainterCache& inCache) {
                  return mu<Jkr::Renderer::Line>(inInstance, inWindow, inCache);
              }),
         "Add",
         &Jkr::Renderer::Line::AddEXT,
         "Update",
         &Jkr::Renderer::Line::UpdateEXT,
         "Dispatch",
         &Jkr::Renderer::Line::Dispatch,
         "Bind",
         &Jkr::Renderer::Line::Bind,
         "Draw",
         &Jkr::Renderer::Line::DrawEXT);
}

inline void ShapeRendererResources::Add(Jkr::Instance& inInstance,
                                        FillType inFillType,
                                        PipelinePropertiesContext inContext,
                                        std::string_view inFileName,
                                        std::string_view inVertexShader,
                                        std::string_view inFragmentShader,
                                        std::string_view inComputeShader,
                                        bool inShouldLoad) {
    mCaches[inFillType] = mu<PainterCache>(inInstance, inContext);
    if (not inShouldLoad) {
        mCaches[inFillType]->Store(string(inFileName),
                                   string(inVertexShader),
                                   string(inFragmentShader),
                                   string(inComputeShader));
    } else {
        mCaches[inFillType]->Load(string(inFileName));
    }
}
} // namespace JkrEXE
#include "Instance.hpp"
#include <Renderers/BestText_Alt.hpp>
#include <sol/sol.hpp>

namespace JkrEXE {
void CreateTextRendererBindings(sol::state &inState) {
    auto Jkr = inState["Jkr"].get_or_create<sol::table>();
    using namespace Jkr::Renderer;
    ///@note this is used in AddTextureFromShapeImage in uniform3D (to get the mImgId)
    Jkr.new_usertype<BestText_Alt::ImageId>("BestText_AltImageId",
                                            sol::call_constructor,
                                            sol::default_constructor,
                                            "mRectId",
                                            &BestText_Alt::ImageId::mRectId,
                                            "mImgId",
                                            &BestText_Alt::ImageId::mImgId);
    Jkr.new_usertype<BestText_base>("BestText_base",
                                    sol::call_constructor,
                                    sol::factories([]() { return mu<BestText_base>(); }),
                                    "AddFontFace",
                                    &BestText_base::AddFontFaceEXT,
                                    "GetTextDimensions",
                                    &BestText_base::GetTextDimensionsEXT);

    Jkr.new_usertype<Renderer::BestText_Alt>(
         "BestText_Alt",
         sol::call_constructor,
         [](Instance &inInstance, Renderer::Shape &inShape, Renderer::BestText_base &inBestText) {
             return mu<BestText_Alt>(inInstance, inShape, inBestText);
         },
         "Add",
         &BestText_Alt::AddEXT,
         "Draw",
         &BestText_Alt::DrawEXT,
         "Update",
         &BestText_Alt::UpdateEXT,
         "UpdatePosOnly",
         &BestText_Alt::UpdatePosOnlyEXT,
         "WrapToTextVector",
         &BestText_Alt::WrapToTextVector);
}
} // namespace JkrEXE

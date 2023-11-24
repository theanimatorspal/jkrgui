#include "GridSheet.hpp"

void App::NodeSheet::Load()
{
    ab::Load();
    ab::SetDefaultBoundedRectangle();
    mNodeWorldTranslationMatrix = glm::identity<glm::mat4>();
    mNodeWorldTranslationMatrix = glm::translate(mNodeWorldTranslationMatrix, glm::vec3(ab::GetPosition() + ab::GetDimension() / 2.0f, 0.0f));
    mNodeWorldInitialDimenion = ab::GetDimension();
    mNodeWorldInitialPosition = ab::GetPosition();
}

void App::NodeSheet::Event()
{
    ab::UpdateDefaultBoundedRectangle();
    int numKeys;
    auto keystate = SDL_GetKeyboardState(&numKeys);

    auto delxy = e.GetRelativeMousePos();

    bool isAltKeyPressed = keystate[SDL_SCANCODE_LALT];
    bool isLeftButtonPressed = (SDL_BUTTON(SDL_BUTTON_LEFT) & e.GetMouseButtonValue()) != 0;

    glm::vec2 Offset2D(delxy.x, delxy.y);
    if (isLeftButtonPressed && isAltKeyPressed && ab::IsMouseOnTop()) {
        mNodeWorldTranslationMatrix = glm::translate(mNodeWorldTranslationMatrix, glm::vec3(Offset2D, 0.0f));
        mOffset2D += Offset2D;
    }

    auto mousexy = e.GetMousePos();

    uint32_t j = 0;
    for (auto& u : mNodeViews) {
        u->Event(mOffset2D + glm::vec2(mNodeWorldInitialPosition + mNodeWorldInitialDimenion / 2.0f));
        auto Offset = mOffset2D + glm::vec2(mNodeWorldInitialPosition + mNodeWorldInitialDimenion / 2.0f);

        for (int i = 0; i < u->GetInputCount(); i++) {
            bool isMouseWithinSlot = u->GetInputNodeSlotBoundedRect(Offset, i).IsPointWithin(mousexy);
            if (isMouseWithinSlot and isLeftButtonPressed) {
                mSelectedInputSlot = glm::uvec2(j, i);
            }
        }

        bool isMouseWithintSlot = u->GetOutputNodeSlotBoundedRect(Offset).IsPointWithin(mousexy);
        if (isMouseWithintSlot and isLeftButtonPressed) {
            mSelectedOutputSlot = j;
        }
        j++;
    }

    auto has_both = mSelectedOutputSlot.has_value() and mSelectedInputSlot.has_value();
    if (has_both and mSelectedOutputSlot.value() != mSelectedInputSlot.value().x) {
        AddConnection(mSelectedInputSlot.value().x, mSelectedOutputSlot.value(), mSelectedInputSlot.value().y);
        mSelectedInputSlot.reset();
        mSelectedOutputSlot.reset();
    }
    UpdateConnections();

    for (auto i = mNodeViews.begin(); i < mNodeViews.end(); i++) {
        for (auto j = mNodeViews.begin(); j < mNodeViews.end(); j++) {

            auto& I = *(*i);
            auto& J = *(*j);
            using namespace glm;
            bool withinX = IsWithinRange<float>(I.GetPosition().x - J.GetPosition().x, -30, 30);
            bool withinY = IsWithinRange<float>(I.GetPosition().y - J.GetPosition().y, -30, 10);
            bool within = withinX and withinY;

            if (within and i != j) {
                (*i)->SetPosition((*i)->GetPosition() + glm::vec2(50, 50));
            }
        }
    }
}

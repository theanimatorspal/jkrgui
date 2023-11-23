#include "GridSheet.hpp"

void App::GridSheet::Load()
{
    ab::Load();
    auto GridSheetValue = e.SetBoundedRect(this->GetPosition(), this->GetDimension(), this->GetDepthValue());
    auto mDepthValue = this->GetDepthValue();
    e.MoveDepthValueTowardsTheCamera();
    mBoundedRectId = e.SetBoundedRect(glm::uvec2(0, 0), glm::uvec2(100, 100), mDepthValue);
    const glm::uvec2 origin = { 0, 0 };
    mGridStartId = r.ln.GetCurrentNewLineId();

    int gW = 500 * mNumLineFactor;
    int gH = 500 * mNumLineFactor;

    for (int i = -gW; i <= gW; i += mGridSpacing) {
        r.ln.AddLine(
            glm::vec2(i, -gH),
            glm::vec2(i, gH),
            mDepthValue,
            mGridEndId);
    }

    for (int i = -gH; i <= gH; i += mGridSpacing) {
        r.ln.AddLine(
            glm::vec2(-gW, i),
            glm::vec2(gW, i),
            mDepthValue,
            mGridEndId);
    }

    r.ln.AddLine(
        glm::vec2(0, -gH),
        glm::vec2(0, gH),
        mDepthValue,
        mGridMainLinesId);

    uint32_t id;
    r.ln.AddLine(
        glm::vec2(-gW, 0),
        glm::vec2(gW, 0),
        mDepthValue, id);

    Jkr::Generator Rect(Jkr::Shapes::Rectangle, glm::uvec2(5, 5));
    auto xy = (glm::vec2(0, 0));
    r.sh.Add(Rect, xy.x, xy.y, mDepthValue, mCenterIndicatorId);
    e.MoveDepthValueTowardsTheCamera();
    mLinesTranslation = glm::identity<glm::mat4>();
    mLinesTranslation = glm::translate(mLinesTranslation, glm::vec3(ab::GetPosition() + ab::GetDimension() / 2.0f, 0.0f));
}

void App::GridSheet::Event()
{
    Jkr::BoundRect2D Rect { .mXy = ab::GetPosition(), .mWh = ab::GetDimension() };
    e.UpdateBoundRect(ab::GetDepthValue(), mBoundedRectId, Rect);
    int numKeys;
    auto keystate = SDL_GetKeyboardState(&numKeys);

    auto delxy = e.GetRelativeMousePos();

    bool isAltKeyPressed = keystate[SDL_SCANCODE_LALT];
    bool isLeftButtonPressed = (SDL_BUTTON(SDL_BUTTON_LEFT) & e.GetMouseButtonValue()) != 0;

    glm::vec2 Offset2D(delxy.x, delxy.y);
    if (isLeftButtonPressed && isAltKeyPressed && e.IsMouseWithinAtTopOfStack(mBoundedRectId, ab::GetDepthValue())) {
        mLinesTranslation = glm::translate(mLinesTranslation, glm::vec3(Offset2D, 0.0f));
        mOffset2D += Offset2D;
    }

    if (e.GetEventHandle().type == SDL_WINDOWEVENT_RESIZED || e.GetEventHandle().type == SDL_WINDOWEVENT_MAXIMIZED || e.GetEventHandle().type == SDL_WINDOWEVENT_MINIMIZED) {
        mLinesTranslation = glm::identity<glm::mat4>();
    }

    auto mousexy = e.GetMousePos();

    uint32_t j = 0;
    for (auto& u : mNodeViews) {
        u->Event(mOffset2D + glm::vec2(ab::GetPosition() + ab::GetDimension() / 2.0f));
        auto Offset = mOffset2D + glm::vec2(ab::GetPosition() + ab::GetDimension() / 2.0f);

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

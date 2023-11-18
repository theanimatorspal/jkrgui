#pragma once
#include "ButtonRect_base.hpp"

namespace Jkr::Component {
class BTextButtonRect : public ButtonRect_base {
    using bb = ButtonRect_base;

public:
    BTextButtonRect(_2d& inR, EventManager& inE)
        : ButtonRect_base(inR, inE)
    {
    }
    void Load()
    {
        r.bt.SetTextProperty(Jkr::Renderer::BestText_base::TextProperty { .H = Jkr::Renderer::BestText_base::AlignH::Left, .V = Jkr::Renderer::BestText_base::AlignV::Bottom });
        auto TextDimensions = r.bt.AddText(mStringView, mHPadding, mVPadding, bb::GetDepthValue() - 1, mId, mLength);
        ButtonRect_base::Load();
    }

    void DrawBestTexts()
    {
        r.bt.Draw(*bb::GetWindow(), glm::vec4(1.0f, 1.0f, 0.9f, 1.0f), bb::GetWindowWidth(), bb::GetWindowHeight(), mId, mLength, bb::GetTranslationMatrix());
    }

    void DrawShapes()
    {
        const auto Shape = bb::GetShapes()[0];
        r.sh.Draw(*bb::GetWindow(), bb::GetColor(), bb::GetWindowWidth(), bb::GetWindowHeight(), Shape[0], Shape[1], bb::GetTranslationMatrix());
    }

    SETTER SetText(std::string_view inString, uint32_t mStringMaxOverflowSize = 10) { 
		mUtilityString = std::string(inString.begin(), inString.end());
		mUtilityString.resize(mUtilityString.size() + mStringMaxOverflowSize, ' ');
		mStringView = mUtilityString;
    }

    void Update()
    {
        r.bt.SetTextProperty(Jkr::Renderer::BestText_base::TextProperty { .H = Jkr::Renderer::BestText_base::AlignH::Left, .V = Jkr::Renderer::BestText_base::AlignV::Bottom });
        auto TextDimensions = r.bt.UpdateText(mId, mStringView, mHPadding, mVPadding, bb::GetDepthValue() - 1);
    }

private:
    std::string_view mStringView;
    std::string mUtilityString;
    uint32_t mLength;
    uint32_t mId;

private:
    uint32_t mVPadding = 5;
    uint32_t mHPadding = 5;
};
}

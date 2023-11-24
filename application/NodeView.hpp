#pragma once
#include "GridSheet.hpp"
#include "Node.hpp"
#include <Components/Area_base.hpp>
#include <Components/FocusProxy.hpp>

namespace App {

using namespace Jkr::Renderer;
using namespace Jkr::Component;

constexpr glm::uvec2 NodeMainRectDimension(142, 32);
constexpr glm::uvec2 NodeSmallRectDimension(10, 10);
constexpr uint32_t Gap = 5;
constexpr glm::uvec2 NodeMainRectPosition(0, NodeSmallRectDimension.y + Gap);

class NodeView : public Node, public Area_base {
    using cb = Area_base;

public:
    NodeView(_2d& in2d, EventManager& inManager, Node& inNode, std::string_view inName, glm::vec2 inPosition, uint32_t inDepthValue)
        : Node(inNode)
        , Area_base(in2d, inManager)
        , mName(inName)
    {
        this->SetDepthValue(inDepthValue);
        this->SetPosition(inPosition);

        auto p = this->GetPosition();
        r.bt.SetTextProperty(Jkr::Renderer::BestText_base::TextProperty { .H = Jkr::Renderer::BestText_base::AlignH::Left, .V = Jkr::Renderer::BestText_base::AlignV::Bottom });

        Jkr::Generator Rectangle(Jkr::Shapes::Rectangle, NodeMainRectDimension);
        Jkr::Generator SmallRectangle(Jkr::Shapes::Rectangle, NodeSmallRectDimension);

        {
            uint32_t i = 0;
            uint32_t j = 0;
            for (uint32_t sx = 0; i < Node::GetInputCount(); sx += (Gap + NodeSmallRectDimension.x)) {
                if (i == 0) {
                    r.sh.Add(SmallRectangle, p.x + sx, p.y, this->GetDepthValue(), mShapeIdStart);
                } else {
                    r.sh.Add(SmallRectangle, p.x + sx, p.y, this->GetDepthValue(), mShapeIdEnd);
                }
                i++;
            }
            j += Gap + NodeSmallRectDimension.y;

            auto TextDimensions = r.bt.AddText(mName, p.x + mHPadding, p.y + mVPadding + NodeMainRectDimension.y / 2.0f, this->GetDepthValue(), mTextStartId, mTextLength);

            if (Node::GetInputCount() == 0) {
                r.sh.Add(Rectangle, p.x, p.y + j, this->GetDepthValue(), mShapeIdStart);
            } else {
                r.sh.Add(Rectangle, p.x, p.y + j, this->GetDepthValue(), mShapeIdEnd);
            }

            j += Gap + NodeMainRectDimension.y;
            r.sh.Add(SmallRectangle, p.x + NodeMainRectDimension.x / 2.0f, p.y + j, this->GetDepthValue(), mShapeIdEnd);
        }

        mBoundedRectId = e.SetBoundedRect(p + glm::vec2(0, Gap) + glm::vec2(NodeSmallRectDimension), NodeMainRectDimension, this->GetDepthValue());
    }

    void Event(glm::vec2 inOffsetXy)
    {
        auto p = this->GetPosition();
        e.UpdateBoundRect(mBoundedRectId, inOffsetXy + p + glm::vec2(0, Gap) + glm::vec2(0, NodeSmallRectDimension.y), NodeMainRectDimension, this->GetDepthValue());

        bool isLeftButtonPressed = (SDL_BUTTON(SDL_BUTTON_LEFT) & e.GetMouseButtonValue()) != 0;
        bool isRightButtonPressed = (SDL_BUTTON(SDL_BUTTON_RIGHT) & e.GetMouseButtonValue()) != 0;
        bool isMouseWithinAtTopOfStack = e.IsMouseWithinAtTopOfStack(mBoundedRectId, this->GetDepthValue());
        auto delxy = e.GetRelativeMousePos();

        if (isMouseWithinAtTopOfStack and isLeftButtonPressed) {
            isSelected = true;
        } else if (not isMouseWithinAtTopOfStack and not isLeftButtonPressed) {
            isSelected = false;
        }

        if (isRightButtonPressed and isMouseWithinAtTopOfStack)
        {
            isHardSelected = true;
        }
        else if (not isMouseWithinAtTopOfStack and isRightButtonPressed)
        {
            isHardSelected = false;
        }

        if (isSelected and isLeftButtonPressed) {
            this->SetPosition(p + glm::vec2(delxy.x, delxy.y));
        }

        auto ssid = mShapeIdStart;
        auto tsid = mTextStartId;
        r.bt.SetTextProperty(Jkr::Renderer::BestText_base::TextProperty { .H = Jkr::Renderer::BestText_base::AlignH::Left, .V = Jkr::Renderer::BestText_base::AlignV::Bottom });
        Jkr::Generator Rectangle(Jkr::Shapes::Rectangle, NodeMainRectDimension);
        Jkr::Generator SmallRectangle(Jkr::Shapes::Rectangle, NodeSmallRectDimension);
        {
            uint32_t i = 0;
            uint32_t j = 0;
            for (uint32_t sx = 0; i < Node::GetInputCount(); sx += (Gap + NodeSmallRectDimension.x)) {
                r.sh.Update(ssid++, SmallRectangle, p.x + sx, p.y, this->GetDepthValue());
                i++;
            }
            j += Gap + NodeSmallRectDimension.y;

            auto TextDimensions = r.bt.UpdateText(tsid, mName, p.x + mHPadding, p.y + mVPadding + NodeMainRectDimension.y / 2.0f, this->GetDepthValue());

            r.sh.Update(ssid++, Rectangle, p.x, p.y + j, this->GetDepthValue());

            j += Gap + NodeMainRectDimension.y;
            r.sh.Update(ssid++, SmallRectangle, p.x + NodeMainRectDimension.x / 2.0f, p.y + j, this->GetDepthValue());
        }
    }

    auto GetShapeId() const { return glm::uvec2(mShapeIdStart, mShapeIdEnd); }
    auto GetTextId() const { return glm::uvec2(mTextStartId, mTextLength); }
    auto IsSelected() const { return isSelected; }

    auto GetOutputNodeSlotPosition() const
    {
        auto p = this->GetPosition();
        uint32_t j = 0;
        j += Gap + NodeSmallRectDimension.y;
        j += Gap + NodeMainRectDimension.y;
        return glm::vec2(p.x + NodeMainRectDimension.x / 2.0f + NodeSmallRectDimension.x / 2.0f, p.y + j + NodeSmallRectDimension.y / 2.0f);
    }

    auto GetInputNodeSlotPosition(uint32_t inSlot) const
    {
        auto p = this->GetPosition();
        uint32_t i = 0;
        uint32_t sx = 0;
        for (sx = 0; i < inSlot; sx += (Gap + NodeSmallRectDimension.x)) {
            i++;
        }
        return glm::vec2(p.x + sx + NodeSmallRectDimension.x / 2.0f, p.y + NodeSmallRectDimension.y / 2.0f);
    }

    auto GetInputNodeSlotBoundedRect(glm::vec2 inOffset, uint32_t inSlot) const
    {
        auto p = GetInputNodeSlotPosition(inSlot) + inOffset;
        auto xy = p - glm::vec2(NodeSmallRectDimension.x / 2.0f, NodeSmallRectDimension.y / 2.0f);
        auto wh = NodeSmallRectDimension;
        return Jkr::BoundRect2D { .mXy = xy, .mWh = wh };
    }

    auto GetOutputNodeSlotBoundedRect(glm::vec2 inOffset) const
    {
        auto p = GetOutputNodeSlotPosition() + inOffset;
        auto xy = p - glm::vec2(NodeSmallRectDimension.x / 2.0f, NodeSmallRectDimension.y / 2.0f);
        auto wh = NodeSmallRectDimension;
        return Jkr::BoundRect2D { .mXy = xy, .mWh = wh };
    }

    static glm::vec<2, glm::vec2> GetConnection(NodeView* inInputNode, NodeView* inNode, uint32_t inSlot)
    {
        return glm::vec<2, glm::vec2>(inInputNode->GetInputNodeSlotPosition(inSlot), inNode->GetOutputNodeSlotPosition());
    }

    GETTER IsHardSelected() { return isHardSelected; }

private:
    bool isSelected = false;

private:
    bool isHardSelected = false;

private:
    uint32_t mHPadding = 5;
    uint32_t mVPadding = 5;
    glm::vec2 mPosition;
    std::string mName;

private:
    uint32_t mTextStartId;
    uint32_t mTextLength;
    uint32_t mShapeIdStart;
    uint32_t mShapeIdEnd;
    uint32_t mBoundedRectId;
};

};

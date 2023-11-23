#pragma once
#include "NodeView.hpp"
#include <Components/Area_base.hpp>
#include <numeric>

namespace App {
using namespace Jkr::Component;
class GridSheet : public Area_base {
    using ab = Area_base;
    std::vector<Up<NodeView>> mNodeViews;

    glm::vec2 mOffset2D = { 0.0f, 0.0f };

public:
    GridSheet(_2d& inR, EventManager& inE)
        : Area_base(inR, inE)
    {
    }

    GETTER GetSelection()
    {
        return std::find_if(mNodeViews.begin(), mNodeViews.end(), [&](Up<NodeView>& n) { return n->IsHardSelected(); });
    }

    bool IsSelectionEmpty()
    {
        auto isHardSelected = [](Up<NodeView>& n) { return n->IsHardSelected(); };
        if (std::any_of(mNodeViews.begin(), mNodeViews.end(), isHardSelected)) {
            return false;
        }
        return true;
    }

    void AddNodeView(Node& inNode, glm::vec2 inPos, std::string_view inName)
    {
        mNodeViews.push_back(MakeUp<NodeView>(r, e, inNode, inName, inPos, this->GetDepthValue()));
    }

    void Load();

    void AddConnection(uint32_t inNodeFemaleId, uint32_t inMaleNodeId, uint32_t inSlot)
    {
        auto line = NodeView::GetConnection(mNodeViews[inNodeFemaleId].get(), mNodeViews[inMaleNodeId].get(), inSlot);
        uint32_t id;
        r.ln.AddLine(line.x, line.y, this->GetDepthValue(), id);
        Connection connection = { .mNodeFemale = mNodeViews[inNodeFemaleId].get(), .mNodeMale = mNodeViews[inMaleNodeId].get(), .mSlot = inSlot, .mLineId = id };
        mConnections.push_back(connection);
        Node::ConnectNode(mNodeViews[inMaleNodeId].get(), mNodeViews[inNodeFemaleId].get(), inSlot);
    }

    void UpdateConnections()
    {
        for (auto& u : mConnections) {
            auto line = NodeView::GetConnection(u.mNodeFemale, u.mNodeMale, u.mSlot);
            r.ln.UpdateLine(u.mLineId, line.x, line.y, this->GetDepthValue());
        }
    }

private:
    std::optional<glm::uvec2> mSelectedInputSlot;
    std::optional<uint32_t> mSelectedOutputSlot;
    struct Connection {
        NodeView* mNodeFemale;
        NodeView* mNodeMale;
        uint32_t mSlot;
        uint32_t mLineId;
    };
    std::vector<Connection> mConnections;

public:
    void DrawOutline()
    {
        r.ln.Draw(*this->GetWindow(), glm::vec4(1.0f, 1.0f, 1.0f, 0.04f), ab::GetWindowWidth(), ab::GetWindowHeight(), this->GetLines()[0].x, this->GetLines()[0].y, ab::GetTranslationMatrix());
    }

    void DrawShapes()
    {
        if (not mNodeViews.empty()) {
            auto shapes_start = mNodeViews.front()->GetShapeId();
            auto shapes_end = mNodeViews.back()->GetShapeId();
            r.sh.Draw(*this->GetWindow(), glm::vec4(0.3f, 0.3f, 0.3f, 1.0f), ab::GetWindowWidth(), ab::GetWindowHeight(), shapes_start.x, shapes_end.y, mLinesTranslation);
        }
    }

    void DrawBestTexts()
    {
        if (not mNodeViews.empty()) {
            auto texts_start = mNodeViews.front()->GetTextId().x;
            auto texts_end = mNodeViews.back()->GetTextId().y + mNodeViews.back()->GetTextId().x - mNodeViews.front()->GetTextId().x;
            r.bt.Draw(*this->GetWindow(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), ab::GetWindowWidth(), ab::GetWindowHeight(), texts_start, texts_end, mLinesTranslation);
        }
    }

    void SetScissor() { mWindow->SetScissor(this->GetScissor()); }
    void DrawLines()
    {
        r.ln.Draw(*mWindow, glm::vec4(1.0f, 1.0f, 1.0f, 0.04f), ab::GetWindowWidth(), ab::GetWindowHeight(), mGridStartId, mGridEndId, mLinesTranslation);
        r.ln.Draw(*mWindow, glm::vec4(1.0f, 1.0f, 1.0f, 0.2f), ab::GetWindowWidth(), ab::GetWindowHeight(), mGridMainLinesId, mGridMainLinesId + 1, mLinesTranslation);
        if (not mConnections.empty())
            r.ln.Draw(*this->GetWindow(), glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), ab::GetWindowWidth(), ab::GetWindowHeight(), mConnections.front().mLineId, mConnections.back().mLineId, mLinesTranslation);
    }
    void ResetScissor() { mWindow->ResetScissor(); }
    void Update(Window* inWindow, uint32_t inW, uint32_t inH)
    {
        ab::Update();
        this->SetWindow(inWindow, inW, inH);
    }

    void Event();

private:
    glm::uvec2 mId;

private:
    uint32_t mGridSpacing = 10;
    uint32_t mNumLineFactor = 1;
    uint32_t mGridStartId;
    uint32_t mGridEndId;
    uint32_t mGridMainLinesId;
    uint32_t mCenterIndicatorId;
    uint32_t mBoundedRectId;
    glm::mat4 mLinesTranslation = glm::identity<glm::mat4>();

protected:
    class ToWc {
    public:
        ToWc(uint32_t inW, uint32_t inH)
            : mW(inW)
            , mH(inH)
        {
        }
        glm::vec2 operator()(glm::vec2 inC)
        {
            return inC + glm::vec2(mW / 2.f, mH / 2.f);
        }

    private:
        uint32_t mW, mH;
    };
    class TogWc {
    public:
        TogWc(uint32_t inW, uint32_t inH)
            : mW(inW)
            , mH(inH)
        {
        }
        glm::vec2 operator()(glm::vec2 inC)
        {
            return inC - glm::vec2(mW / 2.f, mH / 2.f);
        }

    private:
        uint32_t mW, mH;
    };
};
}
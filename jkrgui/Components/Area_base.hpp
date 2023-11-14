#pragma once
#include "Component_base.hpp"
#include <concepts>

namespace Jkr::Component {
class Area_base : public Component_base {
    using cb = Component_base;

public:
    Area_base(_2d& inR, EventManager& inE)
        : Component_base(inR, inE)
    {
    }
    void Load()
    {
        cb::CheckExpectations();
        auto d = cb::GetDimension();
        uint32_t id;
        r.ln.AddLine(glm::vec2(0, 0), glm::vec2(d.x, 0), cb::GetDepthValue(), id);
        uint32_t endId;
        r.ln.AddLine(glm::vec2(0, 0), glm::vec2(0, d.y), cb::GetDepthValue(), endId);
        r.ln.AddLine(glm::vec2(d.x, 0), glm::vec2(d.x, d.y), cb::GetDepthValue(), endId);
        r.ln.AddLine(glm::vec2(0, d.y), glm::vec2(d.x, d.y), cb::GetDepthValue(), endId);
        cb::GetLines().push_back(glm::uvec2(id, endId));
    }
    void Update()
    {
        auto line = cb::GetLines()[0];
        auto d = cb::GetDimension();
        uint32_t id;
        r.ln.UpdateLine(line.x++, glm::vec2(0, 0), glm::vec2(d.x, 0), cb::GetDepthValue());
        uint32_t endId;
        r.ln.UpdateLine(line.x++, glm::vec2(0, 0), glm::vec2(0, d.y), cb::GetDepthValue());
        r.ln.UpdateLine(line.x++, glm::vec2(d.x, 0), glm::vec2(d.x, d.y), cb::GetDepthValue());
        r.ln.UpdateLine(line.x++, glm::vec2(0, d.y), glm::vec2(d.x, d.y), cb::GetDepthValue());
    }
    GETTER GetScissor()
    {
        return vk::Rect2D(
            vk::Offset2D(cb::GetPosition().x, cb::GetPosition().y),
            vk::Extent2D(cb::GetDimension().x, cb::GetDimension().y));
    }

private:
};
}
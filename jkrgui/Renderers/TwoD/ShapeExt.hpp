#include "Shape.hpp"

namespace Jkr::Renderer {
struct ShapeExt : public Shape {
          ShapeExt(const Instance& inInstance,
                   Window& inCompatibleWindow,
                   std::unordered_map<FillType, Up<PainterCache>>& inPainterCaches,
                   uint32_t inVarDesCount = 5000)
              : Shape(inInstance, inCompatibleWindow, inPainterCaches, inVarDesCount) {}

      private:
          Up<VulkanDescriptorSet> mVarDesVulkanDescriptorSet;
          uint32_t mCurrentImageIndex;
};
} // namespace Jkr::Renderer
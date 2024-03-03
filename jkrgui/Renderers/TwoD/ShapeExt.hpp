#include "Shape.hpp"

namespace Jkr::Renderer {
	class ShapeExt : public Shape {
	public:
		ShapeExt(const Instance& inInstance, Window& inCompatibleWindow, std::unordered_map<FillType, Up<PainterCache>>& inPainterCaches, uint32_t inVarDesCount = 5000)
			: Shape(inInstance, inPainterCaches, inVarDesCount)
		{}
	private:
		Up<VulkanDescriptorSet> mVarDesVulkanDescriptorSet;
		uint32_t mCurrentImageIndex;
	};
}
#include "Neural/SimpleNN.hpp"
#include <Vulkan/VulkanBufferVMA.hpp>
#include <Config.hpp>

namespace Jkr::Neural::GPU {
	using namespace ksai;
	class Network : public Jkr::Neural::Network {
		using GPUBufferLocal = VulkanBufferVMA<ksai::BufferContext::Storage, ksai::MemoryType::DeviceLocal>;
		using GPUBufferCoherent = VulkanBufferVMA<ksai::BufferContext::Staging, ksai::MemoryType::HostVisibleAndCoherenet>;
	public:
		Network(std::vector<int> inTopology, float inLearningRate) : Jkr::Neural::Network(inTopology, inLearningRate) { }
	};
}
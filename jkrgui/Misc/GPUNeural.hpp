#include "Neural/SimpleNN.hpp"
#include <Config.hpp>
#include <Vulkan/VulkanBufferVMA.hpp>

namespace Jkr::Misc::GPUNeural {
using namespace ksai;
class Network : public Jkr::Neural::Network {
          using GPUBufferLocal = VulkanBufferVMA<ksai::BufferContext::Storage, ksai::MemoryType::DeviceLocal>;
          using GPUBufferCoherent = VulkanBufferVMA<ksai::BufferContext::Staging, ksai::MemoryType::HostVisibleAndCoherenet>;

      public:
          Network(std::vector<int> inTopology, float inLearningRate) : Jkr::Neural::Network(inTopology, inLearningRate) {}
};
} // namespace Jkr::Misc::GPUNeural
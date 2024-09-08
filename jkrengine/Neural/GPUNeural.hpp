#include "Neural/SimpleNN.hpp"
#include <Config.hpp>
#include <Vulkan/VulkanBufferVMA.hpp>
#include <Vulkan/Pipeline/VulkanPipeline.hpp>

namespace Jkr::Misc::GPUNeural {
using namespace ksai;
class Network : public Jkr::Neural::Network {
    using GPUBufferLocal      = VulkanBufferVMA;
    using GPUBufferCoherent   = VulkanBufferVMA;
    using ComputePipelineType = VulkanPipeline<1, PipelineContext::Compute>;

    public:
    Network(std::vector<int> inTopology, float inLearningRate)
        : Jkr::Neural::Network(inTopology, inLearningRate) {}
};
} // namespace Jkr::Misc::GPUNeural
#pragma once
#include "VulkanInstance.hpp"

namespace ksai {
class VulkanMessenger {
    public:
    operator vk::DebugUtilsMessengerEXT() const { return mMessenger; }
    VulkanMessenger(const VulkanInstance& inInstance);
    ~VulkanMessenger();

    private:
    const vk::Instance& mInstance;
    vk::DebugUtilsMessengerEXT mMessenger;
};

} // namespace ksai

#pragma once
#include "VulkanInstance.hpp"

namespace ksai {
class VulkanMessenger {
    public:
    operator vk::DebugUtilsMessengerEXT() const { return mMessenger; }
    VulkanMessenger(VulkanInstance &inInstance);
    ~VulkanMessenger();

    private:
    vk::Instance &mInstance;
    vk::DebugUtilsMessengerEXT mMessenger;
};

} // namespace ksai

#include "Window.hpp"

namespace Jkr {
class WindowMulT : public Window {
    WindowMulT(const Instance& inInstance, const sv inTitle, ui inHeight, ui inWidth)
        : Window(inInstance, inTitle, inHeight, inWidth)
    {
    }


    ui mNumThreads;
    std::vector<Up<VulkanCommandPool>> mThreadCmdPools;
};

} // namespace Jkr

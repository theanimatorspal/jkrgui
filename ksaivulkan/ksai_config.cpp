#include "ksai_config.hpp"
#include <Vendor/Tracy/tracy/Tracy.hpp>
#include <sstream>

void* operator new(std ::size_t count)
{
    auto ptr = malloc(count);
    TracyAlloc(ptr, count);
    return ptr;
}
void operator delete(void* ptr) noexcept
{
    TracyFree(ptr);
    free(ptr);
}

namespace ksai {
    void Log(const sv msg, const sv type) {
        static const std::unordered_map<sv, sv> colors = {
            { "INFO",    "\033[34m" },  // Blue
            { "WARNING", "\033[33m" },  // Yellow
            { "ERROR",   "\033[31m" },  // Red
            { "DEBUG",   "\033[36m" }   // Cyan
        };

        sv color = colors.contains(type) ? colors.at(type) : colors.at("INFO"); // Default to blue

        std::stringstream stream;
        stream << color << "[KSAI " << type << "]: " << msg << "\033[0m";
        ksai_print(stream.str().data());
    }
}
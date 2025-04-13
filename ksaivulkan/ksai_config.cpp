// #define TRACY_DELAYED_INIT 1
// #define TRACY_MANUAL_LIFETIME 1
// #define TRACY_ON_DEMAND 1
// #define TRACY_IMPORTS 1
#include "ksai_config.hpp"
#include <Vendor/Tracy/tracy/Tracy.hpp>
#include <sstream>

void *operator new(std ::size_t count) {
  auto ptr = malloc(count);
  TracyAlloc(ptr, count);
  return ptr;
}
void operator delete(void *ptr) noexcept {
  TracyFree(ptr);
  free(ptr);
}

namespace ksai {
void Log(sv msg, sv type) {
  sv color;
  if (type == "WARNING") {
    color = "\033[33m";
  } else if (type == "ERROR") {
    color = "\033[31m";
  } else if (type == "DEBUG") {
    color = "\033[36m";
  } else {
    color = "\033[32m";
  }

  std::stringstream stream;
  stream << color << "[KSAI " << type << "]: " << msg << "\033[0m";
  ksai_print(stream.str().data());
  if (type == "ERROR") {
    exit(-1);
  }
}
} // namespace ksai
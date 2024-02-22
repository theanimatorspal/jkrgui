#include <ksai_config.hpp>

#ifndef ANDROID
#else
#include <SDL.h>
#include <SDL_rwops.h>
#include <SDL_system.h>
#endif

namespace ksai {
using fileinputstream = std::ifstream;
using fileoutputstream = std::ofstream;
}

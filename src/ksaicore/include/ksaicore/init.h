#ifndef _KSAICORE_INIT_HPP_
#define _KSAICORE_INIT_HPP_

#ifdef __cplusplus
extern "C" {
#endif

// Don't use C++ in headers, limit it to your source files
// Just use C Features here, so that it is easy to create APIs (you can create bindings for any
// language that way) Since C ABI is supported everywhere and is stable, your library/software will
// outlive you. You can declare functions (with C signatures), structs (With only C features), No
// Namespaces, use your_libname_struct, your_libname_func and constants.

// Start here
#include <SDL3/SDL.h>
#include <ksaicore/memory.h>
typedef struct ksaicore_ctx {
    SDL_Window *window;
    SDL_GPUDevice *gpu_device;
} ksaicore_ctx;

ksaicore_ctx *ksaicore_get_global_ctx();
void ksaicore_init();
// End here

#ifdef __cplusplus
}
#endif

#endif // _KSAICORE_INIT_HPP_

#ifndef _KSAICORE_MEMORY_HPP_
#define _KSAICORE_MEMORY_HPP_

#ifdef __cplusplus
extern "C" {
#endif

// Don't use C++ in headers, limit it to your source files
// Just use C Features here, so that it is easy to create APIs (you can create bindings for any
// language that way) Since C ABI is supported everywhere and is stable, your library/software will
// outlive you. You can declare functions (with C signatures), structs (With only C features), No
// Namespaces, use your_libname_struct, your_libname_func and constants.

void ksaicore_ss();
void ksaicore_se();
void *ksaicore_malloc(size_t size);
void *ksaicore_realloc(void *memptr, size_t size);
void *ksaicore_calloc(size_t n, size_t size);
void ksaicore_free(void *ptr);

#ifdef __cplusplus
}
#endif

#endif // _KSAICORE_MEMORY_HPP_

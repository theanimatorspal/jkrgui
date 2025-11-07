#ifndef _KSAIENG_INIT_HPP_
#define _KSAIENG_INIT_HPP_

#ifdef __cplusplus
extern "C" {
#endif

// Don't use C++ in headers, limit it to your source files
// Just use C Features here, so that it is easy to create APIs (you can create bindings for any
// language that way) Since C ABI is supported everywhere and is stable, your library/software will
// outlive you. You can declare functions (with C signatures), structs (With only C features), No
// Namespaces, use your_libname_struct, your_libname_func and constants.
#include <ksaicore/init.h>
// Start here
// End here

#ifdef __cplusplus
}
#endif

#endif // _KSAIENG_INIT_HPP_

#include <SDL3/SDL.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <stack>
#include <vector>

struct Allocation {
    void *memory;
    size_t size;
    void (*destroyer)(void *);
};

struct Scope {
    std::vector<Allocation> allocations;
    bool active = true;
};

extern "C" {
static std::stack<Scope> g_scope_stack;

void ksaicore_ss() { g_scope_stack.push({}); }

void *ksaicore_malloc(size_t size) {
    if (g_scope_stack.empty() || !g_scope_stack.top().active) {
        fprintf(stderr, "ERROR: No active scope for malloc\n");
        std::abort();
    }

    void *mem = std::malloc(size);
    if (!mem) {
        fprintf(stderr, "ERROR: malloc failed for %zu bytes\n", size);
        std::abort();
    }

    g_scope_stack.top().allocations.push_back({mem, size, nullptr});
    return mem;
}

void *ksaicore_calloc(size_t n, size_t size) {
    if (g_scope_stack.empty() || !g_scope_stack.top().active) {
        fprintf(stderr, "ERROR: No active scope for calloc\n");
        std::abort();
    }

    void *mem = std::calloc(n, size);
    if (!mem) {
        fprintf(stderr, "ERROR: calloc failed for %zu elements of %zu bytes\n", n, size);
        std::abort();
    }

    g_scope_stack.top().allocations.push_back({mem, n * size, nullptr});
    return mem;
}

void *ksaicore_realloc(void *ptr, size_t new_size) {
    if (g_scope_stack.empty() || !g_scope_stack.top().active) {
        fprintf(stderr, "ERROR: No active scope for realloc\n");
        std::abort();
    }

    auto &allocs = g_scope_stack.top().allocations;
    auto it = std::find_if(allocs.begin(), allocs.end(),
                           [ptr](const Allocation &a) { return a.memory == ptr; });

    if (it != allocs.end()) {
        void *new_mem = std::realloc(ptr, new_size);
        if (!new_mem) {
            fprintf(stderr, "ERROR: realloc failed for %zu bytes\n", new_size);
            std::abort();
        }
        it->memory = new_mem;
        it->size = new_size;
        return new_mem;
    } else {
        void *new_mem = std::realloc(ptr, new_size);
        if (!new_mem) {
            fprintf(stderr, "ERROR: realloc failed for %zu bytes\n", new_size);
            std::abort();
        }
        allocs.push_back({new_mem, new_size, nullptr});
        return new_mem;
    }
}

void ksaicore_defer(void *ptr, void (*destroyer)(void *)) {
    if (g_scope_stack.empty() || !g_scope_stack.top().active) {
        fprintf(stderr, "ERROR: No active scope for defer\n");
        std::abort();
    }

    auto &allocs = g_scope_stack.top().allocations;
    auto it = std::find_if(allocs.begin(), allocs.end(),
                           [ptr](const Allocation &a) { return a.memory == ptr; });

    if (it != allocs.end()) {
        it->destroyer = destroyer;
    } else {
        allocs.push_back({ptr, 0, destroyer});
    }
}

void ksaicore_se() {
    if (g_scope_stack.empty()) {
        fprintf(stderr, "ERROR: No active scope to end\n");
        std::abort();
    }

    Scope &scope = g_scope_stack.top();
    if (!scope.active) {
        fprintf(stderr, "ERROR: Scope already ended\n");
        std::abort();
    }

    for (auto &alloc : scope.allocations) {
        if (alloc.destroyer)
            alloc.destroyer(alloc.memory);
        else if (alloc.memory)
            std::free(alloc.memory);
    }

    scope.allocations.clear();
    scope.active = false;
    g_scope_stack.pop();
}

void ksaicore_free(void *ptr) { return; }
}
#define _GNU_SOURCE
#include <stdlib.h>
#include <dlfcn.h>
#include <stddef.h>

void* malloc(size_t size) {
    static void* (*real_malloc)(size_t) = NULL;
    static int in_malloc = 0;

    if (!real_malloc) {
        if (in_malloc) {
            // Prevent recursion during dlsym itself
            return NULL;
        }

        in_malloc = 1;
        real_malloc = dlsym(RTLD_NEXT, "malloc");
        in_malloc = 0;

    }

    if (size > 1000) {
        return NULL;
    }

    return real_malloc(size);
}

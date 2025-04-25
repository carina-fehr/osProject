#define _GNU_SOURCE
#include <stdlib.h>
#include <dlfcn.h>
#include <stddef.h>
#include <stdio.h>

void* malloc(size_t size) {
    static void* (*real_malloc)(size_t) = NULL;

    if (!real_malloc) {
        real_malloc = dlsym(RTLD_NEXT, "malloc");
        if (!real_malloc) {
            fprintf(stderr, "Error: could not find original malloc\n");
            exit(1);
        }
    }

    if (size > 1000) {
        printf("[!] malloc blocked: %zu bytes too large\n", size);
        return NULL;
    }

    return real_malloc(size);
}

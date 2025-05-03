#define _GNU_SOURCE

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <stddef.h>

void* malloc(size_t size) {
    static void* (*real_malloc)(size_t) = NULL;
    static int in_malloc = 0;
    int random;


    if (!real_malloc) {
        if (in_malloc) {
            // Prevent recursion during dlsym itself
            return NULL;
        }

        in_malloc = 1;
        real_malloc = dlsym(RTLD_NEXT, "malloc"); //calls real malloc
        in_malloc = 0;

    }

    srand(time(0)); //generate random number on base of time  
    random = rand();

    printf("randomNumber %d\n", random );

    if (size > 15000) { //TODO: make random genarated sizes 
        return NULL;
    }

    return real_malloc(size);
}

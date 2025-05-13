#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>

static void* (*real_malloc)(size_t)=NULL;

static void mtrace_init(void)
{
    real_malloc = dlsym(RTLD_NEXT, "malloc"); //calls real malloc
    if (NULL == real_malloc) { //error if real malloc couldnt be called
        fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
    }
}

void *malloc(size_t size)
{
    if(real_malloc==NULL) { //if real malloc doesnt exist call it
        mtrace_init();
    }

    void *p = NULL;
    fprintf(stderr, "malloc(%zu) = ", size);
    p = real_malloc(size); //find free space
    fprintf(stderr, "%p\n", p);
    return p;
}

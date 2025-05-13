#define _GNU_SOURCE
// This library return adresses of allocated data but randomly return NULL
// if the size being allocated exeds RANDOM 
// The pool of numbers for a random generated number is set to 60000000
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

static void* (*real_malloc)(size_t)=NULL;

long get_random_uint() {
    long r;
    long fd = open("/dev/urandom", O_RDONLY);
    read(fd, &r, sizeof(r));
    close(fd);

    if (r < 0) {
        r = r * (-1);
    }

    r = r/1000;
    return r;  // always >= 0
}

static void mtrace_init(void)
{
    real_malloc = dlsym(RTLD_NEXT, "malloc"); //calls real malloc
    if (NULL == real_malloc) { //error if real malloc couldnt be called
        fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
    }
}

void *malloc(size_t size)
{
    long random;
    if(real_malloc==NULL) { //if real malloc doesnt exist call it
        mtrace_init();
    }

    random = get_random_uint(); //generater random number
    random = random % 60000000; // top limit how big rando can be, sweet spot for gnome-waether

    void *p = NULL;
    fprintf(stderr, "size = %lu\n ", size);
    fprintf(stderr, "random Number = %ld\n ", random);
    p = real_malloc(size); //find free space

    if (size > random ){
        return NULL; 
    }
    return p;
}

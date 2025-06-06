#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Hijacks the getchar function to replace certain letters (vowels) with random words
// How to run getchar.c:
// gcc -shared -fPIC -o  getchar.so getchar..c -ldl
// gcc -o test_getchar test_getchar.c
// LD_PRELOAD=./getchar.so ./test_getchar

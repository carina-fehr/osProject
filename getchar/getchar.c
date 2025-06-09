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

typedef int (*orig_getchar_type)(void);

static orig_getchar_type orig_getchar = NULL;

static const char *replacement = NULL;
static size_t replacement_pos = 0;

// Mapping vowels to replacement words
static const struct {
    char ch;
    const char *word;
} vowel_map[] = {
    {'a', "apple"},
    {'e', "elephant"},
    {'i', "iguana"},
    {'o', "orange"},
    {'u', "umbrella"},
};

int getchar(void) {
    if (!orig_getchar) {
        orig_getchar = (orig_getchar_type)dlsym(RTLD_NEXT, "getchar"); // use original getchar
        if (!orig_getchar) {
            fprintf(stderr, "Error: original getchar not found\n");
            exit(1);
        }
    }

    // Return next character from replacement word
    if (replacement && replacement[replacement_pos] != '\0') {
        int ch = replacement[replacement_pos++];
        if (replacement[replacement_pos] == '\0') {
            replacement = NULL;
            replacement_pos = 0;
        }
        return ch;
    }

    int c = orig_getchar();
    if (c == EOF) return c;

    // Check if character is a vowel to replace in created struct, if so replace
    // if not then return original character from the syscall getchar
    for (size_t i = 0; i < sizeof(vowel_map) / sizeof(vowel_map[0]); ++i) {
        if (c == vowel_map[i].ch) {
            replacement = vowel_map[i].word;
            replacement_pos = 0;
            return replacement[replacement_pos++];
        }
    }

    return c;
}


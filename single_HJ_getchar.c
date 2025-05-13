// V2 hijack for getchar()
#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>
#include <ctype.h>
#include <string.h>

// Supported keyboard layouts (lowercase only)
static const char* LAYOUTS[] = {
    "qwertyuiopasdfghjkl;'\\zxcvbnm,./",  // QWERTY 
    "qwertzuiopasdfghjklöäyxcvbnmü,.-",   // QWERTZ 
    "azertyuiopqsdfghjklmwxcvbn,;:!",      // AZERTY 
    "qwertzuiopasdfghjklöäyxcvbnmü,.-",    // CH DE 
    "qwertyuiopasdfghjkl;'\\zxcvbnm,./",   // CH FR
    NULL
};

// debugging checks
static const char* LAYOUT_NAMES[] = {
    "QWERTY (US)", "QWERTZ (German)", "AZERTY (French)",
    "Swiss-German", "Swiss-French", NULL
};

// what is the current layout
const char* detect_layout() {
    // In a real project, use: `setxkbmap -query` or `/etc/default/keyboard`
    // Here we default to QWERTY for demo purposes
    return LAYOUTS[0]; // Default to QWERTY
}

// Ignore keycap layout and convert to ABCDEFG/ abcdefg
char to_alphabetical(char c) {
    const char* layout = detect_layout();
    if (!layout) return c;

    char lower_c = tolower(c);
    const char* pos = strchr(layout, lower_c);
    if (pos) {
        int index = pos - layout;
        return 'a' + index; // Map to abcde...
    }
    return c; // Not a letter, leave unchanged
}

// V2 Hijacked getchar()
int getchar(void) {
    int (*original_getchar)(void) = dlsym(RTLD_NEXT, "getchar");
    int c = original_getchar();

    if (isalpha(c)) {
        char mapped = to_alphabetical(c);
        c = islower(c) ? mapped : toupper(mapped);
    }
    return c;
}

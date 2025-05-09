// hijack for getchar()
#define _GNU_SOURCE
#include <stdio.h>
#include <dlfcn.h>



int getchar(void) {
    int (*original_getchar)(void) = dlsym(RTLD_NEXT, "getchar");
    int c = original_getchar();
// Change functionality of KEY127 (Backspace) or KEY8 (Delete Key)
    if (c == 127 || c == 8) { 
    //Both Keystrokes will now add a newLine instead of letting you type backspace
        return '\n'; 
    }
    return c;
}

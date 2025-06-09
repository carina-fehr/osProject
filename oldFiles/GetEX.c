#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <ctype.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

// File for communication
#define LAYOUT_FILE "/tmp/layout_phase.log"

// ####### COMBINED GETCHAR #######
// in combination with the execve() hijack, turns KCapp into a time/layout based riddle where Firefox and Thunderbird Mail can't be used.
static int (*real_getchar)(void) = NULL;

const char *PHYSICAL_KEYS  = "qwertzuiopasdfghjklyxcvbnm";
const char *DEFAULT_LAYOUT = "qwertzuiopasdfghjklyxcvbnm";

// Helper function to find first hint
void build_abc_layout(char *out) {
    strcpy(out, "abcdefghijklmnopqrstuvwxyz"); //  mnbvcxy spells f i r e f o x in the Alphabetical layout
    // mnbvcxy -> firefox
    const char *phys = PHYSICAL_KEYS;
    const char *special = "mnbvcxy";
    const char *firefox = "firefox";
    for (int i = 0; i < 7; ++i) {
        char *p = strchr(phys, special[i]);
        if (p)
            out[p - phys] = firefox[i];
    }
}

// Helper function to find first hint
void build_zyx_layout(char *out) {
    strcpy(out, "zyxwvutsrqponmlkjihgfedcba"); // //  poiuztasdf spells t h u n d e r b i r d in the reverse Alphabetical layout
    const char *phys = PHYSICAL_KEYS;
    // poiuztrasdf -> thunderbird
    const char *pmap = "poiuztrasdf";
    const char *thunderbird = "thunderbird";
    for (int i = 0; i < 11; ++i) {
        char *p = strchr(phys, pmap[i]);
        if (p)
            out[p - phys] = thunderbird[i];
    }
}


// QWERTZ=1s, ABC=10s, ZYX=10s
int get_phase(time_t start_time) { // Three phases exist which ultimatelly decide how effective, the given hints are
    time_t now = time(NULL);
    int period = 21; // total time for one cycle.
    int t = (now - start_time) % period;
    if (t == 0) return 0;         // The standard layout will be displayed for 1 second, enough time to close the program
    else if (t >= 1 && t <= 10) return 1; // Alphabetical lasts 10 seconds, which is enough time to solve for the hint 
    else return 2;                // Reverse Alphabetical lasts 10 seconds, which is short enough to confuse the user
}
// logic for "communication" between getchar() and execve().
int getchar(void) {
    if (!real_getchar)
        real_getchar = dlsym(RTLD_NEXT, "getchar");

    static time_t start_time = 0;
    if (start_time == 0)
        start_time = time(NULL);

    int phase = get_phase(start_time);

    // write the current phase into the log file
    FILE *f = fopen(LAYOUT_FILE, "w");
    if (f) {
        fprintf(f, "%d", phase);
        fclose(f);
    }
// To let the user know on what he has to do / introduction after the hijack starts. ONLY DISPLAYED DURING HIJACK
    char current_layout[28];
    const char *layout_name = "You need to hit the right Keys to know what is going on. --> Three rows merged as one... the keys, not the code <--";
    if (phase == 1) {
        build_abc_layout(current_layout);
        layout_name = "ABC.. IS THE CURRENT LAYOUT. (mnbvcxy -> firefox)";
    } else if (phase == 2) {
        build_zyx_layout(current_layout);
        layout_name = "ZYX.. IS THE CURRENT LAYOUT. (poiuztr -> ? + asdf -> bird)";
    } else {
        strcpy(current_layout, DEFAULT_LAYOUT);
    }
    static int last_phase = -1;
    if (phase != last_phase) {
        fprintf(stderr, "\n\033[1;33m[--> %s]\033[0m\n> ", layout_name);
        fflush(stderr);
        last_phase = phase;
    }

    int c = real_getchar();
    if (c == EOF) return EOF;

    if (isalpha(c)) {
        char lower_c = tolower(c);
        char *key_ptr = strchr(PHYSICAL_KEYS, lower_c);
        if (key_ptr) {
            int key_index = key_ptr - PHYSICAL_KEYS;
            char mapped = current_layout[key_index];
            return isupper(c) ? toupper(mapped) : mapped;
        }
    }

    return c;
}

// ####### COMBINED EXECVE #######
typedef int (*orig_execve_f_type)(const char*, char*const[], char*const[]);

int execve(const char *pathname, char *const argv[], char *const envp[]) {
    int phase = 0;
    FILE *f = fopen(LAYOUT_FILE, "r");
    if (f) {
        fscanf(f, "%d", &phase);
        fclose(f);
    }

    // Decide what to block based on phase
    int block = 0;
    if (phase == 1 && strstr(pathname, "firefox")) {
        block = 1;
        fprintf(stderr, "\n\033[1;31m[BLOCKED] Solve the riddle in KCapp to launch Firefox!\033[0m\n");
    }
    if (phase == 2 && strstr(pathname, "thunderbird")) {
        block = 1;
        fprintf(stderr, "\n\033[1;31m[BLOCKED] Solve the riddle in KCapp to launch Thunderbird Mail!\033[0m\n");
    }

    if (block) {
        errno = EACCES; // deny permission
        return -1;
    }

    orig_execve_f_type orig_execve;
    orig_execve = (orig_execve_f_type)dlsym(RTLD_NEXT, "execve");
    return orig_execve(pathname, argv, envp);
}

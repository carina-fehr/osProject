#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>  // Added for signal handling

#define RED    "\033[1;31m"
#define YELLOW "\033[1;33m"
#define RESET  "\033[0m"

// a helper function which will check if the buffer ends with the desired string
int ends_with(const char *haystack, const char *needle) {
    size_t hlen = strlen(haystack);
    size_t nlen = strlen(needle);
    if (hlen < nlen) return 0;
    return (strcmp(haystack + hlen - nlen, needle) == 0);
}

int main() {
    signal(SIGINT, SIG_IGN);  // Closing with control C is impossible, since another hijack blocks this.
    printf(YELLOW "KCapp// Welcome to KCapp Checking program //KCapp\n" RESET);
    printf("Start typing to see mappings (press Ctrl+P to exit):\n");  // Control P is used to close the program.

    char buffer[128] = {0};
    int bufpos = 0;

    while (1) {
        int c = getchar();
        // logic for Control P exit
        if (c == EOF || c == 0x10)  // condition changed so Ctrl P is usable. 
            break;

        printf("The physical Keycap corresponds to %c (0x%x)\n", isprint(c) ? c : ' ', c);
        fflush(stdout);

        // relevant for the buffer
        if ((isprint(c) || c == '\n') && bufpos < (int)sizeof(buffer) - 1) {
            buffer[bufpos++] = tolower(c);
            buffer[bufpos] = 0;
        }

        // you have to create a newline to submit your "guess"
        if (c == '\n') {
            // no need for \n during the check.
            if (bufpos > 1) {
                buffer[bufpos - 1] = '\0';

                // pressing "mnbvcxy" in the right time, during the right layout, yields the first hint. 
                if (ends_with(buffer, "firefox")) {
                    printf(RED "Solved #1. Exit KCapp while the layout is changed. The app allowed to launch is dependant on the current layout. *Hint for the second riddle: The full name. Be fast.\n" RESET);
                }
                 // pressing "poiuztrasdf" in the right time, during the right layout, yields the second hint. 
                if (ends_with(buffer, "thunderbird")) {
                    printf(RED "Solved #2 Exit during ABC to open Thunderbird Mail. Exit during XYZ to open Firefox.\n" RESET);
                }

                buffer[bufpos - 1] = '\n'; // restore the buffer
            }
        }
    }
    return 0;
}

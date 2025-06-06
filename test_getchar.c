#include <stdio.h>

// Simple program to be able to test/simulate the getchar functionality from the terminal directly
int main() {
    int c;

    printf("Type characters (Ctrl+D to quit):\n");

    while ((c = getchar()) != EOF) {
        putchar(c);  // Echo back the character read
        fflush(stdout);
    }

    printf("\nGoodbye!\n");
    return 0;
}

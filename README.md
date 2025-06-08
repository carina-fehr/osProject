# OS project group 12
Hijack seven different syscall fuctions, along with three other libc functions/syscalls with LD_PRELOAD: 
- getchar() (tested on the simulation program test_getchar externally)
- libc functions: readdir(), stat(), openat() (added afterwards)
- execve()
- open()
- read()
- write()
- malloc()
- connect()

To run the OShomework.c file manually:
- `gcc -shared -fPIC -o OShomework.so OShomework.c -ldl`
- `export LD_PRELOAD=./OShomework.so`
- `idontexist` or `invalidcommand -abc` (should say "Finished execution: no errors")
- `hexchat` (should not work)
- `DISABLE_WRITE_PRANK = 1 geary` (should not work)
- `liferea` (should not work)
- `gnome-weather` (should not work)
- `bash` or `cat file.txt` (should show fake deletion in red writing)
- `touch openThis.txt` or `head openThis.txt` (should show hacked text)
- `man ls` (or any other man command; should show remote access) 
- `nano secrets.txt` (creates new file, writes in this)
- `nano preloadLib.c` (permission denied) 
- `w3m google.com` (should not work)
- `w3m youtube.com` (should not work)
- To check the log file for blocked connections: `cat /tmp/blocked.log` or `blocked2.log` for OShomework.c

To run OShomeworkBash.sh (permanent changes):
- IMPORTANT: read the note at the beginning of the file first!
- then run it like usual with: ./OShomeworkBash.sh
- has stat and other added functions in it, runs OShomework.c
- To check the log file for blocked connections: `cat /tmp/blocked.log` or `blocked2.log` for different address families
- To test stat and other newly added functions hijack for file hiding: bash after running, then ls should not show any .txt files in directory


To run getchar with test_getchar: read instructions at the beginning of the getchar file




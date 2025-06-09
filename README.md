# OS project group 12
Hijack seven different syscall fuctions, along with one extra libc function/syscall with LD_PRELOAD: 
- getchar() (tested on the simulation program test_getcharEda externally)
- readdir() (added afterwards)
- execve()
- open()
- read()
- write()
- malloc()
- connect()

To run the preloadLib.c file manually:
- `gcc -shared -fPIC -o preloadLib.so preloadLib.c -ldl`
- `export LD_PRELOAD=./preloadLib.so`

Then try the following commands to test functionality of preloadLib functions:
- `hexchat` (should show network error messages on terminal and on app)
- `DISABLE_WRITE_PRANK = 1 geary` (should show network error messages on terminal and on app)
- `liferea` (should show network error messages on terminal and on app)
- `gnome-weather` (should show network error messages on terminal and on app)
- `chmod +x file.txt` (should show fake deletion in red writing)
- `install build-essential` (sould show remote access detected)
- `touch openThis.txt` or `head openThis.txt` (should show hacked text)
- `man ls` (or any other man command; should show colorful text)
- `idontexist` or `invalidcommand -abc` (should say "Finished execution: no errors")
- `whoami` (should print username reversed)
- `cp secrets.txt s.txt` (does not work)
- `gedit secrets.txt` (creates new file, writes in this)
- `nano preloadLib.c` (permission denied) 
- `w3m google.com` (should say `Can't load ...`)
- `w3m youtube.com` (should say `Can't load ...`)
- `curl google.com` (should say `Couldn't connect to server` and show error messages on the terminal)
- `ls`or `ls -l` (should not show .txt files)

To check the log file for blocked connections: `cat /tmp/blocked.log` for IPv4/6 connections or `blocked2.log` for app related connections (disable pranks first with `unset LD_PRELOAD`)

To run OShomeworkNP.sh (non-permanent script that runs seven functions):
- to run this it needs to be sourced as follows: `source OShomeworkNP.sh`
- it can be reset by starting a new terminal session or with `unset LD_PRELOAD` on the current terminal

To run OShomework.sh (permanent script that runs seven functions):
- IMPORTANT: read the note at the beginning of the file first!
- then run it like usual with: ./OShomework.sh

To run getchar function on simulated program: 
- To run getcharEda with test_getcharEda: read instructions at the beginning of the getcharEda file



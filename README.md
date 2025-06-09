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

To run the OShomework.c file manually:
- `gcc -shared -fPIC -o OShomework.so OShomework.c -ldl`
- `export LD_PRELOAD=./OShomework.so`
- `hexchat` (should not work)
- `DISABLE_WRITE_PRANK = 1 geary` (should not work)
- `liferea` (should not work)
- `gnome-weather` (should not work)
- `chmod +x file.txt` (should show fake deletion in red writing)
- `install build-essential` (sould show remote access detected)
- `touch openThis.txt` or `head openThis.txt` (should show hacked text)
- `man ls` (or any other man command; should show colorful text)
- `idontexist` or `invalidcommand -abc` (should say "Finished execution: no errors")
- `whoami` (should print username reversed)
- `cp secrets.txt s.txt` (does not work)
- `gedit secrets.txt` (creates new file, writes in this)
- `nano preloadLib.c` (permission denied) 
- `w3m google.com` (should not work)
- `w3m youtube.com` (should not work)
- `curl google.com` (should not work)
- `ls`or `ls -l` (should not show .txt files)
- To check the log file for blocked connections: `cat /tmp/blocked.log` for IPv4/6 connections or `blocked2.log` for app related connections (disable prank first with `unset LD_PRELOAD`)

To run OShomeworkNP.sh (non-permanent):
- this bash script doesn't do any file hiding due to the non permanent changes
- to run this it needs to be sourced as follows: `source OShomeworkNP.sh`
- it can be reset by starting a new terminal session or with unset LD_PRELOAD on the current terminal

To run OShomework.sh (permanent changes):
- IMPORTANT: read the note at the beginning of the file first!
- then run it like usual with: ./OShomework.sh
- has extra function readdir added to it
- runs preloadLib.c
- To check the log file for blocked connections: `cat /tmp/blocked.log` or `blocked2.log` for different address families, app related or IP related (disable prank first with `unset LD_PRELOAD`)


To run getcharEda with test_getcharEda: read instructions at the beginning of the getcharEda file



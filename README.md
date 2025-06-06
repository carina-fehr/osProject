# OS project group 12
Hijack ten different fuctions with LD_PRELOAD: 
- getchar() (tested on the simulation program test_getchar externally)
- stat(), readdir(), openat() (added afterwards)
- execve()
- open()
- read()
- write()
- malloc()
- connect()

To run the combinedMRWOC.c file manually:
- `gcc -shared -fPIC -o combinedMRWOC.so combinedMRWOC.c -ldl`
- `export LD_PRELOAD=./combinedMRWOC.so`
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
- To check the log file for blocked connections: `cat /tmp/blocked.log` or `blocked2.log` for combinedRWOC.c

To run combinedMRWOCG.c with the bash script:
- delete previous combinedMRWOCG.so file
- `./install_invisible_blocker.sh` and then try the mentioned commands again here

To run install_invisible_blocker_upt.sh (permanent changes):
- IMPORTANT: read the note at the beginning of the file first!
- then run it like usual with: `./install_invisible_blocker_upt.sh`

(Latest version) To run OShomeworkBash.sh (permanent changes):
- IMPORTANT: read the note at the beginning of the file first!
- then run it like usual with: `./OShomeworkBash.sh
- has stat and added functions in it, runs OShomework.c
- To check the log file for blocked connections: `cat /tmp/blocked.log` or `blocked2.log` for different address families

To run getchar with test_getchar: read instructions at the beginning of the getchar file




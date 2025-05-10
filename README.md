# OS project group 12
Hijack seven different fuctions with LD_PRELOAD: 
- getchar()
- execve()
- open()
- read()
- write()
- malloc()
- connect()

To run the combinedMRWOC.c file manually:
- gcc -shared -fPIC -o combinedMRWOC.so combinedMRWOC.c -ldl
- export LD_PRELOAD=./combinedMRWOC.so bash and then idontexist (should say "Finished execution: no errors")
- hexchat (should not work)
- DISABLE_WRITE_PRANK = 1 geary (should not work)
- liferea (should not work)
- gnome-weather (should not work)
- bash (should show fake deletion in red writing)
- head secrets.txt or head openThis.txt (should show hacked text)
- w3m google.com (should not work)
- w3m youtube.com (should not work)
- To check the log file for blocked connections: cat /tmp/blocked.log or blocked2.log for combinedRWOC.c

To run combinedMRWOCG.c with the bash script:
- delete previous combinedMRWOCG.so file
- ./install_invisible_blocker.sh and then try the mentioned commands again here


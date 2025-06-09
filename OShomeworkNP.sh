#!/bin/bash

# Make sure this script is sourced on the terminal with "source OShomeworkNP.sh"
(return 0 2>/dev/null) || {
    echo "Please run this script with: source $0"
    exit 1
}

# find preload 
# SOURCE_SO=$(find ~/ -name preloadLib.so | grep -m 1 "preloadLib.so") 
# SOURCE_C=$(find ~/ -name preloadLib.c | grep -m 1 "preloadLib.c") 

# Compile the .so from the .c file
#gcc -shared -fPIC -o "$SOURCE_SO" "$SOURCE_C" -ldl

# Compile preloadLib.c into preloadLib.so
gcc -shared -fPIC -o preloadLib.so preloadLib.c -ldl

if [ $? -ne 0 ]; then
    echo "Compilation failed."
    return 1
fi

# Set LD_PRELOAD for current shell and alias
export LD_PRELOAD="$PWD/preloadLib.so"
alias geary='DISABLE_WRITE_PRANK=1 geary'
echo "LD_PRELOAD set to: $LD_PRELOAD"

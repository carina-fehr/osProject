#!/bin/bash

REPO_URL="https://raw.githubusercontent.com/carina-fehr/osProject/main"
C_FILE="preloadLib.c"
OUTPUT_LIB="preloadLib.so"

echo "[*] Lade C-Datei herunter von $REPO_URL/$C_FILE ..."
curl -s -o "$C_FILE" "$REPO_URL/$C_FILE"


echo "[*] Kompiliere die Library..."
gcc -fPIC -shared -o "$OUTPUT_LIB" "$C_FILE" -ldl

export LD_PRELOAD="$PWD/$OUTPUT_LIB"

(return 0 2>/dev/null) || {
    echo "[!] Hinweis: Damit LD_PRELOAD dauerhaft wirkt, führe das Skript mit:"
    echo "    source $0"
    return 0 2>/dev/null || exit 0
}

alias geary='DISABLE_WRITE_PRANK=1 geary'


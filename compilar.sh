#!/usr/bin/env bash
# Compila la GM en Linux:  ./compilar.sh
#
# Necesitas pawncc 3.10.10 (https://github.com/pawn-lang/compiler/releases/tag/v3.10.10)
# y YSI-Includes (https://github.com/pawn-lang/YSI-Includes).
#   PAWNCC   = ruta a pawncc        (por defecto: pawncc del PATH)
#   YSI_DIR  = carpeta con YSI_*    (por defecto: pawno/include)
# El binario de Linux es de 32 bits: en Debian/Ubuntu instala libc6:i386.
set -e
cd "$(dirname "$0")/gamemodes"

PAWNCC="${PAWNCC:-pawncc}"
YSI_DIR="${YSI_DIR:-../pawno/include}"

"$PAWNCC" -i../pawno/include -i"$YSI_DIR" -d3 -Z "-(+" "-;+" -osrp.amx srp.pwn
echo "[OK] Compilado: gamemodes/srp.amx"

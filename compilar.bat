@echo off
REM ==========================================================================
REM  Compila la GM en Windows. Doble clic sobre este archivo.
REM
REM  Necesitas (una sola vez):
REM    1. pawncc 3.10.10 para Windows:
REM       https://github.com/pawn-lang/compiler/releases/tag/v3.10.10
REM       Copia pawncc.exe y pawnc.dll dentro de la carpeta  pawno\
REM    2. YSI-Includes: https://github.com/pawn-lang/YSI-Includes
REM       Copia las carpetas YSI_Coding, YSI_Core, YSI_Data, YSI_Extra,
REM       YSI_Game, YSI_Players, YSI_Server, YSI_Storage y YSI_Visual
REM       dentro de  pawno\include\
REM
REM  Resultado: gamemodes\srp.amx
REM ==========================================================================
cd /d "%~dp0gamemodes"

if not exist "..\pawno\pawncc.exe" (
    echo [ERROR] No se encontro pawno\pawncc.exe. Lee las instrucciones al inicio de compilar.bat
    pause
    exit /b 1
)

"..\pawno\pawncc.exe" -i..\pawno\include -d3 -Z "-(+" "-;+" -osrp.amx srp.pwn
if errorlevel 1 (
    echo.
    echo [ERROR] La compilacion fallo. Mira arriba el archivo y la linea del error.
    echo         Ayuda con errores comunes: docs\GUIA.md, seccion "Errores comunes".
) else (
    echo.
    echo [OK] Compilado: gamemodes\srp.amx
)
pause

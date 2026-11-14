/*
    Punto de entrada del módulo de cuentas. Se incluye en srp.pwn después de
    core/main/functions (necesita sus helpers: getRandomSalt, IsValidEmail...).
*/
#if defined account_included
    #endinput
#endif
#define account_included

#include "core/account/account_query"
#include "core/account/account_data"
#include "core/account/account_auth"

// Autoguardado: y_timers ejecuta esta tarea por cada jugador conectado.
ptask Account_AutoSave[ACCOUNT_AUTOSAVE_INTERVAL](playerid)
{
    if (!PLAYER_TEMP[playerid][pt_USER_LOGGED]) return;
    if (gettime() - PLAYER_TEMP[playerid][pt_SAVE_ACCOUNT_TIME] < ACCOUNT_AUTOSAVE_MIN_GAP) return;

    Account_SyncLivePosition(playerid);
    Account_SaveAll(playerid);
}

/*
    ============================================================================
     API FÁCIL (funciones en español)
    ============================================================================

     Funciones cortas y en español para programar sin conocer las variables
     internas de la GM (PI, ACCOUNT_INFO, PLAYER_TEMP...).

     Ejemplo, en vez de:
         if (PI[playerid][ac_CASH] < 500) return SendNotification(playerid, "No tienes dinero.");
         GivePlayerCash(playerid, -500, true, true);

     escribes:
         if (!QuitarDinero(playerid, 500)) return Aviso(playerid, "No tienes dinero.");

     Todas las funciones son "stock": si no las usas, no ocupan espacio.
     Ejemplos de uso: gamemodes/personalizado/comandos.inc
    ============================================================================
*/
#if defined facil_included
    #endinput
#endif
#define facil_included

/* ---------------------------------------------------------------- Rangos */
// Se usan con EsAdmin() y con flags:comando(RANGO_...)
#define RANGO_USUARIO           CMD_USER
#define RANGO_AYUDANTE          CMD_HELPER
#define RANGO_AYUDANTE_MOD      CMD_HELPER_MODERATOR
#define RANGO_MODERADOR         CMD_MODERATOR
#define RANGO_ADMINISTRADOR     CMD_ADMINISTRATOR
#define RANGO_DESARROLLADOR     CMD_DEVELOPER

/* ---------------------------------------------------------------- Colores */
// Colores para Mensaje() / MensajeColor() (formato 0xRRGGBBAA)
#define COLOR_BLANCO        0xFFFFFFFF
#define COLOR_GRIS          0xD1D1D1FF
#define COLOR_ROJO          0xFF6633FF
#define COLOR_VERDE         0x98F442FF
#define COLOR_AZUL          0x90C3D4FF
#define COLOR_AMARILLO      0xFCE679FF
#define COLOR_MORADO        0xC2A2DAFF

/* ================================================================ JUGADOR */

// true si el jugador ya inició sesión (registrado o logeado).
stock bool:EstaLogeado(playerid)
{
    return IsPlayerConnected(playerid) && PLAYER_TEMP[playerid][pt_USER_LOGGED];
}

// Nombre de rol, sin guion bajo ("Juan Perez").
stock NombreJugador(playerid)
{
    new nombre[24];
    format(nombre, sizeof nombre, "%s", PLAYER_TEMP[playerid][pt_RP_NAME]);
    return nombre;
}

// Nombre de la cuenta, tal como entra al servidor ("Juan_Perez").
stock NombreCuenta(playerid)
{
    new nombre[24];
    format(nombre, sizeof nombre, "%s", PLAYER_TEMP[playerid][pt_NAME]);
    return nombre;
}

// ID de la cuenta en la base de datos (columna id de la tabla player).
stock IdCuenta(playerid)            return ACCOUNT_INFO[playerid][ac_ID];
stock NivelJugador(playerid)        return ACCOUNT_INFO[playerid][ac_LEVEL];
stock NivelAdmin(playerid)          return ACCOUNT_INFO[playerid][ac_ADMIN_LEVEL];
stock bool:EsVIP(playerid)          return ACCOUNT_INFO[playerid][ac_VIP] != 0;
stock ObtenerSkin(playerid)         return PI[playerid][ac_SKIN];

// true si el jugador tiene ese rango o uno mayor. Ej: EsAdmin(playerid, RANGO_MODERADOR)
stock bool:EsAdmin(playerid, rango = RANGO_AYUDANTE)
{
    return ACCOUNT_INFO[playerid][ac_ADMIN_LEVEL] >= rango;
}

/* ================================================================ DINERO */

stock ObtenerDinero(playerid)       return PI[playerid][ac_CASH];
stock ObtenerDineroBanco(playerid)  return PI[playerid][ac_BANK_MONEY];

// Da dinero en mano (se guarda en la BD al instante).
stock DarDinero(playerid, cantidad)
{
    if (cantidad <= 0) return 0;
    return GivePlayerCash(playerid, cantidad);
}

// Quita dinero en mano. Devuelve 0 (y no quita nada) si no le alcanza.
stock QuitarDinero(playerid, cantidad)
{
    if (cantidad <= 0) return 0;
    return GivePlayerCash(playerid, -cantidad, true, true);
}

// true si el jugador tiene al menos esa cantidad en mano.
stock bool:TieneDinero(playerid, cantidad)
{
    return PI[playerid][ac_CASH] >= cantidad;
}

/* ================================================================ VIDA / CHALECO */

stock Float:ObtenerVida(playerid)       return PI[playerid][ac_HEALTH];
stock Float:ObtenerChaleco(playerid)    return PI[playerid][ac_ARMOUR];

// Pone la vida exacta (0.0 - 100.0). Respeta el anticheat.
stock PonerVida(playerid, Float:vida)       return SetPlayerHealthEx(playerid, vida);

// Suma vida a la actual.
stock DarVida(playerid, Float:vida)         return GivePlayerHealthEx(playerid, vida);

stock PonerChaleco(playerid, Float:chaleco) return SetPlayerArmourEx(playerid, chaleco);

/* ================================================================ POSICIÓN */

// Lleva al jugador a un punto. Respeta el anticheat.
stock Teletransportar(playerid, Float:x, Float:y, Float:z, Float:angulo = 0.0, interior = 0, mundo = 0)
{
    return SetPlayerPosEx(playerid, x, y, z, angulo, interior, mundo);
}

// Ej: new Float:x, Float:y, Float:z; ObtenerPosicion(playerid, x, y, z);
stock ObtenerPosicion(playerid, &Float:x, &Float:y, &Float:z)
{
    return GetPlayerPos(playerid, x, y, z);
}

// true si "otro" está a menos de "distancia" metros de playerid (mismo mundo).
stock bool:EstaCerca(playerid, otro, Float:distancia = 5.0)
{
    if (!IsPlayerConnected(otro)) return false;
    if (GetPlayerVirtualWorld(playerid) != GetPlayerVirtualWorld(otro)) return false;

    new Float:x, Float:y, Float:z;
    GetPlayerPos(otro, x, y, z);
    return IsPlayerInRangeOfPoint(playerid, distancia, x, y, z) != 0;
}

/* ================================================================ ARMAS */

// Da un arma y la guarda en la BD. Ej: DarArma(playerid, 24, 50) -> Desert Eagle con 50 balas
stock DarArma(playerid, arma, balas)
{
    if (arma < 1 || arma >= sizeof WEAPON_INFO) return 0;
    if (!WEAPON_INFO[arma][weapon_info_AMMO]) balas = 1;

    new slot = WEAPON_INFO[arma][weapon_info_SLOT];
    new bool:ya_tenia = PLAYER_WEAPONS[playerid][slot][player_weapon_VALID] && PLAYER_WEAPONS[playerid][slot][player_weapon_DB_ID];

    GivePlayerWeaponEx(playerid, arma, balas);

    // Si ya había un arma guardada en ese slot se actualiza; si no, se crea la fila.
    if (ya_tenia)
    {
        mysql_format(srp_db, QUERY_BUFFER, sizeof QUERY_BUFFER, "UPDATE pweapons SET weaponid = %d, ammo = %d WHERE id = %d;", PLAYER_WEAPONS[playerid][slot][player_weapon_ID], PLAYER_WEAPONS[playerid][slot][player_weapon_AMMO], PLAYER_WEAPONS[playerid][slot][player_weapon_DB_ID]);
        mysql_tquery(srp_db, QUERY_BUFFER);
    }
    else RegisterNewPlayerWeapon(playerid, slot);
    return 1;
}

/* ================================================================ MENSAJES */

// Notificación en pantalla (la cajita de la GM). Admite ~r~ ~g~ ~w~ ~b~ ~y~.
stock Aviso(playerid, const texto[])
{
    SendNotification(playerid, texto);
    return 1;
}

// Mensaje en el chat, en blanco.
stock Mensaje(playerid, const texto[])
{
    return SendClientMessage(playerid, COLOR_BLANCO, texto);
}

// Mensaje en el chat con color. Ej: MensajeColor(playerid, COLOR_VERDE, "Listo")
stock MensajeColor(playerid, color, const texto[])
{
    return SendClientMessage(playerid, color, texto);
}

// Mensaje para todos los jugadores logeados.
stock MensajeATodos(color, const texto[])
{
    for (new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
    {
        if (EstaLogeado(i)) SendClientMessage(i, color, texto);
    }
    return 1;
}

// Mensaje para los jugadores cercanos (incluye al que lo envía).
stock MensajeCercano(playerid, color, const texto[], Float:radio = 20.0)
{
    new Float:x, Float:y, Float:z, mundo = GetPlayerVirtualWorld(playerid);
    GetPlayerPos(playerid, x, y, z);

    for (new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
    {
        if (!EstaLogeado(i) || GetPlayerVirtualWorld(i) != mundo) continue;
        if (IsPlayerInRangeOfPoint(i, radio, x, y, z)) SendClientMessage(i, color, texto);
    }
    return 1;
}

// Mensaje para el staff con rango igual o mayor al indicado.
stock MensajeAdmins(color, const texto[], rango = RANGO_AYUDANTE)
{
    for (new i = 0, j = GetPlayerPoolSize(); i <= j; i++)
    {
        if (EstaLogeado(i) && EsAdmin(i, rango)) SendClientMessage(i, color, texto);
    }
    return 1;
}

/* ================================================================ CUENTA */

// Guarda todo el progreso del jugador en la BD ahora mismo.
stock GuardarCuenta(playerid)
{
    Account_SyncLivePosition(playerid);
    return Account_SaveAll(playerid);
}

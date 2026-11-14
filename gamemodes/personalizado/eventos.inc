/*
    ============================================================================
     TUS EVENTOS
    ============================================================================

     Un "evento" es código que se ejecuta solo cuando pasa algo en el juego
     (alguien se conecta, inicia sesión, se registra...).

     Con "hook" agregas tu código al evento SIN modificar el original:
     la GM sigue haciendo lo suyo y además ejecuta lo que pongas aquí.

     Eventos útiles:
         hook OnPlayerLogin(playerid)       el jugador inició sesión
         hook OnPlayerRegister(playerid)    el jugador terminó de registrarse
         hook OnPlayerConnect(playerid)     se conectó (todavía NO inició sesión)
         hook OnPlayerDisconnect(playerid, reason)
         hook OnPlayerSpawn(playerid)
         hook OnPlayerDeath(playerid, killerid, reason)

     Los de abajo son ejemplos: cámbialos o bórralos.
    ============================================================================
*/
#if defined personalizado_eventos_included
    #endinput
#endif
#define personalizado_eventos_included

// Obligatorio en todo archivo que use "hook".
#include <YSI_Coding\y_hooks>


// Se ejecuta cada vez que un jugador inicia sesión.
hook OnPlayerLogin(playerid)
{
    if (EsVIP(playerid))
    {
        MensajeColor(playerid, COLOR_AMARILLO, "Gracias por ser VIP.");
    }
    return 1;
}


// Se ejecuta una sola vez, cuando la cuenta se acaba de crear.
hook OnPlayerRegister(playerid)
{
    new texto[144];
    format(texto, sizeof texto, "¡Bienvenido %s al servidor!", NombreJugador(playerid));
    MensajeATodos(COLOR_VERDE, texto);
    return 1;
}

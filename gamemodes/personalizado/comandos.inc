/*
    ============================================================================
     TUS COMANDOS
    ============================================================================

     Escribe aquí tus propios comandos. No hace falta tocar los archivos
     gigantes de core/ (commands.inc tiene más de 6000 líneas).

     Esqueleto de un comando:

         CMD:nombre(playerid, params[])
         {
             // lo que hace el comando
             return 1;
         }

       - "nombre" es lo que el jugador escribe después de la barra: /nombre
       - playerid = el jugador que escribió el comando
       - params   = lo que escribió después del nombre (los argumentos)
       - termina siempre con "return 1;"

     Si un comando es solo para el staff, pon arriba (antes del CMD):

         flags:nombre(RANGO_ADMINISTRADOR)

       Rangos: RANGO_AYUDANTE, RANGO_AYUDANTE_MOD, RANGO_MODERADOR,
               RANGO_ADMINISTRADOR, RANGO_DESARROLLADOR

     Funciones fáciles disponibles (todas en core/facil/facil.inc):
       Aviso, Mensaje, MensajeColor, MensajeATodos, MensajeCercano, MensajeAdmins,
       DarDinero, QuitarDinero, TieneDinero, ObtenerDinero, ObtenerDineroBanco,
       PonerVida, DarVida, PonerChaleco, ObtenerVida, Teletransportar,
       ObtenerPosicion, EstaCerca, DarArma, NombreJugador, NivelJugador,
       NivelAdmin, EsAdmin, EsVIP, EstaLogeado, GuardarCuenta

     ¡OJO! Antes de crear un comando revisa en docs/INDICE.md que el nombre
     no exista ya, o el compilador dará error de "símbolo ya definido".

     Los comandos de abajo son EJEMPLOS: funcionan, puedes probarlos,
     copiarlos, cambiarles el nombre o borrarlos.
    ============================================================================
*/
#if defined personalizado_comandos_included
    #endinput
#endif
#define personalizado_comandos_included


/* ----------------------------------------------------------------------------
    EJEMPLO 1: comando sin argumentos.  Uso: /misdatos
---------------------------------------------------------------------------- */
CMD:misdatos(playerid, params[])
{
    new texto[144];   // aquí se arma el texto (144 = largo máximo de un mensaje de chat)

    // format() rellena %s (texto) y %d (número) con los valores de la derecha, en orden.
    format(texto, sizeof texto, "Nombre: %s | Nivel: %d | Dinero: $%d | Banco: $%d",
        NombreJugador(playerid),
        NivelJugador(playerid),
        ObtenerDinero(playerid),
        ObtenerDineroBanco(playerid));

    MensajeColor(playerid, COLOR_AMARILLO, texto);
    return 1;
}


/* ----------------------------------------------------------------------------
    EJEMPLO 2: comando que cobra dinero.  Uso: /comprarbotiquin
---------------------------------------------------------------------------- */
#define PRECIO_BOTIQUIN 250    // cambiar el precio aquí

CMD:comprarbotiquin(playerid, params[])
{
    // QuitarDinero devuelve 0 si no le alcanza, y en ese caso no quita nada.
    if (!QuitarDinero(playerid, PRECIO_BOTIQUIN))
    {
        return Aviso(playerid, "~r~No tienes suficiente dinero.");
    }

    PonerVida(playerid, 100.0);
    Aviso(playerid, "~g~Te curaste por completo.");
    return 1;
}


/* ----------------------------------------------------------------------------
    EJEMPLO 3: comando de admin con argumentos.  Uso: /ejdardinero [id] [cantidad]

    sscanf lee los argumentos de params:
       "u" = un jugador (acepta ID o parte del nombre)
       "d" = un número entero
       "f" = un número con decimales
       "s[64]" = un texto de hasta 64 letras
    sscanf devuelve "verdadero" si FALTA algo -> ahí mostramos cómo se usa.
---------------------------------------------------------------------------- */
flags:ejdardinero(RANGO_ADMINISTRADOR)
CMD:ejdardinero(playerid, params[])
{
    new objetivo, cantidad;
    if (sscanf(params, "ud", objetivo, cantidad))
    {
        return Aviso(playerid, "~r~Uso: ~w~/ejdardinero [id] [cantidad]");
    }

    if (!EstaLogeado(objetivo)) return Aviso(playerid, "Ese jugador no está conectado.");
    if (cantidad <= 0 || cantidad > 1000000) return Aviso(playerid, "Cantidad no válida (1 - 1000000).");

    DarDinero(objetivo, cantidad);

    new texto[144];
    format(texto, sizeof texto, "Le diste $%d a %s.", cantidad, NombreJugador(objetivo));
    MensajeColor(playerid, COLOR_VERDE, texto);

    format(texto, sizeof texto, "Un administrador te dio $%d.", cantidad);
    MensajeColor(objetivo, COLOR_VERDE, texto);
    return 1;
}


/* ----------------------------------------------------------------------------
    EJEMPLO 4: comando que usa la posición.  Uso: /ejtraer [id]
---------------------------------------------------------------------------- */
flags:ejtraer(RANGO_MODERADOR)
CMD:ejtraer(playerid, params[])
{
    new objetivo;
    if (sscanf(params, "u", objetivo)) return Aviso(playerid, "~r~Uso: ~w~/ejtraer [id]");
    if (!EstaLogeado(objetivo)) return Aviso(playerid, "Ese jugador no está conectado.");

    new Float:x, Float:y, Float:z;
    ObtenerPosicion(playerid, x, y, z);

    // +1.0 en x para que no aparezca encima tuyo
    Teletransportar(objetivo, x + 1.0, y, z, 0.0, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
    Aviso(objetivo, "Un moderador te trajo a su posición.");
    return 1;
}


/* ----------------------------------------------------------------------------
    EJEMPLO 5: ver tus coordenadas.  Uso: /mipos
    Sirve para copiar posiciones a CONFIGURACION.inc o a cualquier tabla.
    También las imprime en la consola del servidor para copiarlas fácil.
---------------------------------------------------------------------------- */
flags:mipos(RANGO_AYUDANTE)
CMD:mipos(playerid, params[])
{
    new Float:x, Float:y, Float:z, Float:angulo;
    ObtenerPosicion(playerid, x, y, z);
    GetPlayerFacingAngle(playerid, angulo);

    new texto[144];
    format(texto, sizeof texto, "X: %.4f  Y: %.4f  Z: %.4f  Ángulo: %.1f  Interior: %d  Mundo: %d",
        x, y, z, angulo, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
    MensajeColor(playerid, COLOR_AMARILLO, texto);
    printf("[mipos] %s: %.4f, %.4f, %.4f, %.1f (int %d, mundo %d)", NombreCuenta(playerid), x, y, z, angulo, GetPlayerInterior(playerid), GetPlayerVirtualWorld(playerid));
    return 1;
}

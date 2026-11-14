/*
    ============================================================================
     CONFIGURACIÓN DEL SERVIDOR
    ============================================================================

     Este es el ÚNICO archivo que necesitas tocar para cambiar los valores
     básicos del servidor: nombre, colores, slots, dinero inicial, límites...

     Cómo se lee cada línea:

         #define NOMBRE    valor      // explicación

     Cambia solo el "valor". No cambies el NOMBRE: el resto de la GM lo usa.
     Los textos van entre comillas ("..."), los números sin comillas.
     Después de cambiar algo hay que volver a compilar srp.pwn.
    ============================================================================
*/
#if defined configuracion_included
    #endinput
#endif
#define configuracion_included

/* ---------------------------------------------------------------------------
    1. MODO DEBUG
       0 = producción (sin mensajes de depuración, código optimizado)
       1 = mensajes de debug solo en la consola
       2 = mensajes de debug en la consola y en el juego
--------------------------------------------------------------------------- */
#define DEBUG_MODE 2

/* ---------------------------------------------------------------------------
    2. DATOS DEL SERVIDOR
--------------------------------------------------------------------------- */
#define SERVER_SLOTS        500                     // máximo de jugadores (igual que "maxplayers" del server.cfg)
#define SERVER_VERSION      "3.0.0"
#define VERSION_DATE        "30/08/2020"
#define SERVER_NAME         "Super Roleplay"        // nombre que se ve en los mensajes
#define SERVER_SHORT_NAME   "Super"
#define SERVER_GAMEMODE     "SRP2 "SERVER_VERSION" - Roleplay español"   // texto de "modo" en la lista de servidores
#define SERVER_LANGUAGE     "Español - Spanish"
#define SERVER_WEBSITE      "yourwebsite"           // tu página web
#define SERVER_HOSTNAME     "Super Roleplay 2"      // nombre en la lista de servidores
#define SERVER_COIN         "Coins"                 // nombre de la moneda premium

/* ---------------------------------------------------------------------------
    3. COLORES (formato hex RRGGBB, sin #)
       Se usan dentro de los textos así: "{"#PRIMARY_COLOR"}Hola"
--------------------------------------------------------------------------- */
#define PRIMARY_COLOR   "fce679"    // amarillo principal
#define SILVER_COLOR    "d1d1d1"    // gris
#define RED_COLOR       "FF6633"    // rojo
#define GREEN_COLOR     "98f442"    // verde
#define BLUE_COLOR      "90C3D4"    // azul

/* ---------------------------------------------------------------------------
    4. CUENTAS: REGISTRO Y LOGIN
--------------------------------------------------------------------------- */
#define MIN_PASS_LENGTH         6       // largo mínimo de contraseña
#define MAX_PASS_LENGTH         18      // largo máximo de contraseña
#define MAX_BAD_LOGIN_ATTEMPS   3       // intentos de contraseña antes de expulsar
#define DINERO_INICIAL_MIN      5000    // dinero al registrarse (se elige al azar
#define DINERO_INICIAL_MAX      7000    //  entre MIN y MAX)

// Dónde aparece un jugador nuevo (y adónde se manda si su posición guardada no es válida).
// Para sacar coordenadas: entra al juego, ponte en el lugar y usa /mipos (staff).
#define SPAWN_NUEVO_X           1773.307250
#define SPAWN_NUEVO_Y           -1896.441040
#define SPAWN_NUEVO_Z           13.551166
#define SPAWN_NUEVO_ANGULO      270.0

#define ACCOUNT_AUTOSAVE_INTERVAL   600000  // autoguardado de cada jugador, en milisegundos (600000 = 10 min)
#define ACCOUNT_AUTOSAVE_MIN_GAP    60      // no autoguardar si se guardó hace menos de X segundos
#define ACCOUNT_CSAVE_COOLDOWN      30      // segundos de espera entre usos de /csave

// Verificación en dos pasos (necesita una API web propia)
#define SECURE_LOGIN_IP_DAYS        30      // días que se recuerda una IP verificada
#define SECURE_LOGIN_REQUEST_URL    "api.yourwebsite/auth/auth-request-code"
#define SECURE_LOGIN_CHECK_URL      "api.yourwebsite/auth/auth-check-code"
#define SECURE_LOGIN_TIME           180000  // tiempo para logearse antes del kick, en ms (180000 = 3 min)

/* ---------------------------------------------------------------------------
    5. NIVEL, REPUTACIÓN Y PAYDAY
--------------------------------------------------------------------------- */
#define REP_MULTIPLIER  3           // reputación necesaria para subir = nivel * este número
#define TIME_FOR_REP    1500000     // tiempo jugando para ganar 1 punto de reputación, en ms (1500000 = 25 min)
#define REP_FOR_PAYDAY  2           // cada cuántos puntos de reputación hay payday
#define PAYDAY_POR_NIVEL 300        // paga del gobierno = nivel * este número
#define PAYDAY_MAXIMO    4500       // paga máxima (sin el extra)
#define PAYDAY_EXTRA_MIN 100        // extra al azar que se suma a la paga
#define PAYDAY_EXTRA_MAX 200
#define CMD_LOGGIN      1           // 1 = imprimir en consola cada comando usado, 0 = no

/* ---------------------------------------------------------------------------
    6. LÍMITES POR JUGADOR
--------------------------------------------------------------------------- */
// Usuario normal
#define MAX_NU_VEHICLES         2
#define MAX_NU_PROPERTIES       1
#define MAX_NU_WORKS            1
#define MAX_NU_TOYS             3
#define MAX_NU_VOBJECTS         3
#define MAX_NU_PROPERTY_OBJECTS 150

// VIP
#define MAX_SU_VEHICLES         4
#define MAX_SU_PROPERTIES       4
#define MAX_SU_WORKS            3
#define MAX_SU_TOYS             6
#define MAX_SU_VOBJECTS         10
#define MAX_SU_PROPERTY_OBJECTS 250

/* ---------------------------------------------------------------------------
    7. SERVICIOS EXTERNOS (YouTube / MP3)
--------------------------------------------------------------------------- */
#define SEARCH_YT_PATH      "yt.yourwebsite/search"
#define DOWNLOAD_YT_PATH    "yt.yourwebsite/download"
#define DOWNLOADED_YT_PATH  "https://yt.yourwebsite/mp3/"

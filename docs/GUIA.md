# Guía para programar la GM (desde cero)

Esta guía es para que puedas cambiar la GM **sin saber programar todavía**. Léela en orden la primera vez; después úsala como consulta.

---

## 1. Empieza aquí: los 4 archivos que importan

| Archivo | Para qué sirve |
| :-- | :-- |
| [`gamemodes/CONFIGURACION.inc`](../gamemodes/CONFIGURACION.inc) | **Cambiar valores**: nombre del servidor, colores, slots, dinero inicial, lugar de aparición, payday, límites de vehículos o casas. Solo se cambian números y textos. |
| [`gamemodes/personalizado/comandos.inc`](../gamemodes/personalizado/comandos.inc) | **Tus comandos.** Trae 5 ejemplos explicados línea por línea. |
| [`gamemodes/personalizado/eventos.inc`](../gamemodes/personalizado/eventos.inc) | **Tu código que corre solo** (cuando alguien inicia sesión, se registra...). |
| [`docs/INDICE.md`](INDICE.md) | **Buscador**: todos los comandos, diálogos, variables y funciones, con enlace al archivo y la línea. |

Regla simple: **empieza siempre en `personalizado/`**. Solo entra a `core/` para leer o para cambiar algo que ya existe.

---

## 2. Mapa de carpetas

```
gamemodes/
├── srp.pwn                 Archivo principal: solo une todo (no hace falta tocarlo)
├── CONFIGURACION.inc       ★ valores del servidor
├── personalizado/          ★ TU código (comandos y eventos)
└── core/                   el código de la GM, separado por sistema
    ├── facil/              ★ funciones fáciles en español (DarDinero, Aviso...)
    ├── account/            cuentas: registro, login, guardado
    ├── main/               los archivos gigantes de la GM original:
    │     commands.inc        casi todos los comandos de jugador
    │     functions.inc       casi todas las funciones
    │     callbacks.inc       eventos (conectar, morir, spawn, teclas...)
    │     global_h.inc        tablas de datos: tiendas, cárceles, skins...
    ├── dialog/             ventanas de diálogo (ver sección 7)
    ├── administration/     comandos de staff, uno por rango
    ├── player/             definición de las variables del jugador
    ├── jobs/               trabajos
    ├── vehicles/ door/     vehículos (precios en door/door_h.inc → VEHICLE_INFO)
    ├── crew/               bandas y territorios
    ├── police/             policía
    ├── bank/               cajeros
    ├── properties/ furniture/   casas, hospitales, gasolineras, muebles
    ├── world/              entradas y salidas, zonas, mundo
    ├── anticheat/          anticheat
    └── sub_modules/        sistemas sueltos: inventario, robos, taxímetro, grafitis...
```

---

## 3. "Quiero cambiar..." → "Ve a..."

| Quiero cambiar... | Archivo | Qué buscar |
| :-- | :-- | :-- |
| Nombre del servidor, web, colores | `CONFIGURACION.inc` | sección 2 y 3 |
| Cantidad de slots | `CONFIGURACION.inc` | `SERVER_SLOTS` |
| Dinero con el que empieza un jugador | `CONFIGURACION.inc` | `DINERO_INICIAL_MIN` / `_MAX` |
| Dónde aparece un jugador nuevo | `CONFIGURACION.inc` | `SPAWN_NUEVO_X` ... (usa `/mipos` para ver tus coordenadas) |
| La paga del payday | `CONFIGURACION.inc` | `PAYDAY_POR_NIVEL`, `PAYDAY_MAXIMO` |
| Límite de autos, casas o trabajos | `CONFIGURACION.inc` | `MAX_NU_*` (normal) y `MAX_SU_*` (VIP) |
| Largo de contraseña o intentos de login | `CONFIGURACION.inc` | sección 4 |
| Precios del 24/7 | `core/main/global_h.inc` | `Supermarket_Product_List` |
| Armas y precios del mercado negro | `core/main/global_h.inc` | `BLACK_MARKT_WEAPONS` (nivel, id del arma, precio) |
| Skins para elegir al registrarse | `core/main/global_h.inc` | `Intro_Skins` |
| Precio y datos de cada vehículo | `core/door/door_h.inc` | `VEHICLE_INFO` |
| Trabajos (nivel necesario, pagas extra) | `core/jobs/jobs_h.inc` | `work_info` |
| Las reglas que se leen al registrarse | `core/directives/rules_h.inc` | `Server_Rules` |
| Entradas y salidas de interiores | `core/world/enter_exits_h.inc` | `ENTER_EXIT` |
| Un texto o mensaje que sale en el juego | — | Búscalo (sección 4) copiando una parte del texto |
| Cualquier comando que ya existe | `docs/INDICE.md` | Ctrl+F y escribe `/nombre` |

---

## 4. Cómo encontrar cualquier cosa

1. **Abre la carpeta del proyecto en VS Code** (Archivo → Abrir carpeta).
2. **Ctrl+Shift+F** busca en todos los archivos a la vez.
   - ¿Un texto del juego? Pega un trozo, por ejemplo `no tienes suficiente`.
   - ¿Un comando? Busca `CMD:nombre`.
   - ¿Dónde se usa una variable? Busca su nombre, por ejemplo `ac_CASH`.
3. **Ctrl+clic** sobre el nombre de una función te lleva a donde está escrita.
4. **[`docs/INDICE.md`](INDICE.md)** lista todo con enlaces directos. Si agregas cosas nuevas, regenéralo con:
   ```
   python3 tools/generar_indice.py
   ```

---

## 5. Las 3 variables grandes del jugador

Casi todo el código usa estas tres. Se escriben así: `VARIABLE[playerid][campo]`.

| Variable | Qué guarda | ¿Se guarda en la BD? | Ejemplo |
| :-- | :-- | :-- | :-- |
| `ACCOUNT_INFO` | Datos de la **cuenta**: nivel, rango de admin, VIP, email | **Sí** | `ACCOUNT_INFO[playerid][ac_LEVEL]` = nivel |
| `PI` | Datos del **personaje**: dinero, vida, posición, banco, teléfono, drogas | **Sí** | `PI[playerid][ac_CASH]` = dinero en mano |
| `PLAYER_TEMP` | Cosas **temporales** de la sesión: qué diálogo tiene abierto, si está trabajando... | **No**, se borra al salir | `PLAYER_TEMP[playerid][pt_USER_LOGGED]` = ya inició sesión |

- `playerid` es el número del jugador en el servidor (0, 1, 2...). Cambia cada vez que entra.
- `ACCOUNT_INFO[playerid][ac_ID]` es el número de su cuenta en la base de datos. Ese **no** cambia.
- La lista completa de campos, explicados uno por uno, está en [INDICE.md → Variables del jugador](INDICE.md#variables-del-jugador).

**Atajo:** en vez de usar estas variables directamente, usa las funciones de `core/facil/facil.inc`:

| En vez de... | Escribe... |
| :-- | :-- |
| `PI[playerid][ac_CASH]` | `ObtenerDinero(playerid)` |
| `GivePlayerCash(playerid, 500)` | `DarDinero(playerid, 500)` |
| `GivePlayerCash(playerid, -500, true, true)` | `QuitarDinero(playerid, 500)` |
| `ACCOUNT_INFO[playerid][ac_ADMIN_LEVEL] >= CMD_MODERATOR` | `EsAdmin(playerid, RANGO_MODERADOR)` |
| `PLAYER_TEMP[playerid][pt_RP_NAME]` | `NombreJugador(playerid)` |
| `SendNotification(playerid, "...")` | `Aviso(playerid, "...")` |
| `SetPlayerPosEx(playerid, x, y, z, a, int, vw)` | `Teletransportar(playerid, x, y, z)` |

---

## 6. Glosario de nombres raros

| Si ves... | Significa |
| :-- | :-- |
| `ac_...` | campo de `ACCOUNT_INFO` o `PI` (datos guardados) |
| `pt_...` | campo de `PLAYER_TEMP` (temporal) |
| `..._h.inc` | archivo de **declaraciones** (variables y tablas, sin lógica) |
| `CMD:x` | un comando `/x` |
| `flags:x(...)` | qué rango necesita el comando `/x` |
| `alias:x("y")` | `/y` hace lo mismo que `/x` |
| `DIALOG_...` | una ventana de diálogo |
| `hook X` | agrega código a un evento sin reemplazar el original |
| `inline ... MySQL_TQueryInline` | consulta a la base de datos; el código de adentro corre cuando llega la respuesta |
| `mysql_format` + `mysql_tquery` | armar y enviar una consulta a la base de datos |
| `srp_db` | la conexión a la base de datos |
| `QUERY_BUFFER` | texto compartido donde se arma la consulta |
| `Float:` | número con decimales (`100.0`) |
| `bool:` | `true` o `false` |
| `~r~ ~g~ ~w~ ~b~ ~y~` | colores en notificaciones (rojo, verde, blanco, azul, amarillo) |
| `{FF0000}` | color en mensajes de chat y diálogos |
| `SU` / `NU` en `MAX_SU_*` | VIP (super user) / usuario normal |
| `crew` | banda o facción |

---

## 7. Pawn en 10 minutos

```pawn
new dinero = 500;                 // "new" crea una variable (número)
new Float:vida = 100.0;           // número con decimales
new nombre[24];                   // texto de hasta 23 letras

if (dinero >= 100)                // si...
{
    dinero = dinero - 100;
}
else                              // si no...
{
    Aviso(playerid, "No te alcanza.");
}

// format: arma un texto. %d = número, %s = texto, %.2f = decimal con 2 dígitos
new texto[144];
format(texto, sizeof texto, "Tienes $%d y %.1f de vida", dinero, vida);

// return: termina la función. En comandos, siempre "return 1;"
return 1;
```

- Cada instrucción termina en `;`. En esta GM es opcional, pero ponlo siempre.
- `//` comenta hasta el final de la línea. `/* ... */` comenta un bloque.
- Las llaves `{ }` agrupan código. Cada `{` necesita su `}`.
- `==` compara, `=` asigna. `!=` es "distinto". `&&` es "y", `||` es "o", `!` es "no".

---

## 8. Recetas

### Crear un comando
En `personalizado/comandos.inc`, copia un ejemplo y cámbialo. Revisa antes en [INDICE.md](INDICE.md#comandos) que el nombre no exista.

### Hacer que algo pase al iniciar sesión
En `personalizado/eventos.inc`, dentro de `hook OnPlayerLogin(playerid)`.

### Agregar una ventana de diálogo
1. En `core/dialog/dialogs_id_h.inc`, debajo de la línea "Tus diálogos", agrega un nombre terminado en coma, por ejemplo `DIALOG_MI_MENU,`.
2. En `core/dialog/showDialog.inc`, agrega dentro del `switch`:
   ```pawn
   case DIALOG_MI_MENU: return ShowPlayerDialog(playerid, dialogid, DIALOG_STYLE_LIST, "Mi menú", "Opción 1\nOpción 2", "Elegir", "Cerrar");
   ```
3. En `core/dialog/response.inc`, agrega dentro del `switch`:
   ```pawn
   case DIALOG_MI_MENU:
   {
       if (!response) return 1;                     // apretó "Cerrar"
       if (listitem == 0) Aviso(playerid, "Elegiste la opción 1");
       if (listitem == 1) Aviso(playerid, "Elegiste la opción 2");
       return 1;
   }
   ```
4. Ábrelo desde un comando con `ShowDialog(playerid, DIALOG_MI_MENU);`.

### Agregar un dato que se guarde en la cuenta
Sigue los 4 pasos de [`docs/cuentas.md`](cuentas.md#agregar-un-dato-persistente).

---

## 9. Compilar

- **Windows:** doble clic en [`compilar.bat`](../compilar.bat). La primera vez lee las instrucciones al inicio del archivo: hay que descargar `pawncc` y YSI.
- **Linux:** `./compilar.sh`.

El resultado es `gamemodes/srp.amx`. Los **errores** impiden compilar. Los **warnings** compilan, pero avisan de algo raro y conviene arreglarlos.

### Errores comunes

| Mensaje | Qué pasa | Solución |
| :-- | :-- | :-- |
| `error 017: undefined symbol "x"` | usas un nombre que no existe | revisa mayúsculas y faltas de ortografía |
| `error 021: symbol already defined: "x"` | ese nombre ya existe (por ejemplo, un comando repetido) | cambia el nombre; búscalo en INDICE.md |
| `error 001: expected token: ";"` | falta un `;` o un paréntesis en la línea anterior | revisa la línea anterior a la indicada |
| `error 030: compound statement not closed` | falta una `}` | cuenta tus llaves |
| `error 035: argument type mismatch` | pasaste un tipo equivocado (texto en vez de número...) | revisa qué pide la función |
| `warning 213: tag mismatch` | mezclaste `Float:` con número entero | usa `100.0` en vez de `100` donde va decimal |
| `warning 219: local variable "x" shadows...` | una variable tuya se llama igual que otra global | cambia el nombre de la tuya |
| `warning 203: symbol is never used` | creaste algo que no usas | bórralo o úsalo |

El mensaje dice **archivo(línea)**. Por ejemplo, `personalizado/comandos.inc(57)` es la línea 57 de ese archivo.

---

## 10. Reglas de oro

1. **Haz una copia** (o un commit) antes de cambiar algo grande.
2. **Compila seguido**: cambia poco, compila, prueba. Así sabes qué rompió qué.
3. **Prueba en un servidor local**, no en el servidor con jugadores.
4. Textos que escribe el jugador, siempre con `%e` en las consultas a la BD, nunca con `%s` (evita hackeos por SQL injection).
5. Dinero, vida y posición: usa las funciones (`DarDinero`, `PonerVida`, `Teletransportar`), no cambies las variables a mano. El anticheat espera esas funciones y si no podría banear al jugador.

#!/usr/bin/env python3
"""
Genera docs/INDICE.md: un buscador de TODO lo que hay en la GM.

  - todos los comandos (/nombre), quién puede usarlos y en qué archivo y línea están
  - todos los diálogos y dónde se muestran / responden
  - todas las variables del jugador (PI, ACCOUNT_INFO, PLAYER_TEMP) explicadas
  - todas las funciones, agrupadas por archivo

Uso (desde la carpeta del proyecto):
    python3 tools/generar_indice.py

Vuelve a ejecutarlo cada vez que agregues comandos o funciones.
"""
import os
import re
import sys
from collections import defaultdict

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GM = os.path.join(ROOT, "gamemodes")
OUT = os.path.join(ROOT, "docs", "INDICE.md")

RANGOS = {
    "CMD_USER": "Usuario",
    "CMD_HELPER": "Ayudante",
    "CMD_HELPER_MODERATOR": "Ayudante-mod",
    "CMD_MODERATOR": "Moderador",
    "CMD_ADMINISTRATOR": "Administrador",
    "CMD_DEVELOPER": "Desarrollador",
    "CMD_DISABLED": "Deshabilitado",
    "RANGO_USUARIO": "Usuario",
    "RANGO_AYUDANTE": "Ayudante",
    "RANGO_AYUDANTE_MOD": "Ayudante-mod",
    "RANGO_MODERADOR": "Moderador",
    "RANGO_ADMINISTRADOR": "Administrador",
    "RANGO_DESARROLLADOR": "Desarrollador",
}

# Explicación en español de cada dato persistente del jugador.
DESCRIPCIONES = {
    # ACCOUNT_INFO
    "ac_ID": "ID de la cuenta en la BD (columna `id`)",
    "ac_IP": "IP de la última conexión",
    "ac_NAME": "Nombre de la cuenta (Nombre_Apellido)",
    "ac_EMAIL": "Correo electrónico",
    "ac_PASS": "Contraseña (hash SHA256, nunca en texto plano)",
    "ac_SALT": "Salt de la contraseña",
    "ac_LAST_CONNECTION": "Fecha de la última conexión (texto)",
    "ac_LAST_CONNECTION_TIMESTAMP": "Última conexión (timestamp unix)",
    "ac_TIME_PLAYING": "Tiempo total jugado",
    "ac_LEVEL": "Nivel del jugador (usa el de ACCOUNT_INFO)",
    "ac_REP": "Reputación (experiencia para subir de nivel)",
    "ac_STATE": "Estado de rol: normal, en interior, en propiedad, cárcel, hospital... (ROLEPLAY_STATE_*)",
    "ac_REG_DATE": "Fecha de registro",
    "ac_DOUBT_CHANNEL": "1 = ve el canal de dudas (/d)",
    "ac_TIME_FOR_REP": "Milisegundos que faltan para el siguiente punto de reputación",
    "ac_ADMIN_LEVEL": "Rango de staff (0 = usuario)",
    "ac_PAYDAY_REP": "Reputación acumulada para el próximo payday",
    "ac_VIP": "Nivel VIP (0 = no es VIP)",
    "ac_VIP_EXPIRE_DATE": "Fecha en que vence el VIP",
    # PI
    "ac_CONNECTED": "1 si está conectado (para la web/BD)",
    "ac_PLAYERID": "playerid actual (para la web/BD)",
    "ac_COINS": "Monedas premium (SERVER_COIN)",
    "ac_SKIN": "Skin (ropa) del personaje",
    "ac_CASH": "Dinero en mano",
    "ac_POS_X": "Posición X guardada",
    "ac_POS_Y": "Posición Y guardada",
    "ac_POS_Z": "Posición Z guardada",
    "ac_ANGLE": "Ángulo (hacia dónde mira)",
    "ac_INTERIOR": "Interior de GTA (0 = exterior)",
    "ac_LOCAL_INTERIOR": "ID del interior/propiedad de la GM donde está",
    "ac_FIGHT_STYLE": "Estilo de pelea",
    "ac_HEALTH": "Vida (0.0 - 100.0)",
    "ac_ARMOUR": "Chaleco (0.0 - 100.0)",
    "ac_GENDER": "Género: 0 hombre, 1 mujer",
    "ac_HUNGRY": "Hambre (100 = lleno)",
    "ac_THIRST": "Sed (100 = sin sed)",
    "ac_BLACK_MARKET_LEVEL": "Nivel en el mercado negro",
    "ac_WANTED_LEVEL": "Nivel de búsqueda policial",
    "ac_POLICE_JAIL_TIME": "Segundos de cárcel que le quedan",
    "ac_POLICE_DUTY": "1 si está de servicio como policía",
    "ac_POLICE_JAIL_ID": "En qué celda está",
    "ac_DRIVE_LICENSE_POINTS": "Puntos de la licencia de conducir",
    "ac_BANK_ACCOUNT": "Número de cuenta bancaria (0 = no tiene)",
    "ac_BANK_MONEY": "Dinero en el banco",
    "ac_PHONE_NUMBER": "Número de teléfono (0 = no tiene)",
    "ac_PHONE_STATE": "Teléfono encendido/apagado",
    "ac_PHONE_VISIBLE_NUMBER": "1 = muestra su número al llamar",
    "ac_GPS": "1 si tiene GPS",
    "ac_Maso": "Mazo (herramienta)",
    "ac_MP3": "1 si tiene MP3",
    "ac_PHONE_RESOLVER": "Guía telefónica",
    "ac_SPEAKERS": "Parlantes",
    "ac_MECHANIC_PIECES": "Piezas de mecánico",
    "ac_FUEL_DRUM": "Bidones de gasolina",
    "ac_SEED_MEDICINE": "Semillas de medicina",
    "ac_SEED_CANNABIS": "Semillas de marihuana",
    "ac_SEED_CRACK": "Semillas de crack",
    "ac_MEDICINE": "Medicina",
    "ac_CANNABIS": "Marihuana",
    "ac_CRACK": "Crack",
    "ac_CONFIG_SOUNDS": "Config: sonidos",
    "ac_CONFIG_AUDIO": "Config: audio/música",
    "ac_CONFIG_TIME": "Config: mostrar hora",
    "ac_CONFIG_HUD": "Config: mostrar HUD",
    "ac_CONFIG_ADMIN": "Config: ver mensajes de admin",
    "ac_CONFIG_SECURE_LOGIN": "Config: verificación en dos pasos",
    "ac_MUTE": "Tiempo de silencio (mute)",
    "ac_PLACA_PD": "Número de placa policial",
    "ac_CAN_BUY_BM": "Puede comprar en el mercado negro",
    "ac_CREW": "ID de la banda (0 = sin banda)",
    "ac_CREW_RANK": "Rango dentro de la banda",
    "ac_MECHANIC_KITS": "Kits de mecánico",
    "ac_MEDICAL_KITS": "Botiquines",
}


def leer(path):
    with open(path, encoding="utf-8", errors="replace") as f:
        return f.read().split("\n")


def archivos():
    for base, _, files in os.walk(GM):
        for name in sorted(files):
            if name.endswith((".inc", ".pwn")):
                yield os.path.join(base, name)


def rel(path):
    return os.path.relpath(path, ROOT).replace(os.sep, "/")


def link(path, line):
    return f"[`{rel(path)}:{line}`](../{rel(path)}#L{line})"


def leer_enum(lines, nombre):
    """Devuelve [(campo, tipo, comentario)] de 'enum nombre { ... }'."""
    out, dentro = [], False
    for l in lines:
        if not dentro:
            if re.match(rf"\s*enum\s+{nombre}\b", l):
                dentro = True
            continue
        if l.strip().startswith("}"):
            break
        m = re.match(r"\s*(?:(\w+):)?(\w+)(\[[^\]]*\])?\s*(?:=[^,]*)?,?\s*(?://\s*(.*))?$", l)
        if m and m.group(2) and not l.strip().startswith("//"):
            tipo = {"Float": "decimal", "bool": "sí/no"}.get(m.group(1), "texto" if m.group(3) else "número")
            out.append((m.group(2) + (m.group(3) or ""), tipo, (m.group(4) or "").strip()))
    return out


def main():
    comandos, flags, aliases = [], {}, defaultdict(list)
    dialog_show, dialog_resp = defaultdict(list), defaultdict(list)
    funciones = defaultdict(list)
    columnas = {}

    for path in archivos():
        lines = leer(path)
        for i, l in enumerate(lines, 1):
            m = re.match(r"CMD:(\w+)\s*\(", l)
            if m:
                comandos.append((m.group(1), path, i))
                continue
            m = re.match(r"flags:(\w+)\((\w+)\)", l)
            if m:
                flags[m.group(1)] = m.group(2)
                continue
            m = re.match(r"alias:(\w+)\((.*)\)", l)
            if m:
                aliases[m.group(1)] += re.findall(r'"(\w+)"', m.group(2))
                continue
            m = re.match(r"\s*case\s+(DIALOG_\w+)\s*:", l)
            if m:
                if path.endswith("showDialog.inc"):
                    dialog_show[m.group(1)].append((path, i))
                elif path.endswith("response.inc"):
                    dialog_resp[m.group(1)].append((path, i))
            m = re.search(r'cache_get_value_name(?:_int|_float)?\(0,\s*"(\w+)",\s*(\w+)\[playerid\]\[(ac_\w+)\]', l)
            if m:
                columnas.setdefault((m.group(2), m.group(3)), m.group(1))
            # Definición de función: empieza en columna 0 y la línea siguiente es "{"
            m = re.match(r"(?:stock\s+|static\s+|public\s+)*(?:(?:Float|bool|Text|PlayerText|MySQL):)?([A-Za-z_]\w*)\s*\(([^;]*)\)\s*$", l)
            if m and i < len(lines) and lines[i].strip() == "{" and m.group(1) not in ("if", "for", "while", "switch", "else"):
                funciones[path].append((m.group(1), m.group(2).strip(), i))

    dialog_enum = []
    for l in leer(os.path.join(GM, "core/dialog/dialogs_id_h.inc")):
        m = re.match(r"\s*(DIALOG_\w+)", l)
        if m:
            dialog_enum.append(m.group(1))

    acc = leer(os.path.join(GM, "core/player/player_account_h.inc"))
    tmp = leer(os.path.join(GM, "core/player/player_temp_h.inc"))

    o = []
    w = o.append
    w("# Índice de la GM")
    w("")
    w("> Archivo generado por `tools/generar_indice.py`. No lo edites a mano: vuelve a ejecutar el script.")
    w(">")
    w("> Consejo: usa **Ctrl+F** en esta página para buscar un comando, un diálogo o una función.")
    w("")
    w(f"- [Comandos](#comandos) ({len(comandos)})")
    w(f"- [Diálogos](#diálogos) ({len(dialog_enum)})")
    w("- [Variables del jugador](#variables-del-jugador)")
    w(f"- [Funciones por archivo](#funciones-por-archivo) ({sum(len(v) for v in funciones.values())})")
    w("")

    w("## Comandos")
    w("")
    w("| Comando | Alias | Quién puede usarlo | Dónde está |")
    w("| :-- | :-- | :-- | :-- |")
    for nombre, path, line in sorted(comandos, key=lambda c: c[0]):
        rango = RANGOS.get(flags.get(nombre, ""), flags.get(nombre, "Todos"))
        al = ", ".join("/" + a for a in aliases.get(nombre, []))
        w(f"| `/{nombre}` | {al} | {rango} | {link(path, line)} |")
    w("")

    w("## Diálogos")
    w("")
    w("Un diálogo se agrega en 3 pasos: el nombre en `core/dialog/dialogs_id_h.inc`, cómo se ve en `showDialog.inc` y qué pasa al responder en `response.inc`.")
    w("")
    w("| Diálogo | Cómo se ve (showDialog) | Qué hace al responder (response) |")
    w("| :-- | :-- | :-- |")
    for d in dialog_enum:
        s = "<br>".join(link(p, l) for p, l in dialog_show.get(d, [])) or "—"
        r = "<br>".join(link(p, l) for p, l in dialog_resp.get(d, [])) or "—"
        w(f"| `{d}` | {s} | {r} |")
    w("")

    w("## Variables del jugador")
    w("")
    w("Todas se usan como `VARIABLE[playerid][campo]`. Ejemplo: `PI[playerid][ac_CASH]` = dinero del jugador.")
    w("")
    for titulo, enum, var, explicacion in (
        ("ACCOUNT_INFO — datos de la cuenta (se guardan en la BD)", "account_Enum", "ACCOUNT_INFO", ""),
        ("PI — datos del personaje (se guardan en la BD)", "enum_PI", "PI", ""),
    ):
        w(f"### {titulo}")
        w("")
        w("| Campo | Tipo | Columna en la BD | Qué es |")
        w("| :-- | :-- | :-- | :-- |")
        otros = {c.split("[")[0] for c, _, _ in leer_enum(acc, "enum_PI" if var == "ACCOUNT_INFO" else "account_Enum")}
        for campo, tipo, com in leer_enum(acc, enum):
            base = campo.split("[")[0]
            desc = DESCRIPCIONES.get(base, com)
            col = columnas.get((var, base), "")
            if var == "PI" and base in otros and not col:
                desc = f"⚠️ Repetido y sin uso real: usa `ACCOUNT_INFO[playerid][{base}]`"
            w(f"| `{var}[playerid][{base}]` | {tipo} | {('`' + col + '`') if col else '—'} | {desc} |")
        w("")
    w("### PLAYER_TEMP — datos temporales (se borran al desconectar, NO se guardan)")
    w("")
    w("| Campo | Tipo | Comentario |")
    w("| :-- | :-- | :-- |")
    for campo, tipo, com in leer_enum(tmp, "Temp_Enum"):
        w(f"| `PLAYER_TEMP[playerid][{campo.split('[')[0]}]` | {tipo} | {com} |")
    w("")

    w("## Funciones por archivo")
    w("")
    for path in sorted(funciones, key=rel):
        w(f"### `{rel(path)}`")
        w("")
        for nombre, args, line in funciones[path]:
            w(f"- [`{nombre}({args})`](../{rel(path)}#L{line})")
        w("")

    os.makedirs(os.path.dirname(OUT), exist_ok=True)
    with open(OUT, "w", encoding="utf-8") as f:
        f.write("\n".join(o))
    print(f"OK: {rel(OUT)} ({len(comandos)} comandos, {len(dialog_enum)} diálogos)")


if __name__ == "__main__":
    sys.exit(main())

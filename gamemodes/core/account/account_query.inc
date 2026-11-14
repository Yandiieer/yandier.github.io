/*
    Constructor de UPDATE por columnas.

    En lugar de un único mysql_format con 70 "%d" y 70 argumentos que hay que
    mantener alineados a mano, cada columna se agrega con una línea:

        Q_Begin("player");
        Q_Int("cash", PI[playerid][ac_CASH]);
        Q_Float("pos_x", PI[playerid][ac_POS_X]);
        Q_Str("name", ACCOUNT_INFO[playerid][ac_NAME]);   // escapado con %e
        Q_Execute(ACCOUNT_INFO[playerid][ac_ID]);

    Agregar un campo nuevo = una línea aquí, sin contar posiciones de argumentos.
    Usa el buffer global QUERY_BUFFER.
*/
#if defined account_query_included
    #endinput
#endif
#define account_query_included

static bool:Q_FIRST_COLUMN;
static bool:Q_OVERFLOW;

static stock Q_Append(const part[])
{
    if (strlen(QUERY_BUFFER) + strlen(part) >= sizeof QUERY_BUFFER - 32)
    {
        Q_OVERFLOW = true;
        return 0;
    }
    if (!Q_FIRST_COLUMN) strcat(QUERY_BUFFER, ",", sizeof QUERY_BUFFER);
    Q_FIRST_COLUMN = false;
    strcat(QUERY_BUFFER, part, sizeof QUERY_BUFFER);
    return 1;
}

stock Q_Begin(const table[])
{
    format(QUERY_BUFFER, sizeof QUERY_BUFFER, "UPDATE `%s` SET ", table);
    Q_FIRST_COLUMN = true;
    Q_OVERFLOW = false;
    return 1;
}

stock Q_Int(const column[], value)
{
    new part[96];
    format(part, sizeof part, "`%s`=%d", column, value);
    return Q_Append(part);
}

stock Q_Float(const column[], Float:value)
{
    new part[96];
    format(part, sizeof part, "`%s`=%f", column, value);
    return Q_Append(part);
}

// Texto de usuario: siempre escapado.
stock Q_Str(const column[], const value[])
{
    new part[256];
    mysql_format(srp_db, part, sizeof part, "`%s`='%e'", column, value);
    return Q_Append(part);
}

// Valor ya seguro (NULL, números ya formateados, fechas entre comillas).
stock Q_Raw(const column[], const value[])
{
    new part[96];
    format(part, sizeof part, "`%s`=%s", column, value);
    return Q_Append(part);
}

stock Q_Execute(id)
{
    if (Q_OVERFLOW || Q_FIRST_COLUMN)
    {
        printf("[account] Q_Execute: query descartada (overflow=%d, sin columnas=%d) id=%d", Q_OVERFLOW, Q_FIRST_COLUMN, id);
        return 0;
    }

    new where[32];
    format(where, sizeof where, " WHERE `id`=%d;", id);
    strcat(QUERY_BUFFER, where, sizeof QUERY_BUFFER);

    #if DEBUG_MODE != 0
        Logger_Log(QUERY_BUFFER);
    #endif
    mysql_tquery(srp_db, QUERY_BUFFER);
    return 1;
}

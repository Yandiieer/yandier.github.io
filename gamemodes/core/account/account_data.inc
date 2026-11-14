/*
    Datos de la cuenta: valores por defecto, carga desde la BD y guardado.

    Para agregar un atributo persistente nuevo:
        1. Campo en enum_PI o account_Enum (player/player_account_h.inc).
        2. Columna en la tabla `player` (db/srp_db.sql + ALTER TABLE).
        3. Una línea en Account_LoadFromCache().
        4. Una línea Q_Int/Q_Float/Q_Str en SavePlayerData().
*/
#if defined account_data_included
    #endinput
#endif
#define account_data_included

SetPiDefaultValues(playerid)
{
    new tmp_PI[enum_PI];
    PI[playerid] = tmp_PI;

    new date[24];
    getDateTime(date);

    GetPlayerIp(playerid, ACCOUNT_INFO[playerid][ac_IP], 16);
    format(ACCOUNT_INFO[playerid][ac_NAME], 24, "%s", PLAYER_TEMP[playerid][pt_NAME]);
    format(ACCOUNT_INFO[playerid][ac_LAST_CONNECTION], 24, "%s", date);
    format(ACCOUNT_INFO[playerid][ac_REG_DATE], 24, "%s", date);
    ACCOUNT_INFO[playerid][ac_LAST_CONNECTION_TIMESTAMP] = gettime();
    ACCOUNT_INFO[playerid][ac_LEVEL] = 1;
    ACCOUNT_INFO[playerid][ac_REP] = 1;
    ACCOUNT_INFO[playerid][ac_TIME_FOR_REP] = TIME_FOR_REP;
    ACCOUNT_INFO[playerid][ac_STATE] = ROLEPLAY_STATE_NORMAL;
    ACCOUNT_INFO[playerid][ac_DOUBT_CHANNEL] = 1;
    PI[playerid][ac_CONNECTED] = 1;
    PI[playerid][ac_PLAYERID] = playerid;
    PI[playerid][ac_SKIN] = Intro_Skins[0][0];
    PI[playerid][ac_CASH] = minrand(5000, 7000);
    PI[playerid][ac_POS_X] = New_User_Pos[0];
    PI[playerid][ac_POS_Y] = New_User_Pos[1];
    PI[playerid][ac_POS_Z] = New_User_Pos[2];
    PI[playerid][ac_ANGLE] = New_User_Pos[3];
    PI[playerid][ac_FIGHT_STYLE] = 4;
    PI[playerid][ac_HEALTH] = 100.0;
    PI[playerid][ac_HUNGRY] = 100.0;
    PI[playerid][ac_THIRST] = 100.0;
    PI[playerid][ac_CONFIG_SOUNDS] = 1;
    PI[playerid][ac_CONFIG_AUDIO] = 1;
    PI[playerid][ac_CONFIG_TIME] = 1;
    PI[playerid][ac_CONFIG_HUD] = 1;
    PI[playerid][ac_CONFIG_ADMIN] = 1;
    PI[playerid][ac_CONFIG_SECURE_LOGIN] = 0;
    PI[playerid][ac_PHONE_VISIBLE_NUMBER] = 1;
    return 1;
}

// Lee la fila de `player` del cache actual (SELECT * FROM player ...) a ACCOUNT_INFO/PI.
Account_LoadFromCache(playerid)
{
    new bool:isnull_crew;
    cache_get_value_name(0, "reg_date", ACCOUNT_INFO[playerid][ac_REG_DATE], 24);
    cache_get_value_name(0, "last_connection", ACCOUNT_INFO[playerid][ac_LAST_CONNECTION], 24);
    cache_get_value_name_int(0, "last_connection_timestamp", ACCOUNT_INFO[playerid][ac_LAST_CONNECTION_TIMESTAMP]);
    cache_get_value_name_int(0, "time_playing", ACCOUNT_INFO[playerid][ac_TIME_PLAYING]);
    cache_get_value_name_int(0, "level", ACCOUNT_INFO[playerid][ac_LEVEL]);
    cache_get_value_name_int(0, "rep", ACCOUNT_INFO[playerid][ac_REP]);
    cache_get_value_name_int(0, "doubt_channel", ACCOUNT_INFO[playerid][ac_DOUBT_CHANNEL]);
    cache_get_value_name_int(0, "time_for_rep", ACCOUNT_INFO[playerid][ac_TIME_FOR_REP]);
    cache_get_value_name_int(0, "admin_level", ACCOUNT_INFO[playerid][ac_ADMIN_LEVEL]);
    cache_get_value_name_int(0, "payday_rep", ACCOUNT_INFO[playerid][ac_PAYDAY_REP]);
    cache_get_value_name_int(0, "vip", ACCOUNT_INFO[playerid][ac_VIP]);
    cache_get_value_name(0, "vip_expire_date", ACCOUNT_INFO[playerid][ac_VIP_EXPIRE_DATE], 24);
    cache_get_value_name_int(0, "coins", PI[playerid][ac_COINS]);
    cache_get_value_name_int(0, "skin", PI[playerid][ac_SKIN]);
    cache_get_value_name_int(0, "cash", PI[playerid][ac_CASH]);
    cache_get_value_name_float(0, "pos_x", PI[playerid][ac_POS_X]);
    cache_get_value_name_float(0, "pos_y", PI[playerid][ac_POS_Y]);
    cache_get_value_name_float(0, "pos_z", PI[playerid][ac_POS_Z]);
    cache_get_value_name_float(0, "angle", PI[playerid][ac_ANGLE]);
    cache_get_value_name_int(0, "state", ACCOUNT_INFO[playerid][ac_STATE]);
    cache_get_value_name_int(0, "interior", PI[playerid][ac_INTERIOR]);
    cache_get_value_name_int(0, "local_interior", PI[playerid][ac_LOCAL_INTERIOR]);
    cache_get_value_name_int(0, "fight_style", PI[playerid][ac_FIGHT_STYLE]);
    cache_get_value_name_float(0, "health", PI[playerid][ac_HEALTH]);
    cache_get_value_name_float(0, "armour", PI[playerid][ac_ARMOUR]);
    cache_get_value_name_int(0, "gender", PI[playerid][ac_GENDER]);
    cache_get_value_name_float(0, "hungry", PI[playerid][ac_HUNGRY]);
    cache_get_value_name_float(0, "thirst", PI[playerid][ac_THIRST]);
    cache_get_value_name_int(0, "black_market_level", PI[playerid][ac_BLACK_MARKET_LEVEL]);
    cache_get_value_name_int(0, "wanted_level", PI[playerid][ac_WANTED_LEVEL]);
    cache_get_value_name_int(0, "police_jail_time", PI[playerid][ac_POLICE_JAIL_TIME]);
    cache_get_value_name_int(0, "police_duty", PI[playerid][ac_POLICE_DUTY]);
    cache_get_value_name_int(0, "police_jail_id", PI[playerid][ac_POLICE_JAIL_ID]);
    cache_get_value_name_int(0, "drive_license_points", PI[playerid][ac_DRIVE_LICENSE_POINTS]);
    cache_get_value_name_int(0, "bank_account", PI[playerid][ac_BANK_ACCOUNT]);
    cache_get_value_name_int(0, "bank_money", PI[playerid][ac_BANK_MONEY]);
    cache_get_value_name_int(0, "phone_number", PI[playerid][ac_PHONE_NUMBER]);
    cache_get_value_name_int(0, "phone_state", PI[playerid][ac_PHONE_STATE]);
    cache_get_value_name_int(0, "phone_visible_number", PI[playerid][ac_PHONE_VISIBLE_NUMBER]);
    cache_get_value_name_int(0, "gps", PI[playerid][ac_GPS]);
    cache_get_value_name_int(0, "maso", PI[playerid][ac_Maso]);
    cache_get_value_name_int(0, "mp3", PI[playerid][ac_MP3]);
    cache_get_value_name_int(0, "phone_resolver", PI[playerid][ac_PHONE_RESOLVER]);
    cache_get_value_name_int(0, "speakers", PI[playerid][ac_SPEAKERS]);
    cache_get_value_name_int(0, "mechanic_pieces", PI[playerid][ac_MECHANIC_PIECES]);
    cache_get_value_name_int(0, "fuel_drum", PI[playerid][ac_FUEL_DRUM]);
    cache_get_value_name_int(0, "seed_medicine", PI[playerid][ac_SEED_MEDICINE]);
    cache_get_value_name_int(0, "seed_cannabis", PI[playerid][ac_SEED_CANNABIS]);
    cache_get_value_name_int(0, "seed_crack", PI[playerid][ac_SEED_CRACK]);
    cache_get_value_name_int(0, "medicine", PI[playerid][ac_MEDICINE]);
    cache_get_value_name_int(0, "cannabis", PI[playerid][ac_CANNABIS]);
    cache_get_value_name_int(0, "crack", PI[playerid][ac_CRACK]);
    cache_get_value_name_int(0, "config_sounds", PI[playerid][ac_CONFIG_SOUNDS]);
    cache_get_value_name_int(0, "config_audio", PI[playerid][ac_CONFIG_AUDIO]);
    cache_get_value_name_int(0, "config_time", PI[playerid][ac_CONFIG_TIME]);
    cache_get_value_name_int(0, "config_hud", PI[playerid][ac_CONFIG_HUD]);
    cache_get_value_name_int(0, "config_admin", PI[playerid][ac_CONFIG_ADMIN]);
    cache_get_value_name_int(0, "config_secure_login", PI[playerid][ac_CONFIG_SECURE_LOGIN]);
    cache_get_value_name_int(0, "mute", PI[playerid][ac_MUTE]);
    cache_get_value_name_int(0, "placa_pd", PI[playerid][ac_PLACA_PD]);
    cache_get_value_name_int(0, "can_buy_bm", PI[playerid][ac_CAN_BUY_BM]);
    cache_is_value_name_null(0, "crew", isnull_crew);
    if (!isnull_crew) cache_get_value_name_int(0, "crew", PI[playerid][ac_CREW]);
    cache_get_value_name_int(0, "crew_rank", PI[playerid][ac_CREW_RANK]);
    cache_get_value_name_int(0, "mechanic_kits", PI[playerid][ac_MECHANIC_KITS]);
    cache_get_value_name_int(0, "medical_kits", PI[playerid][ac_MEDICAL_KITS]);
    return 1;
}

/*
    Guarda el estado de la cuenta y el personaje.
    Ya NO guarda email/salt/pass en cada guardado: esos campos se escriben solo
    cuando cambian (registro, cambio de contraseña, cambio de correo), así un
    guardado rutinario nunca puede pisar una contraseña recién cambiada.
*/
SavePlayerData(playerid)
{
    if (!ACCOUNT_INFO[playerid][ac_ID]) return 0;

    new crew[16], vip_expire_date[32];

    crew = "NULL";
    if (PI[playerid][ac_CREW]) format(crew, sizeof crew, "%d", PI[playerid][ac_CREW]);

    vip_expire_date = "'0000-00-00 00:00:00'";
    if (strlen(ACCOUNT_INFO[playerid][ac_VIP_EXPIRE_DATE]) > 3) mysql_format(srp_db, vip_expire_date, sizeof vip_expire_date, "'%e'", ACCOUNT_INFO[playerid][ac_VIP_EXPIRE_DATE]);

    Q_Begin("player");
    // Cuenta
    Q_Str("name", ACCOUNT_INFO[playerid][ac_NAME]);
    Q_Str("ip", ACCOUNT_INFO[playerid][ac_IP]);
    Q_Str("last_connection", ACCOUNT_INFO[playerid][ac_LAST_CONNECTION]);
    Q_Int("last_connection_timestamp", ACCOUNT_INFO[playerid][ac_LAST_CONNECTION_TIMESTAMP]);
    Q_Int("state", ACCOUNT_INFO[playerid][ac_STATE]);
    Q_Int("time_playing", ACCOUNT_INFO[playerid][ac_TIME_PLAYING]);
    Q_Int("level", ACCOUNT_INFO[playerid][ac_LEVEL]);
    Q_Int("rep", ACCOUNT_INFO[playerid][ac_REP]);
    Q_Int("connected", PI[playerid][ac_CONNECTED]);
    Q_Int("playerid", PI[playerid][ac_PLAYERID]);
    Q_Int("doubt_channel", ACCOUNT_INFO[playerid][ac_DOUBT_CHANNEL]);
    Q_Int("time_for_rep", ACCOUNT_INFO[playerid][ac_TIME_FOR_REP]);
    Q_Int("admin_level", ACCOUNT_INFO[playerid][ac_ADMIN_LEVEL]);
    Q_Int("payday_rep", ACCOUNT_INFO[playerid][ac_PAYDAY_REP]);
    Q_Int("vip", ACCOUNT_INFO[playerid][ac_VIP]);
    Q_Raw("vip_expire_date", vip_expire_date);

    // Personaje
    Q_Int("skin", PI[playerid][ac_SKIN]);
    Q_Int("cash", PI[playerid][ac_CASH]);
    Q_Float("pos_x", PI[playerid][ac_POS_X]);
    Q_Float("pos_y", PI[playerid][ac_POS_Y]);
    Q_Float("pos_z", PI[playerid][ac_POS_Z]);
    Q_Float("angle", PI[playerid][ac_ANGLE]);
    Q_Int("interior", PI[playerid][ac_INTERIOR]);
    Q_Int("local_interior", PI[playerid][ac_LOCAL_INTERIOR]);
    Q_Int("fight_style", PI[playerid][ac_FIGHT_STYLE]);
    Q_Float("health", PI[playerid][ac_HEALTH]);
    Q_Float("armour", PI[playerid][ac_ARMOUR]);
    Q_Int("gender", PI[playerid][ac_GENDER]);
    Q_Float("hungry", PI[playerid][ac_HUNGRY]);
    Q_Float("thirst", PI[playerid][ac_THIRST]);

    // Policía / legal
    Q_Int("black_market_level", PI[playerid][ac_BLACK_MARKET_LEVEL]);
    Q_Int("wanted_level", PI[playerid][ac_WANTED_LEVEL]);
    Q_Int("police_jail_time", PI[playerid][ac_POLICE_JAIL_TIME]);
    Q_Int("police_duty", PI[playerid][ac_POLICE_DUTY]);
    Q_Int("police_jail_id", PI[playerid][ac_POLICE_JAIL_ID]);
    Q_Int("drive_license_points", PI[playerid][ac_DRIVE_LICENSE_POINTS]);
    Q_Int("placa_pd", PI[playerid][ac_PLACA_PD]);
    Q_Int("can_buy_bm", PI[playerid][ac_CAN_BUY_BM]);
    Q_Int("mute", PI[playerid][ac_MUTE]);

    // Banco
    Q_Int("bank_account", PI[playerid][ac_BANK_ACCOUNT]);
    Q_Int("bank_money", PI[playerid][ac_BANK_MONEY]);

    // Teléfono y objetos
    Q_Int("phone_number", PI[playerid][ac_PHONE_NUMBER]);
    Q_Int("phone_state", PI[playerid][ac_PHONE_STATE]);
    Q_Int("phone_visible_number", PI[playerid][ac_PHONE_VISIBLE_NUMBER]);
    Q_Int("phone_resolver", PI[playerid][ac_PHONE_RESOLVER]);
    Q_Int("gps", PI[playerid][ac_GPS]);
    Q_Int("maso", PI[playerid][ac_Maso]);
    Q_Int("mp3", PI[playerid][ac_MP3]);
    Q_Int("speakers", PI[playerid][ac_SPEAKERS]);

    // Inventario
    Q_Int("mechanic_pieces", PI[playerid][ac_MECHANIC_PIECES]);
    Q_Int("fuel_drum", PI[playerid][ac_FUEL_DRUM]);
    Q_Int("seed_medicine", PI[playerid][ac_SEED_MEDICINE]);
    Q_Int("seed_cannabis", PI[playerid][ac_SEED_CANNABIS]);
    Q_Int("seed_crack", PI[playerid][ac_SEED_CRACK]);
    Q_Int("medicine", PI[playerid][ac_MEDICINE]);
    Q_Int("cannabis", PI[playerid][ac_CANNABIS]);
    Q_Int("crack", PI[playerid][ac_CRACK]);
    Q_Int("mechanic_kits", PI[playerid][ac_MECHANIC_KITS]);
    Q_Int("medical_kits", PI[playerid][ac_MEDICAL_KITS]);

    // Configuración del cliente
    Q_Int("config_sounds", PI[playerid][ac_CONFIG_SOUNDS]);
    Q_Int("config_audio", PI[playerid][ac_CONFIG_AUDIO]);
    Q_Int("config_time", PI[playerid][ac_CONFIG_TIME]);
    Q_Int("config_hud", PI[playerid][ac_CONFIG_HUD]);
    Q_Int("config_admin", PI[playerid][ac_CONFIG_ADMIN]);
    Q_Int("config_secure_login", PI[playerid][ac_CONFIG_SECURE_LOGIN]);

    // Banda
    Q_Raw("crew", crew);
    Q_Int("crew_rank", PI[playerid][ac_CREW_RANK]);
    return Q_Execute(ACCOUNT_INFO[playerid][ac_ID]);
}

// Escribe solo las credenciales (contraseña hasheada + salt).
Account_SaveCredentials(playerid)
{
    if (!ACCOUNT_INFO[playerid][ac_ID]) return 0;

    mysql_format(srp_db, QUERY_BUFFER, sizeof QUERY_BUFFER, "UPDATE player SET salt = '%e', pass = '%e' WHERE id = %d;", ACCOUNT_INFO[playerid][ac_SALT], ACCOUNT_INFO[playerid][ac_PASS], ACCOUNT_INFO[playerid][ac_ID]);
    mysql_tquery(srp_db, QUERY_BUFFER);
    return 1;
}

// Guarda todo lo del jugador (cuenta + vehículos + accesorios + armas + trabajos).
Account_SaveAll(playerid, bool:disconnect = false)
{
    if (!ACCOUNT_INFO[playerid][ac_ID] || !PLAYER_TEMP[playerid][pt_USER_LOGGED]) return 0;

    SavePlayerData(playerid);
    SavePlayerVehicles(playerid, disconnect);
    SavePlayerToys(playerid);
    SavePlayerWeaponsData(playerid);
    SavePlayerWorkLevels(playerid);
    PLAYER_TEMP[playerid][pt_SAVE_ACCOUNT_TIME] = gettime();
    return 1;
}

/*
    Actualiza posición/interior en PI sin tocar el estado de rol.
    (UpdatePlayerWorldInfo se usa solo al desconectar porque además saca al
    jugador de interiores/propiedades.)
*/
Account_SyncLivePosition(playerid)
{
    if (PLAYER_TEMP[playerid][pt_GAME_STATE] != GAME_STATE_NORMAL) return 0;
    if (ACCOUNT_INFO[playerid][ac_STATE] != ROLEPLAY_STATE_NORMAL) return 0;

    GetPlayerPos(playerid, PI[playerid][ac_POS_X], PI[playerid][ac_POS_Y], PI[playerid][ac_POS_Z]);
    GetPlayerFacingAngle(playerid, PI[playerid][ac_ANGLE]);
    PI[playerid][ac_INTERIOR] = GetPlayerInterior(playerid);
    return 1;
}

/*
    Flujo de autenticación:

    OnPlayerRequestClass (chequeo de ban)
        -> Account_Check()               ¿existe la cuenta?
            sí -> Account_ShowLoginScene()  -> DIALOG_LOGIN
                    -> Account_OnLoginDialog()   valida contraseña
                    -> Account_LoadAndLogin()    SELECT * + OnPlayerLogin
                    -> LoginPlayer()             spawn
            no -> Account_ShowRegisterScene() -> DIALOG_REGISTER
                    -> Account_OnRegisterDialog()      contraseña
                    -> Account_OnRegisterEmailDialog() correo + guía/skin
                    -> Account_Create()                INSERT + OnPlayerRegister
*/
#if defined account_auth_included
    #endinput
#endif
#define account_auth_included

/* ---------------------------------------------------------------- Conexión */

Account_Check(playerid)
{
    inline OnPlayerCheck()
    {
        new rows;
        if (!cache_get_row_count(rows)) return Kick(playerid);

        TextDrawShowForPlayer(playerid, Textdraws[textdraw_LOAD_SCREEN][2]);
        TextDrawShowForPlayer(playerid, Textdraws[textdraw_LOAD_SCREEN][3]);

        if (rows)
        {
            cache_get_value_name_int(0, "id", ACCOUNT_INFO[playerid][ac_ID]);
            cache_get_value_name(0, "ip", ACCOUNT_INFO[playerid][ac_IP], 16);
            cache_get_value_name(0, "email", ACCOUNT_INFO[playerid][ac_EMAIL], 32);
            cache_get_value_name(0, "salt", ACCOUNT_INFO[playerid][ac_SALT], 16);
            cache_get_value_name(0, "pass", ACCOUNT_INFO[playerid][ac_PASS], 65);
            cache_get_value_name_int(0, "config_secure_login", PI[playerid][ac_CONFIG_SECURE_LOGIN]);

            mysql_format(srp_db, QUERY_BUFFER, sizeof QUERY_BUFFER, "UPDATE player SET connected = 1, playerid = %d WHERE id = %d;", playerid, ACCOUNT_INFO[playerid][ac_ID]);
            mysql_tquery(srp_db, QUERY_BUFFER);
            PLAYER_TEMP[playerid][pt_USER_EXIST] = true;
        }
        else PLAYER_TEMP[playerid][pt_USER_EXIST] = false;

        GameTextForPlayer(playerid, "_", 500, 3);
        if (PLAYER_TEMP[playerid][pt_USER_EXIST]) Account_ShowLoginScene(playerid);
        else Account_ShowRegisterScene(playerid);
        return 1;
    }
    mysql_format(srp_db, QUERY_BUFFER, sizeof QUERY_BUFFER, "SELECT id, ip, email, salt, pass, config_secure_login FROM player WHERE name = '%e';", PLAYER_TEMP[playerid][pt_NAME]);
    MySQL_TQueryInline(srp_db, using inline OnPlayerCheck, QUERY_BUFFER);
    return 1;
}

Account_ShowLoginScene(playerid)
{
    new intro = random(sizeof(Intro_Info));
    SetPlayerVirtualWorld(playerid, WORLD_INTRO);

    TextDrawShowForPlayer(playerid, Textdraws[textdraw_LOAD_SCREEN][1]);
    InterpolateCameraPos(playerid,
        Intro_Info[intro][intro_info_CAM_X],
        Intro_Info[intro][intro_info_CAM_Y],
        Intro_Info[intro][intro_info_CAM_Z],
        Intro_Info[intro][intro_info_CAM_RX],
        Intro_Info[intro][intro_info_CAM_RY],
        Intro_Info[intro][intro_info_CAM_RZ],
        10000,
        CAMERA_MOVE);
    InterpolateCameraLookAt(playerid,
        Intro_Info[intro][intro_info_CAM_LOOK_AT_X],
        Intro_Info[intro][intro_info_CAM_LOOK_AT_Y],
        Intro_Info[intro][intro_info_CAM_LOOK_AT_Z],
        Intro_Info[intro][intro_info_CAM_LOOK_AT_RX],
        Intro_Info[intro][intro_info_CAM_LOOK_AT_RY],
        Intro_Info[intro][intro_info_CAM_LOOK_AT_RZ],
        10000,
        CAMERA_MOVE);
    SetPlayerMyWeather(playerid, Intro_Info[intro][intro_info_WEATHER]);
    SetPlayerMyTime(playerid, Intro_Info[intro][intro_info_HOUR], 0);

    if (!PI[playerid][ac_CONFIG_SECURE_LOGIN] && strcmp(PLAYER_TEMP[playerid][pt_IP], ACCOUNT_INFO[playerid][ac_IP])) // La IP actual no es la de la última conexión
    {
        SendNotification(playerid, "Tu dirección IP ha cambiado desde tu última conexión.");
    }
    ShowDialog(playerid, DIALOG_LOGIN);
    return 1;
}

Account_ShowRegisterScene(playerid)
{
    TextDrawShowForPlayer(playerid, Textdraws[textdraw_LOAD_SCREEN][0]);
    // Nuevo usuario
    PLAYER_TEMP[playerid][pt_REGISTER_ACTOR] = CreateActor(Intro_Skins[0][0], 444.200225, 514.112182, 1001.419494, 0.939619); //ACTOR DE LA HABITACION DE LA INTRO
    SetActorVirtualWorld(PLAYER_TEMP[playerid][pt_REGISTER_ACTOR], playerid + 10);
    ApplyActorAnimation(PLAYER_TEMP[playerid][pt_REGISTER_ACTOR], "INT_SHOP", "shop_loop", 4.1, 1, 1, 1, 0, 0);

    SetPlayerVirtualWorld(playerid, playerid + 10);
    InterpolateCameraPos(playerid,
        447.1717,
        512.9380,
        1001.6884,
        447.1717,
        512.9380,
        1001.6884,
        4000);
    InterpolateCameraLookAt(playerid,
        446.4043,
        513.5767,
        1001.7037,
        446.4043,
        513.5767,
        1001.7037,
        4000);
    SetPlayerInterior(playerid, 12);
    ShowDialog(playerid, DIALOG_REGISTER);
    return 1;
}

/* ---------------------------------------------------------------- Registro */

// Comprueba largo y que no sea igual al nombre. Devuelve 1 si es válida.
Account_IsValidPassword(playerid, const password[])
{
    new len = strlen(password);
    if (len < MIN_PASS_LENGTH || len > MAX_PASS_LENGTH)
    {
        SendFormatNotification(playerid, "La contraseña debe tener entre %d y %d caracteres.", MIN_PASS_LENGTH, MAX_PASS_LENGTH);
        return 0;
    }
    if (!strcmp(password, PLAYER_TEMP[playerid][pt_NAME], true))
    {
        SendNotification(playerid, "La contraseña no puede ser igual a tu nombre.");
        return 0;
    }
    return 1;
}

// Genera salt nuevo y guarda el hash en ACCOUNT_INFO (no escribe en la BD).
Account_SetPassword(playerid, password[])
{
    new salt[16];
    getRandomSalt(salt);
    format(ACCOUNT_INFO[playerid][ac_SALT], 16, "%s", salt);
    SHA256_PassHash(password, ACCOUNT_INFO[playerid][ac_SALT], ACCOUNT_INFO[playerid][ac_PASS], 64 + 1);
    return 1;
}

// 1 si la contraseña coincide con la guardada.
Account_CheckPassword(playerid, password[])
{
    // strcmp devuelve 0 si alguna cadena está vacía: sin esta guarda una cuenta
    // con pass vacío en la BD aceptaría cualquier contraseña.
    if (isnull(password) || isnull(ACCOUNT_INFO[playerid][ac_PASS])) return 0;

    new hash[64 + 1];
    SHA256_PassHash(password, ACCOUNT_INFO[playerid][ac_SALT], hash, sizeof hash);
    return !strcmp(hash, ACCOUNT_INFO[playerid][ac_PASS], false);
}

Account_OnRegisterDialog(playerid, response, inputtext[])
{
    if (!response) return Kick(playerid);
    if (!Account_IsValidPassword(playerid, inputtext)) return ShowDialog(playerid, DIALOG_REGISTER);

    Account_SetPassword(playerid, inputtext);
    ShowDialog(playerid, DIALOG_REGISTER_EMAIL);
    return 1;
}

Account_OnRegisterEmailDialog(playerid, response, inputtext[])
{
    if (!response) return Kick(playerid);

    new mail[32];
    format(mail, sizeof mail, "%s", inputtext);
    if (!IsValidEmail(mail))
    {
        SendNotification(playerid, "Correo no válido.");
        ShowDialog(playerid, DIALOG_REGISTER_EMAIL);
        return 1;
    }

    inline OnCheckEmail()
    {
        new rows;
        if (!cache_get_row_count(rows)) return Kick(playerid);

        if (rows)
        {
            SendNotification(playerid, "Este correo electrónico ya está en uso, use otro distinto.");
            ShowDialog(playerid, DIALOG_REGISTER_EMAIL);
            return 1;
        }

        format(ACCOUNT_INFO[playerid][ac_EMAIL], 32, "%s", mail);

        // Guía de reglas -> selección de skin (OnPlayerClickPlayerTextDraw) -> Account_Create
        ApplyActorAnimation(PLAYER_TEMP[playerid][pt_REGISTER_ACTOR], "INT_SHOP", "shop_loop", 4.1, 1, 1, 1, 0, 0);
        PLAYER_TEMP[playerid][pt_READING_GUIDE] = true;
        PLAYER_TEMP[playerid][pt_GUIDE_STEP] = 0;
        SendNotification(playerid, "Lee detenidamente la guía, para pasar de concepto haz clic en siguiente.");
        SetPlayerRulesTextDraw(playerid,
            Server_Rules[ PLAYER_TEMP[playerid][pt_GUIDE_STEP] ][Rule_Title],
            Server_Rules[ PLAYER_TEMP[playerid][pt_GUIDE_STEP] ][Rule_Info],
            Server_Rules[ PLAYER_TEMP[playerid][pt_GUIDE_STEP] ][Rule_Pages],
            PLAYER_TEMP[playerid][pt_GUIDE_STEP] + 1,
            sizeof(Server_Rules) );
        SelectTextDrawEx(playerid, 0xe8d08fFF);
        return 1;
    }
    mysql_format(srp_db, QUERY_BUFFER, sizeof QUERY_BUFFER, "SELECT id FROM player WHERE email = '%e';", mail);
    MySQL_TQueryInline(srp_db, using inline OnCheckEmail, QUERY_BUFFER);
    return 1;
}

Account_Create(playerid)
{
    if (PLAYER_TEMP[playerid][pt_USER_EXIST]) return 0;

    inline OnPlayerInserted()
    {
        ACCOUNT_INFO[playerid][ac_ID] = cache_insert_id();
        if (ACCOUNT_INFO[playerid][ac_ID])
        {
            PLAYER_TEMP[playerid][pt_USER_EXIST] = true;
            PI[playerid][ac_PHONE_NUMBER] = getPhoneNumber(ACCOUNT_INFO[playerid][ac_ID]);
            mysql_format(srp_db, QUERY_BUFFER, sizeof QUERY_BUFFER, "UPDATE player SET phone_number = %d WHERE id = %d;", PI[playerid][ac_PHONE_NUMBER], ACCOUNT_INFO[playerid][ac_ID]);
            mysql_tquery(srp_db, QUERY_BUFFER);
            CreateDefaultContacts(playerid);
            CallLocalFunction("OnPlayerRegister", "i", playerid);
        }
        else Kick(playerid);
        return 1;
    }

    mysql_format
    (
        srp_db, QUERY_BUFFER, sizeof QUERY_BUFFER,
        "\
        INSERT INTO player\
        (\
        name,\
        ip,\
        email,\
        salt,\
        pass,\
        reg_date,\
        last_connection_timestamp,\
        state,\
        level,\
        rep,\
        connected,\
        playerid,\
        time_for_rep,\
        skin,\
        cash,\
        pos_x,\
        pos_y,\
        pos_z,\
        angle,\
        fight_style,\
        health,\
        hungry,\
        thirst,\
        config_sounds,\
        config_audio,\
        config_time,\
        config_hud,\
        config_admin,\
        config_secure_login,\
        phone_visible_number,\
        doubt_channel\
        ) \
        VALUES\
        (\
        '%e', '%e', '%e', '%e', '%e', '%e', %d, %d, %d, %d, %d, %d, %d, %d, %d, %f, %f, %f, %f, %d, %f, %f, %f, %d, %d, %d, %d, %d, %d, %d, %d\
        );\
        ",
        ACCOUNT_INFO[playerid][ac_NAME],
        ACCOUNT_INFO[playerid][ac_IP],
        ACCOUNT_INFO[playerid][ac_EMAIL],
        ACCOUNT_INFO[playerid][ac_SALT],
        ACCOUNT_INFO[playerid][ac_PASS],
        ACCOUNT_INFO[playerid][ac_REG_DATE],
        ACCOUNT_INFO[playerid][ac_LAST_CONNECTION_TIMESTAMP],
        ACCOUNT_INFO[playerid][ac_STATE],
        ACCOUNT_INFO[playerid][ac_LEVEL],
        ACCOUNT_INFO[playerid][ac_REP],
        PI[playerid][ac_CONNECTED],
        PI[playerid][ac_PLAYERID],
        ACCOUNT_INFO[playerid][ac_TIME_FOR_REP],
        PI[playerid][ac_SKIN],
        PI[playerid][ac_CASH],
        PI[playerid][ac_POS_X],
        PI[playerid][ac_POS_Y],
        PI[playerid][ac_POS_Z],
        PI[playerid][ac_ANGLE],
        PI[playerid][ac_FIGHT_STYLE],
        PI[playerid][ac_HEALTH],
        PI[playerid][ac_HUNGRY],
        PI[playerid][ac_THIRST],
        PI[playerid][ac_CONFIG_SOUNDS],
        PI[playerid][ac_CONFIG_AUDIO],
        PI[playerid][ac_CONFIG_TIME],
        PI[playerid][ac_CONFIG_HUD],
        PI[playerid][ac_CONFIG_ADMIN],
        PI[playerid][ac_CONFIG_SECURE_LOGIN],
        PI[playerid][ac_PHONE_VISIBLE_NUMBER],
        ACCOUNT_INFO[playerid][ac_DOUBT_CHANNEL]
    );
    // (antes se imprimía la query completa en consola, con el hash de la contraseña)
    MySQL_TQueryInline(srp_db, using inline OnPlayerInserted, QUERY_BUFFER );
    return 1;
}

/* ---------------------------------------------------------------- Login */

Account_OnLoginDialog(playerid, response, inputtext[])
{
    if (!response) return Kick(playerid);
    if (isnull(inputtext)) return ShowDialog(playerid, DIALOG_LOGIN);

    if (!Account_CheckPassword(playerid, inputtext))
    {
        PLAYER_TEMP[playerid][pt_BAD_LOGIN_ATTEMP] ++;
        if (PLAYER_TEMP[playerid][pt_BAD_LOGIN_ATTEMP] > MAX_BAD_LOGIN_ATTEMPS) return Kick(playerid);
        SendFormatNotification(playerid, "Contraseña incorrecta, intento %d/%d.", PLAYER_TEMP[playerid][pt_BAD_LOGIN_ATTEMP], MAX_BAD_LOGIN_ATTEMPS);
        ShowDialog(playerid, DIALOG_LOGIN);
        return 1;
    }

    PLAYER_TEMP[playerid][pt_BAD_LOGIN_ATTEMP] = 0;
    Account_LoadAndLogin(playerid);
    return 1;
}

Account_LoadAndLogin(playerid)
{
    inline OnPlayerDataLoad()
    {
        new rows;
        if (!cache_get_row_count(rows) || !rows) return Kick(playerid);

        Account_LoadFromCache(playerid);
        CallLocalFunction("OnPlayerLogin", "i", playerid);
        return 1;
    }
    GameTextForPlayer(playerid, "~y~cargando...", 10000, 3);
    mysql_format(srp_db, QUERY_BUFFER, sizeof QUERY_BUFFER, "SELECT * FROM player WHERE id = %d;", ACCOUNT_INFO[playerid][ac_ID]);
    MySQL_TQueryInline(srp_db, using inline OnPlayerDataLoad, QUERY_BUFFER);
    return 1;
}

LoginPlayer(playerid)
{
    PLAYER_TEMP[playerid][pt_USER_LOGGED] = true;

    LoadPlayerVehicles(playerid);
    LoadPlayerPhoneBook(playerid);
    LoadPlayerToys(playerid);
    LoadPlayerGPSData(playerid);
    LoadPlayerPocketData(playerid);
    LoadPlayerWeaponsData(playerid);
    LoadPlayerWorks(playerid);
    LoadPlayerCrewInfo(playerid);

    SetPlayerScore(playerid, ACCOUNT_INFO[playerid][ac_LEVEL]);
    PLAYER_TEMP[playerid][pt_DOUBT_CHANNEL_TIME] = gettime();
    ResetPlayerWeapons(playerid);
    ResetPlayerMoney(playerid);
    GivePlayerMoney(playerid, PI[playerid][ac_CASH]);
    SetPlayerFightingStyle(playerid, PI[playerid][ac_FIGHT_STYLE]);
    SetPlayerHealthEx(playerid, PI[playerid][ac_HEALTH]);
    SetPlayerArmourEx(playerid, PI[playerid][ac_ARMOUR]);
    SetPlayerVirtualWorld(playerid, 0);
    SetPlayerNormalColor(playerid);
    StopAudioStreamForPlayer(playerid);
    SetSpawnInfo(playerid,
        NO_TEAM,
        PI[playerid][ac_SKIN],
        PI[playerid][ac_POS_X],
        PI[playerid][ac_POS_Y],
        PI[playerid][ac_POS_Z],
        PI[playerid][ac_ANGLE],
        0,
        0,
        0,
        0,
        0,
    0);
    SetPlayerInterior(playerid, PI[playerid][ac_INTERIOR]);

    PLAYER_TEMP[playerid][pt_BAD_LOGIN_ATTEMP] = 0;
    SendClientMessageEx(playerid, 0xB09CA9FF, "> Bienvenido de vuelta, %s. Tu última conexión fue el %s.", PLAYER_TEMP[playerid][pt_RP_NAME], ACCOUNT_INFO[playerid][ac_LAST_CONNECTION]);
    TogglePlayerSpectatingEx(playerid, false);
    TogglePlayerControllableEx(playerid, true);
}

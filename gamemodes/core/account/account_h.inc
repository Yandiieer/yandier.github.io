/*
    Módulo de cuentas (registro, login, carga y guardado).

    Antes esta lógica estaba repartida entre:
        - dialog/callbacks_with_dialog.inc  (búsqueda de la cuenta al conectar)
        - dialog/response.inc               (DIALOG_REGISTER / _EMAIL / DIALOG_LOGIN)
        - main/functions.inc                (SetPiDefaultValues, LoginPlayer,
                                              SavePlayerData, RegisterNewPlayer)
    Ahora todo vive en core/account/ y esos archivos solo llaman a Account_*.

    Archivos:
        account_h.inc       configuración y declaraciones (este archivo)
        account_query.inc   constructor de UPDATE por columnas (Q_*)
        account_data.inc    valores por defecto, carga desde la BD y guardado
        account_auth.inc    flujo de conexión, registro y login
        account.inc         punto de entrada (incluye los anteriores + autoguardado)
*/
#if defined account_h_included
    #endinput
#endif
#define account_h_included

// Los valores (ACCOUNT_AUTOSAVE_INTERVAL, ACCOUNT_CSAVE_COOLDOWN...) están en
// gamemodes/CONFIGURACION.inc, sección 4.

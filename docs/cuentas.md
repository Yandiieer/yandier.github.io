# Sistema de cuentas (`gamemodes/core/account/`)

Registro, login, carga y guardado de cuentas están en un solo módulo.

| Archivo | Contenido |
| :-- | :-- |
| `account_h.inc` | Configuración: intervalo del autoguardado y espera de `/csave` |
| `account_query.inc` | Constructor de `UPDATE` por columnas (`Q_Begin`, `Q_Int`, `Q_Float`, `Q_Str`, `Q_Raw`, `Q_Execute`) |
| `account_data.inc` | `SetPiDefaultValues`, `Account_LoadFromCache`, `SavePlayerData`, `Account_SaveCredentials`, `Account_SaveAll` |
| `account_auth.inc` | Conexión, registro (`Account_Create`) y login (`LoginPlayer`) |
| `account.inc` | Punto de entrada y autoguardado periódico |

## Flujo

```
OnPlayerRequestClass -> chequeo de ban -> Account_Check
  existe    -> Account_ShowLoginScene -> DIALOG_LOGIN -> Account_OnLoginDialog
               -> Account_LoadAndLogin -> OnPlayerLogin -> LoginPlayer
  no existe -> Account_ShowRegisterScene -> DIALOG_REGISTER -> Account_OnRegisterDialog
               -> DIALOG_REGISTER_EMAIL -> Account_OnRegisterEmailDialog
               -> guía + elegir skin -> Account_Create -> OnPlayerRegister
```

## Guardado

- `Account_SaveAll(playerid, disconnect)` guarda la cuenta, los vehículos, los accesorios, las armas y los trabajos. Lo usan la desconexión, `OnGameModeExit`, `/csave` y el autoguardado.
- El autoguardado corre cada 10 minutos por jugador (`ACCOUNT_AUTOSAVE_INTERVAL`) y se salta si la cuenta se guardó hace menos de 60 s.
- Email, salt y pass ya no se escriben en cada guardado, solo cuando cambian (registro, cambio de contraseña o de correo).

## Agregar un dato persistente

1. Agrega el campo en `enum_PI` o `account_Enum` (`core/player/player_account_h.inc`).
2. Agrega la columna en la tabla `player` (`db/srp_db.sql` + `ALTER TABLE`).
3. Agrega una línea en `Account_LoadFromCache`, por ejemplo `cache_get_value_name_int(0, "col", PI[playerid][ac_X]);`.
4. Agrega una línea en `SavePlayerData`, por ejemplo `Q_Int("col", PI[playerid][ac_X]);`.

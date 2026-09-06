#ifndef CHARACTER_REGISTRY_H
#define CHARACTER_REGISTRY_H

#include <common.h>

enum CharacterFlags
{
    CHARACTER_FLAG_HAS_WHEELS = 1 << 0,
};

struct CharacterDef
{
    s16 id;
    s16 driverPackCharacterID;

    char assetName[MODEL_NAME_BYTE_COUNT];
    const char *displayName;
    const char *shortName;

    u8 engineClass;
    u8 minimapColor;
    u8 flags;
};

int CharacterRegistry_GetCount(void);

int CharacterRegistry_GetDriverPackID(
    int characterID);

int CharacterRegistry_GetEngineClass(
    int characterID);

b32 CharacterRegistry_HasWheels(
    int characterID);

const struct CharacterDef *
CharacterRegistry_GetByID(int characterID);

const struct CharacterDef *
CharacterRegistry_GetByRosterIndex(int rosterIndex);

const char *CharacterRegistry_GetAssetName(
    int characterID);

#endif
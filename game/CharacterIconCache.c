#include <common.h>
#include <CharacterRegistry.h>
#include <CharacterIconCache.h>

struct CharacterIconCacheSlot
{
    s16 characterID;
    struct Icon *icon;
};

static struct CharacterIconCacheSlot
    sCharacterIconCache[CHARACTER_ICON_CACHE_SIZE];

static b32 CharacterIconCache_LoadIntoSlot(
    int slot,
    const struct CharacterDef *character);

void CharacterIconCache_Init(struct GameTracker *gGT)
{
    for (int slot = 0;
         slot < CHARACTER_ICON_CACHE_SIZE;
         slot++)
    {
        int iconID =
            data.MetaDataCharacters[slot].iconID;

        sCharacterIconCache[slot].characterID = -1;
        sCharacterIconCache[slot].icon =
            gGT->ptrIcons[iconID];
    }
}

void CharacterIconCache_LoadRosterPage(
    struct GameTracker *gGT,
    int page)
{
    CharacterIconCache_Init(gGT);

    for (int slot = 0;
         slot < CHARACTER_ICON_CACHE_SIZE;
         slot++)
    {
        int rosterIndex =
            page * CHARACTER_ICON_CACHE_SIZE + slot;

        const struct CharacterDef *character =
            CharacterRegistry_GetByRosterIndex(
                rosterIndex);

        if (character == NULL)
        {
            sCharacterIconCache[slot].characterID = -1;
            continue;
        }

        if (!CharacterIconCache_LoadIntoSlot(slot,character))
        {
            continue;
        }
    }
}

static b32 CharacterIconCache_LoadIntoSlot(
    int slot,
    const struct CharacterDef *character)
{
    if ((u32)slot >= CHARACTER_ICON_CACHE_SIZE ||
        character == NULL)
    {
        return 0;
    }

    struct Icon *icon =
        sCharacterIconCache[slot].icon;

    if (!LOAD_ApplyLooseRacerIcon(
            icon,
            character->assetName))
    {
        sCharacterIconCache[slot].characterID = -1;
        return 0;
    }

    sCharacterIconCache[slot].characterID =
        character->id;

    return 1;
}

struct Icon *CharacterIconCache_Get(
    int characterID)
{
    for (int slot = 0;
         slot < CHARACTER_ICON_CACHE_SIZE;
         slot++)
    {
        if (sCharacterIconCache[slot].characterID ==
            characterID)
        {
            return sCharacterIconCache[slot].icon;
        }
    }

    return NULL;
}

void CharacterIconCache_LoadRaceCharacters(
    struct GameTracker *gGT)
{
    CharacterIconCache_Init(gGT);

    for (int slot = 0;
         slot < LOAD_CHARACTER_ID_COUNT;
         slot++)
    {
        int characterID =
            data.characterIDs[slot];

        const struct CharacterDef *character =
            CharacterRegistry_GetByID(
                characterID);

        if (character == NULL)
            continue;

        if (!CharacterIconCache_LoadIntoSlot(
                slot,
                character))
        {
            continue;
        }
    }
}
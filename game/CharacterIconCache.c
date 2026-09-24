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

static struct CharacterIconCacheSlot sHighScoreIcons[16];

static b32 CharacterIconCache_LoadIntoSlot(
    int slot,
    const struct CharacterDef *character);

void CharacterIconCache_Init(struct GameTracker *gGT)
{
    for (int slot = 0; slot < 16; slot++)
        sHighScoreIcons[slot].characterID = -1;
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

int CharacterIconCache_GetNameColor(int characterID)
{
    const struct CharacterDef *character = CharacterRegistry_GetByID(characterID);
    return character != NULL ? character->minimapColor : WHITE;
}

struct Icon *CharacterIconCache_GetAdventureProfile(
    struct GameTracker *gGT, int characterID, int profileSlot)
{
    if ((u32)profileSlot >= MEMCARD_ADV_PROFILE_COUNT || characterID < 0)
        return NULL;

    // Keep the first eight texture slots intact for the race/hub HUD.
    struct CharacterIconCacheSlot *cached = &sHighScoreIcons[8 + profileSlot];
    if (cached->characterID != characterID)
    {
        struct Icon *icon = gGT->ptrIcons[data.MetaDataCharacters[8 + profileSlot].iconID];
        const struct CharacterDef *character = CharacterRegistry_GetByID(characterID);
        cached->characterID = characterID;
        cached->icon = NULL;
        if (character != NULL && LOAD_ApplyLooseRacerIcon(icon, character->assetName))
            cached->icon = icon;
    }
    return cached->icon;
}

struct Icon *CharacterIconCache_GetHighScore(int characterID)
{
    for (int slot = 0; slot < 16; slot++)
        if (sHighScoreIcons[slot].characterID == characterID)
            return sHighScoreIcons[slot].icon;
    return NULL;
}

void CharacterIconCache_LoadHighScores(struct GameTracker *gGT,
    const struct HighScoreEntry *first, const struct HighScoreEntry *second)
{
    int ids[12];
    int count = 0;
    // Race HUD icons occupy the first eight slots. Menu transitions need up to twelve.
    int firstSlot = gGT->levelID == MAIN_MENU_LEVEL ? 0 : 8;
    for (int page = 0; page < 2; page++)
    {
        const struct HighScoreEntry *entries = page == 0 ? first : second;
        if (entries == NULL) continue;
        for (int row = 0; row < MEMCARD_HIGH_SCORE_ENTRIES_PER_MODE; row++)
        {
            int index;
            for (index = 0; index < count; index++)
                if (ids[index] == entries[row].characterID) break;
            if (index == count) ids[count++] = entries[row].characterID;
        }
    }
    for (int slot = firstSlot; slot < 16; slot++)
    {
        int index;
        for (index = 0; index < count; index++)
            if (ids[index] == sHighScoreIcons[slot].characterID) break;
        if (index == count) sHighScoreIcons[slot].characterID = -1;
    }
    for (int index = 0; index < count; index++)
    {
        int cachedSlot;
        for (cachedSlot = firstSlot; cachedSlot < 16; cachedSlot++)
            if (sHighScoreIcons[cachedSlot].characterID == ids[index]) break;
        if (cachedSlot < 16) continue;
        for (int slot = firstSlot; slot < 16; slot++)
        {
            if (sHighScoreIcons[slot].characterID != -1) continue;
            struct Icon *icon = gGT->ptrIcons[data.MetaDataCharacters[slot].iconID];
            const struct CharacterDef *character = CharacterRegistry_GetByID(ids[index]);
            // Remember failed loads too, so a missing PNG is not read every frame.
            sHighScoreIcons[slot].characterID = ids[index];
            sHighScoreIcons[slot].icon = NULL;
            if (character != NULL && LOAD_ApplyLooseRacerIcon(icon, character->assetName))
            {
                sHighScoreIcons[slot].icon = icon;
            }
            break;
        }
    }
}

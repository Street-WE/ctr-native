#include <common.h>
#include <CharacterRegistry.h>

static const struct CharacterDef sCharacterRegistry[] =
{
    [CRASH_BANDICOOT] = {
        .id = CRASH_BANDICOOT,
        .assetName = "crash",
        .displayName = "Rob-N-Hood",
        .shortName = "R. Hood",
        .engineClass = BALANCED,
        .minimapColor = FOREST_GREEN,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [NEO_CORTEX] = {
        .id = NEO_CORTEX,
        .assetName = "cortex",
        .displayName = "Rocket Racer",
        .shortName = "R. Racer",
        .engineClass = SPEED,
        .minimapColor = CORTEX_RED,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [TINY_TIGER] = {
        .id = TINY_TIGER,
        .assetName = "tiny",
        .displayName = "Sam Sinister",
        .shortName = "Sinister",
        .engineClass = SPEED,
        .minimapColor = ORANGE_RED,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [COCO_BANDICOOT] = {
        .id = COCO_BANDICOOT,
        .assetName = "coco",
        .displayName = "Willa the Witch",
        .shortName = "Willa",
        .engineClass = ACCEL,
        .minimapColor = COCO_MAGENTA,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [N_GIN] = {
        .id = N_GIN,
        .assetName = "ngin",
        .displayName = "Ann Droid",
        .shortName = "A. Droid",
        .engineClass = ACCEL,
        .minimapColor = DINGODILE_OLIVE,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [DINGODILE] = {
        .id = DINGODILE,
        .assetName = "dingo",
        .displayName = "Commander Cold",
        .shortName = "C. Cold",
        .engineClass = SPEED,
        .minimapColor = POLAR_CYAN,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [POLAR] = {
        .id = POLAR,
        .assetName = "polar",
        .displayName = "Gov Broadside",
        .shortName = "Governor",
        .engineClass = TURN,
        .minimapColor = PENTA_WHITE,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [PURA] = {
        .id = PURA,
        .assetName = "pura",
        .displayName = "Royal King",
        .shortName = "King",
        .engineClass = TURN,
        .minimapColor = CRASH_BLUE,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [PINSTRIPE] = {
        .id = PINSTRIPE,
        .assetName = "pinstripe",
        .displayName = "Johnny Thunder",
        .shortName = "J. Thunder",
        .engineClass = BALANCED,
        .minimapColor = PAPU_YELLOW,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [PAPU_PAPU] = {
        .id = PAPU_PAPU,
        .assetName = "papu",
        .displayName = "King Kahuka",
        .shortName = "Kahuka",
        .engineClass = TURN,
        .minimapColor = TINY_GREEN,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [RIPPER_ROO] = {
        .id = RIPPER_ROO,
        .assetName = "roo",
        .displayName = "Captain Redbeard",
        .shortName = "Redbeard",
        .engineClass = BALANCED,
        .minimapColor = PLAYER_YELLOW,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [KOMODO_JOE] = {
        .id = KOMODO_JOE,
        .assetName = "joe",
        .displayName = "Basil the Batlord",
        .shortName = "Basil",
        .engineClass = SPEED,
        .minimapColor = DARK_RED,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [N_TROPY] = {
        .id = N_TROPY,
        .assetName = "ntropy",
        .displayName = "Veronica Voltage",
        .shortName = "Veronica",
        .engineClass = SPEED,
        .minimapColor = TROPY_LIGHT_BLUE,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [PENTA_PENGUIN] = {
        .id = PENTA_PENGUIN,
        .assetName = "pen",
        .displayName = "Baron Von Barron",
        .shortName = "Barron",
        .engineClass = TURN,
        .minimapColor = GRAY,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [FAKE_CRASH] = {
        .id = FAKE_CRASH,
        .assetName = "fake",
        .displayName = "Pharaohs Mummy",
        .shortName = "Mummy",
        .engineClass = ACCEL,
        .minimapColor = JOE_COLOR,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [NITROS_OXIDE] = {
        .id = NITROS_OXIDE,
        .assetName = "oxide",
        .displayName = "Gypsy Moth",
        .shortName = "G. Moth",
        .engineClass = SPEED,
        .minimapColor = PINSTRIPE_PALE_DARK_BLUE,
        .flags = 0,
    },
    [X_BKNIGHT] = {
        .id = X_BKNIGHT,
        .assetName = "x_bknight",
        .displayName = "Black Knight",
        .shortName = "B. Knight",
        .engineClass = SPEED,
        .minimapColor = BLACK,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [X_GAIL] = {
        .id = X_GAIL,
        .assetName = "x_gail",
        .displayName = "Gail Storm",
        .shortName = "G. Storm",
        .engineClass = BALANCED,
        .minimapColor = PLAYER_GREEN,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [X_HAWKINS] = {
        .id = X_HAWKINS,
        .assetName = "x_hawkins",
        .displayName = "Blackjack Hawkins",
        .shortName = "Hawkins",
        .engineClass = TURN,
        .minimapColor = ROO_ORANGE,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [X_ROBO] = {
        .id = X_ROBO,
        .assetName = "x_robo",
        .displayName = "Robo Racer",
        .shortName = "Robo",
        .engineClass = ACCEL,
        .minimapColor = LIME_GREEN,
        .flags = 0,
    },
};


const char *CharacterRegistry_GetAssetName(int characterID)
{
    const struct CharacterDef *character =
        CharacterRegistry_GetByID(characterID);

    if (character == NULL)
        return "crash";

    return character->assetName;
}

int CharacterRegistry_GetCount(void)
{
    return len(sCharacterRegistry);
}

const struct CharacterDef *
CharacterRegistry_GetByID(int characterID)
{
    for (u32 i = 0; i < len(sCharacterRegistry); i++)
    {
        if (sCharacterRegistry[i].id == characterID)
            return &sCharacterRegistry[i];
    }

    return NULL;
}

const struct CharacterDef *
CharacterRegistry_GetByRosterIndex(int rosterIndex)
{
    if ((u32)rosterIndex >= len(sCharacterRegistry))
        return NULL;

    return &sCharacterRegistry[rosterIndex];
}
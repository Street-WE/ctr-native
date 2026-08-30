#include <common.h>
#include <CharacterRegistry.h>

static const struct CharacterDef sCharacterRegistry[] =
{
    [CRASH_BANDICOOT] = {
        .id = CRASH_BANDICOOT,
        .assetName = "crash",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Rob-N-Hood",
        .shortName = "R. Hood",
        .engineClass = BALANCED,
        .minimapColor = FOREST_GREEN,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [NEO_CORTEX] = {
        .id = NEO_CORTEX,
        .assetName = "cortex",
        .driverPackCharacterID = NEO_CORTEX,
        .displayName = "Rocket Racer",
        .shortName = "R. Racer",
        .engineClass = SPEED,
        .minimapColor = CORTEX_RED,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [TINY_TIGER] = {
        .id = TINY_TIGER,
        .assetName = "tiny",
        .driverPackCharacterID = TINY_TIGER,
        .displayName = "Sam Sinister",
        .shortName = "Sinister",
        .engineClass = SPEED,
        .minimapColor = ORANGE_RED,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [COCO_BANDICOOT] = {
        .id = COCO_BANDICOOT,
        .assetName = "coco",
        .driverPackCharacterID = COCO_BANDICOOT,
        .displayName = "Willa the Witch",
        .shortName = "Willa",
        .engineClass = ACCEL,
        .minimapColor = COCO_MAGENTA,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [N_GIN] = {
        .id = N_GIN,
        .assetName = "ngin",
        .driverPackCharacterID = N_GIN,
        .displayName = "Ann Droid",
        .shortName = "A. Droid",
        .engineClass = ACCEL,
        .minimapColor = DINGODILE_OLIVE,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [DINGODILE] = {
        .id = DINGODILE,
        .assetName = "dingo",
        .driverPackCharacterID = DINGODILE,
        .displayName = "Commander Cold",
        .shortName = "C. Cold",
        .engineClass = SPEED,
        .minimapColor = POLAR_CYAN,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [POLAR] = {
        .id = POLAR,
        .assetName = "polar",
        .driverPackCharacterID = POLAR,
        .displayName = "Gov Broadside",
        .shortName = "Governor",
        .engineClass = TURN,
        .minimapColor = PENTA_WHITE,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [PURA] = {
        .id = PURA,
        .assetName = "pura",
        .driverPackCharacterID = PURA,
        .displayName = "Royal King",
        .shortName = "King",
        .engineClass = TURN,
        .minimapColor = CRASH_BLUE,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [PINSTRIPE] = {
        .id = PINSTRIPE,
        .assetName = "pinstripe",
        .driverPackCharacterID = PINSTRIPE,
        .displayName = "Johnny Thunder",
        .shortName = "J. Thunder",
        .engineClass = BALANCED,
        .minimapColor = PAPU_YELLOW,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [PAPU_PAPU] = {
        .id = PAPU_PAPU,
        .assetName = "papu",
        .driverPackCharacterID = PAPU_PAPU,
        .displayName = "King Kahuka",
        .shortName = "Kahuka",
        .engineClass = TURN,
        .minimapColor = TINY_GREEN,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [RIPPER_ROO] = {
        .id = RIPPER_ROO,
        .assetName = "roo",
        .driverPackCharacterID = RIPPER_ROO,
        .displayName = "Captain Redbeard",
        .shortName = "Redbeard",
        .engineClass = BALANCED,
        .minimapColor = PLAYER_YELLOW,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [KOMODO_JOE] = {
        .id = KOMODO_JOE,
        .assetName = "joe",
        .driverPackCharacterID = KOMODO_JOE,
        .displayName = "Basil the Batlord",
        .shortName = "Basil",
        .engineClass = SPEED,
        .minimapColor = DARK_RED,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [N_TROPY] = {
        .id = N_TROPY,
        .assetName = "ntropy",
        .driverPackCharacterID = N_TROPY,
        .displayName = "Veronica Voltage",
        .shortName = "Veronica",
        .engineClass = SPEED,
        .minimapColor = TROPY_LIGHT_BLUE,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [PENTA_PENGUIN] = {
        .id = PENTA_PENGUIN,
        .assetName = "pen",
        .driverPackCharacterID = PENTA_PENGUIN,
        .displayName = "Baron Von Barron",
        .shortName = "Barron",
        .engineClass = TURN,
        .minimapColor = GRAY,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [FAKE_CRASH] = {
        .id = FAKE_CRASH,
        .assetName = "fake",
        .driverPackCharacterID = FAKE_CRASH,
        .displayName = "Pharaohs Mummy",
        .shortName = "Mummy",
        .engineClass = ACCEL,
        .minimapColor = JOE_COLOR,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [NITROS_OXIDE] = {
        .id = NITROS_OXIDE,
        .assetName = "oxide",
        .driverPackCharacterID = NITROS_OXIDE,
        .displayName = "Gypsy Moth",
        .shortName = "G. Moth",
        .engineClass = SPEED,
        .minimapColor = PINSTRIPE_PALE_DARK_BLUE,
        .flags = 0,
    },
    [X_BKNIGHT] = {
        .id = X_BKNIGHT,
        .assetName = "x_bknight",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Black Knight",
        .shortName = "B. Knight",
        .engineClass = SPEED,
        .minimapColor = BLACK,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [X_GAIL] = {
        .id = X_GAIL,
        .assetName = "x_gail",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Gail Storm",
        .shortName = "G. Storm",
        .engineClass = BALANCED,
        .minimapColor = PLAYER_GREEN,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [X_HAWKINS] = {
        .id = X_HAWKINS,
        .assetName = "x_hawkins",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Blackjack Hawkins",
        .shortName = "Hawkins",
        .engineClass = TURN,
        .minimapColor = ROO_ORANGE,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [X_ROBO] = {
        .id = X_ROBO,
        .assetName = "x_robo",
        .driverPackCharacterID = NITROS_OXIDE,
        .displayName = "Robo Racer",
        .shortName = "Robo",
        .engineClass = ACCEL,
        .minimapColor = LIME_GREEN,
        .flags = 0,
    },
    [X_ACHU] = {
        .id = X_ACHU,
        .assetName = "x_achu",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Achu",
        .shortName = "Achu",
        .engineClass = BALANCED,
        .minimapColor = PAPU_YELLOW,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [X_TURBOC] = {
        .id = X_TURBOC,
        .assetName = "x_turboc",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Turbo Charger",
        .shortName = "Turbo. C",
        .engineClass = SPEED,
        .minimapColor = PENTA_WHITE,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [X_ADMIRAL] = {
        .id = X_ADMIRAL,
        .assetName = "x_admiral",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Admiral",
        .shortName = "Admiral",
        .engineClass = TURN,
        .minimapColor = PERIWINKLE,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [X_ALPHA] = {
        .id = X_ALPHA,
        .assetName = "x_alpha",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Alpha Dragonis",
        .shortName = "Alpha",
        .engineClass = TURN,
        .minimapColor = LIGHT_GREEN,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [X_ISLANDER] = {
        .id = X_ISLANDER,
        .assetName = "x_islander",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Islander",
        .shortName = "Islander",
        .engineClass = TURN,
        .minimapColor = ROO_ORANGE,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [X_JOAN] = {
        .id = X_JOAN,
        .assetName = "x_joan",
        .driverPackCharacterID = NITROS_OXIDE,
        .displayName = "Joan of Kart",
        .shortName = "Joan",
        .engineClass = ACCEL,
        .minimapColor = BLUE,
        .flags = 0,
    },
    [X_NOVA] = {
        .id = X_NOVA,
        .assetName = "x_nova",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Nova Hunter",
        .shortName = "N. Hunter",
        .engineClass = BALANCED,
        .minimapColor = FAKE_CRASH_GRAY,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
    [X_SCOOTER] = {
        .id = X_SCOOTER,
        .assetName = "x_scooter",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Scooter",
        .shortName = "Scooter",
        .engineClass = BALANCED,
        .minimapColor = PLAYER_RED,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
    },
};

int CharacterRegistry_GetEngineClass(
    int characterID)
{
    const struct CharacterDef *character =
        CharacterRegistry_GetByID(characterID);

    if (character == NULL)
        return BALANCED;

    return character->engineClass;
}

int CharacterRegistry_GetDriverPackID(
    int characterID)
{
    const struct CharacterDef *character =
        CharacterRegistry_GetByID(characterID);

    if (character == NULL)
        return CRASH_BANDICOOT;

    int packID =
        character->driverPackCharacterID;

    if ((packID < CRASH_BANDICOOT) ||
        (packID > NITROS_OXIDE))
    {
        return CRASH_BANDICOOT;
    }

    return packID;
}

b32 CharacterRegistry_HasWheels(
    int characterID)
{
    const struct CharacterDef *character =
        CharacterRegistry_GetByID(characterID);

    if (character == NULL)
        return true;

    return
        (character->flags &
         CHARACTER_FLAG_HAS_WHEELS) != 0;
}

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
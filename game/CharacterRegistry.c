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
        .exhaustType = PAIR_LOW,
    },
    [NEO_CORTEX] = {
        .id = NEO_CORTEX,
        .assetName = "cortex",
        .driverPackCharacterID = NEO_CORTEX,
        .displayName = "Rocket Racer",
        .shortName = "R. Racer",
        .engineClass = SPEED,
        .minimapColor = EXT_COLOR_RED_BLUE_GRADIENT,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
        .exhaustType = HIGH_EXHAUST,
    },
    [TINY_TIGER] = {
        .id = TINY_TIGER,
        .assetName = "tiny",
        .driverPackCharacterID = TINY_TIGER,
        .displayName = "Sam Sinister",
        .shortName = "Sinister",
        .engineClass = SPEED,
        .minimapColor = ORANGE_DARKENED,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
        .exhaustType = PAIR_LOW,
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
        .exhaustType = PAIR_LOW,
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
        .exhaustType = PAIR_LOW,
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
        .exhaustType = PAIR_LOW,
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
        .exhaustType = PAIR_LOW,
    },
    [PURA] = {
        .id = PURA,
        .assetName = "pura",
        .driverPackCharacterID = PURA,
        .displayName = "Royal King",
        .shortName = "King",
        .engineClass = TURN,
        .minimapColor = EXT_COLOR_KING_GOLD,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
        .exhaustType = PAIR_LOW,
    },
    [PINSTRIPE] = {
        .id = PINSTRIPE,
        .assetName = "pinstripe",
        .driverPackCharacterID = PINSTRIPE,
        .displayName = "Johnny Thunder",
        .shortName = "J. Thunder",
        .engineClass = BALANCED,
        .minimapColor = EXT_COLOR_JOHNNY_BROWN,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
        .exhaustType = V_HIGH_EXHAUST,
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
        .exhaustType = PAIR_LOW,
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
        .exhaustType = RED_BEARD,
    },
    [KOMODO_JOE] = {
        .id = KOMODO_JOE,
        .assetName = "joe",
        .driverPackCharacterID = KOMODO_JOE,
        .displayName = "Basil the Batlord",
        .shortName = "Basil",
        .engineClass = SPEED,
        .minimapColor = EXT_COLOR_BASIL_RED,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
        .exhaustType = SINGLE_BASIL,
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
        .exhaustType = PAIR_LOW,
    },
    [PENTA_PENGUIN] = {
        .id = PENTA_PENGUIN,
        .assetName = "pen",
        .driverPackCharacterID = PENTA_PENGUIN,
        .displayName = "Baron Von Barron",
        .shortName = "Barron",
        .engineClass = TURN,
        .minimapColor = EXT_COLOR_BARON_TAN,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
        .exhaustType = PAIR_LOW,
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
        .exhaustType = PAIR_LOW,
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
        .exhaustType = GYPSY_MOTH,
    },
    [X_BKNIGHT] = {
        .id = X_BKNIGHT,
        .assetName = "x_bknight",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Black Knight",
        .shortName = "B. Knight",
        .engineClass = SPEED,
        .minimapColor = EXT_COLOR_KNIGHT_BLACK,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
        .exhaustType = PAIR_LOW,
    },
    [X_GAIL] = {
        .id = X_GAIL,
        .assetName = "x_gail",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Gail Storm",
        .shortName = "G. Storm",
        .engineClass = BALANCED,
        .minimapColor = EXT_COLOR_GAIL_GREEN,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
        .exhaustType = PAIR_LOW,
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
        .exhaustType = PAIR_LOW,
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
        .exhaustType = ROBO_RACER,
    },
    [X_ACHU] = {
        .id = X_ACHU,
        .assetName = "x_achu",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Achu",
        .shortName = "Achu",
        .engineClass = BALANCED,
        .minimapColor = EXT_COLOR_GOLD_GRADIENT,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
        .exhaustType = PAIR_LOW,
    },
    [X_TURBOC] = {
        .id = X_TURBOC,
        .assetName = "x_turboc",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Turbo Charger",
        .shortName = "Turbo. C",
        .engineClass = SPEED,
        .minimapColor = EXT_COLOR_TURBO_GRN_RED,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
        .exhaustType = PAIR_LOW,
    },
    [X_ADMIRAL] = {
        .id = X_ADMIRAL,
        .assetName = "x_admiral",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Admiral",
        .shortName = "Admiral",
        .engineClass = TURN,
        .minimapColor = EXT_COLOR_ADMIRAL_RED,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
        .exhaustType = PAIR_LOW,
    },
    [X_ALPHA] = {
        .id = X_ALPHA,
        .assetName = "x_alpha",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Alpha Dragonis",
        .shortName = "Alpha",
        .engineClass = TURN,
        .minimapColor = EXT_COLOR_ALPHA_GRN,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
        .exhaustType = PAIR_LOW,
    },
    [X_ISLANDER] = {
        .id = X_ISLANDER,
        .assetName = "x_islander",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Islander",
        .shortName = "Islander",
        .engineClass = TURN,
        .minimapColor = EXT_COLOR_ISLANDER_ORANGE,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
        .exhaustType = PAIR_LOW,
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
        .exhaustType = PAIR_LOW,
    },
    [X_NOVA] = {
        .id = X_NOVA,
        .assetName = "x_nova",
        .driverPackCharacterID = CRASH_BANDICOOT,
        .displayName = "Nova Hunter",
        .shortName = "N. Hunter",
        .engineClass = BALANCED,
        .minimapColor = EXT_COLOR_NOVA_GREY,
        .flags = CHARACTER_FLAG_HAS_WHEELS,
        .exhaustType = PAIR_LOW,
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
        .exhaustType = PAIR_LOW,
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

int CharacterRegistry_GetDriverExhaustType(int characterID)
{
    const struct CharacterDef *character =
        CharacterRegistry_GetByID(characterID);

    if (character == NULL)
        return DEFAULT_EXHAUST;

    return character->exhaustType;
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
#include <common.h>
#include <CharacterRegistry.h>
#include <LevelRegistry.h>

#if defined(CTR_NATIVE) && defined(CTR_INTERNAL)
#include <platform/native_checkpoint.h>
#endif

static const u8 sTrackRosters[NITRO_COURT][LOAD_CHARACTER_ID_COUNT] =
{
	[CRASH_COVE] = {
		KOMODO_JOE,
		COCO_BANDICOOT,
		X_HAWKINS,
		TINY_TIGER,
		X_ALPHA,
		X_BKNIGHT,
		X_GAIL,
		N_TROPY,
	},

	[ROO_TUBES] = {
		KOMODO_JOE,
		COCO_BANDICOOT,
		X_HAWKINS,
		TINY_TIGER,
		X_ALPHA,
		X_BKNIGHT,
		X_GAIL,
		N_TROPY,
	},

	[TIGER_TEMPLE] = {
		KOMODO_JOE,
		COCO_BANDICOOT,
		X_HAWKINS,
		TINY_TIGER,
		X_ALPHA,
		X_BKNIGHT,
		X_GAIL,
		N_TROPY,
	},

	[CORTEX_CASTLE] = {
		KOMODO_JOE,
		COCO_BANDICOOT,
		X_HAWKINS,
		TINY_TIGER,
		X_ALPHA,
		X_BKNIGHT,
		X_GAIL,
		N_TROPY,
	},

	[OXIDE_STATION] = {
		PAPU_PAPU,
		X_ACHU,
		DINGODILE,
		X_ISLANDER,
		PURA,
		X_NOVA,
		X_ADMIRAL,
		N_TROPY,
	},

	[COCO_PARK] = {
		PAPU_PAPU,
		X_ACHU,
		DINGODILE,
		X_ISLANDER,
		PURA,
		X_NOVA,
		X_ADMIRAL,
		N_TROPY,
	},

	[DRAGON_MINES] = {
		PAPU_PAPU,
		X_ACHU,
		DINGODILE,
		X_ISLANDER,
		PURA,
		X_NOVA,
		X_ADMIRAL,
		N_TROPY,
	},

	[PAPU_PYRAMID] = {
		PAPU_PAPU,
		X_ACHU,
		DINGODILE,
		X_ISLANDER,
		PURA,
		X_NOVA,
		X_ADMIRAL,
		N_TROPY,
	},

	[MYSTERY_CAVES] = {
		RIPPER_ROO,
		POLAR,
		N_GIN,
		CRASH_BANDICOOT,
		FAKE_CRASH,
		X_NOVA,
		X_BKNIGHT,
		N_TROPY,
	},

	[N_GIN_LABS] = {
		RIPPER_ROO,
		POLAR,
		N_GIN,
		CRASH_BANDICOOT,
		FAKE_CRASH,
		X_NOVA,
		X_BKNIGHT,
		N_TROPY,
	},

	[SEWER_SPEEDWAY] = {
		RIPPER_ROO,
		POLAR,
		N_GIN,
		CRASH_BANDICOOT,
		FAKE_CRASH,
		X_NOVA,
		X_BKNIGHT,
		N_TROPY,
	},

	[DINGO_CANYON] = {
		RIPPER_ROO,
		POLAR,
		N_GIN,
		CRASH_BANDICOOT,
		FAKE_CRASH,
		X_NOVA,
		X_BKNIGHT,
		N_TROPY,
	},

	[HOT_AIR_SKYWAY] = {
		NEO_CORTEX,
		NITROS_OXIDE,
		PENTA_PENGUIN,
		PINSTRIPE,
		KOMODO_JOE,
		PAPU_PAPU,
		RIPPER_ROO,
		N_TROPY,
	},

	/* One eight-character entry for every race track. */
};

void LOAD_RunPtrMap(char *origin, int *patchArr, int numPtrs)
{
	int *ptrCurrOffset = patchArr;

	for (ptrCurrOffset = &patchArr[0]; ptrCurrOffset < &patchArr[numPtrs]; ptrCurrOffset++)
	{
		int offset = (*ptrCurrOffset >> 2) << 2;
		*(int *)&origin[offset] = *(int *)&origin[offset] + (int)origin;
#if defined(CTR_NATIVE) && defined(CTR_INTERNAL)
		NativeCheckpoint_RegisterPointerSlot(&origin[offset]);
#endif
	}
}

#if defined(CTR_NATIVE)
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#define STBI_NO_GIF
#define STBI_NO_HDR
#define STBI_NO_LINEAR
#define STBI_NO_THREAD_LOCALS
#define STB_IMAGE_IMPLEMENTATION
#include "../../externals/SDL/src/video/stb_image.h"

	enum
	{
		LOOSE_RACER_ICON_COUNT = NITROS_OXIDE + 1,

		LOOSE_ICON_WIDTH = 44,
		LOOSE_ICON_HEIGHT = 26,
		LOOSE_ICON_WORDS_PER_ROW = (LOOSE_ICON_WIDTH + 3) / 4, // 11
	};

	static const char *const sLooseRacerIconPaths[LOOSE_RACER_ICON_COUNT] = {
		"mods/racer_icons/crash.png",
		"mods/racer_icons/cortex.png",
		"mods/racer_icons/tiny.png",
		"mods/racer_icons/coco.png",
		"mods/racer_icons/ngin.png",
		"mods/racer_icons/dingo.png",
		"mods/racer_icons/polar.png",
		"mods/racer_icons/pura.png",
		"mods/racer_icons/pinstripe.png",
		"mods/racer_icons/papu.png",
		"mods/racer_icons/roo.png",
		"mods/racer_icons/joe.png",
		"mods/racer_icons/ntropy.png",
		"mods/racer_icons/pen.png",
		"mods/racer_icons/fake.png",
		"mods/racer_icons/oxide.png",
	};

	static int LOAD_ReadLooseRacerIcon(
    const char *path,
    u16 packedPixels[LOOSE_ICON_WORDS_PER_ROW * LOOSE_ICON_HEIGHT],
    u16 clut[16])
	{
		struct NativeAssetsByteBuffer file;
		unsigned char *rgba;
		int width;
		int height;
		int channels;
		int paletteCount = 1; // Index 0 is reserved for transparency.

		memset(packedPixels, 0,
			   LOOSE_ICON_WORDS_PER_ROW * LOOSE_ICON_HEIGHT * sizeof(u16));
		memset(clut, 0, 16 * sizeof(u16));

		if (!NativeAssets_ReadBytes(
				path,
				NATIVE_ASSET_READ_DATA_FILE,
				&file))
		{
			return 0;
		}

		rgba = stbi_load_from_memory(
			file.data,
			file.size,
			&width,
			&height,
			&channels,
			4);

		NativeAssets_FreeBytes(&file);

		if ((rgba == NULL) ||
			(width != LOOSE_ICON_WIDTH) ||
			(height != LOOSE_ICON_HEIGHT))
		{
			if (rgba != NULL)
			{
				stbi_image_free(rgba);
			}

			return 0;
		}

		for (int y = 0; y < LOOSE_ICON_HEIGHT; y++)
		{
			for (int x = 0; x < LOOSE_ICON_WIDTH; x++)
			{
				const unsigned char *source =
					&rgba[(y * LOOSE_ICON_WIDTH + x) * 4];

				int paletteIndex;
				u16 rgb555;

				// Fully transparent PNG pixels use PS1 CLUT entry zero.
				if (source[3] == 0)
				{
					paletteIndex = 0;
				}
				else
				{
					rgb555 =
						((u16)(source[0] >> 3) << 0) |
						((u16)(source[1] >> 3) << 5) |
						((u16)(source[2] >> 3) << 10);

					// RGB555 zero is transparent in CTR's texture renderer.
					if (rgb555 == 0)
					{
						// RGB555 zero is transparent; bit 15 makes black visible.
						rgb555 = 0x8000;
					}

					for (paletteIndex = 1;
						 paletteIndex < paletteCount;
						 paletteIndex++)
					{
						if (clut[paletteIndex] == rgb555)
						{
							break;
						}
					}

					if (paletteIndex == paletteCount)
					{
						if (paletteCount == 16)
						{
							stbi_image_free(rgba);
							return 0;
						}

						clut[paletteCount] = rgb555;
						paletteCount++;
					}
				}

				packedPixels[
					y * LOOSE_ICON_WORDS_PER_ROW + (x / 4)
				] |= (u16)(paletteIndex << ((x & 3) * 4));
			}
		}

		stbi_image_free(rgba);
		return 1;
	}

	static b32 LOAD_ApplyLooseRacerIcon(
    struct Icon *icon,
    const char *assetName)
	{
		char path[128];
		u16 packedPixels[
			LOOSE_ICON_WORDS_PER_ROW *
			LOOSE_ICON_HEIGHT];
		u16 clut[16];

		if (icon == NULL || assetName == NULL)
			return false;

		snprintf(
			path,
			sizeof(path),
			"mods/racer_icons/%s.png",
			assetName);

		if (!LOAD_ReadLooseRacerIcon(
				path,
				packedPixels,
				clut))
		{
			return false;
		}

		struct TextureLayout *layout =
			&icon->texLayout;

		int pageX =
			(layout->tpage & 0x0f) << 6;

		int pageY =
			(layout->tpage & 0x10) ? 256 : 0;

		int textureX =
			pageX + layout->u0 / 4;

		int textureY =
			pageY + layout->v0;

		int clutX =
			(layout->clut & 0x3f) << 4;

		int clutY =
			layout->clut >> 6;

		RECT16 textureRect = {
			textureX,
			textureY,
			LOOSE_ICON_WORDS_PER_ROW,
			LOOSE_ICON_HEIGHT,
		};

		RECT16 clutRect = {
			clutX,
			clutY,
			16,
			1,
		};

		LoadImage(&textureRect, packedPixels);
		LoadImage(&clutRect, clut);

		return true;
	}

	void LOAD_ApplyLooseRacerIcons(
		struct GameTracker *gGT)
	{
		if (gGT == NULL)
			return;

		/*
		 * Restore the original sixteen icons to their normal physical
		 * locations whenever the level icon data is loaded.
		 */
		for (int characterID = CRASH_BANDICOOT;
			 characterID <= NITROS_OXIDE;
			 characterID++)
		{
			const struct CharacterDef *character =
				CharacterRegistry_GetByID(characterID);

			if (character == NULL)
				continue;

			int iconID =
				data.MetaDataCharacters[characterID].iconID;

			if ((u32)iconID >= len(gGT->ptrIcons))
				continue;

			struct Icon *icon =
				gGT->ptrIcons[iconID];

			if (icon == NULL)
				continue;

			LOAD_ApplyLooseRacerIcon(
				icon,
				character->assetName);
		}
	}
#endif

#if defined(CTR_NATIVE)
#include <platform/native_assets.h>

static void *LOAD_ReadLooseRacerModel(
    int characterID)
{
    const struct CharacterDef *character =
        CharacterRegistry_GetByID(characterID);

    if (character == NULL)
        return NULL;

    char path[128];

    int pathLength = snprintf(
        path,
        sizeof(path),
        "mods/racers/%s.ctr",
        character->assetName);

    if ((pathLength < 0) ||
        ((u32)pathLength >= sizeof(path)))
    {
        return NULL;
    }

    struct NativeAssetsByteBuffer file;

    if (!NativeAssets_ReadBytes(
            path,
            NATIVE_ASSET_READ_DATA_FILE,
            &file))
    {
        return NULL;
    }

    void *modelFile =
        MEMPACK_AllocMem(file.size);

    if (modelFile == NULL)
    {
        NativeAssets_FreeBytes(&file);
        return NULL;
    }

    memcpy(
        modelFile,
        file.data,
        file.size);

    NativeAssets_FreeBytes(&file);

    int ptrMapOffset =
        *(int *)modelFile;

    char *modelData =
        (char *)modelFile +
        LOAD_MODEL_FILE_HEADER_BYTES;

    if (ptrMapOffset >= 0)
    {
        struct DramPointerMap *pointerMap =
            (struct DramPointerMap *)
                &modelData[ptrMapOffset];

        LOAD_RunPtrMap(
            modelData,
            DRAM_GETOFFSETS(pointerMap),
            pointerMap->numBytes >>
                DRAM_POINTER_MAP_WORD_SHIFT);
    }

    return modelData;
}
#endif

static void *sLooseRacerFileBases[CHARACTER_ID_COUNT];

static struct Model *sLooseRacerModels[CHARACTER_ID_COUNT + 1];
static int sLooseRacerModelCount;

void LOAD_LoadAllLooseRacerModels(void)
{
    int outputIndex = 0;

    memset(
        sLooseRacerFileBases,
        0,
        sizeof(sLooseRacerFileBases));

    memset(
        sLooseRacerModels,
        0,
        sizeof(sLooseRacerModels));

    int rosterCount =
        CharacterRegistry_GetCount();

    for (int rosterIndex = 0;
         rosterIndex < rosterCount;
         rosterIndex++)
    {
        const struct CharacterDef *character =
            CharacterRegistry_GetByRosterIndex(
                rosterIndex);

        if (character == NULL)
            continue;

        void *model =
            LOAD_ReadLooseRacerModel(
                character->id);

        if (model == NULL)
            continue;

        if (outputIndex >= CHARACTER_ID_COUNT)
            break;

        sLooseRacerFileBases[outputIndex] =
            model;

        outputIndex++;
    }

    sLooseRacerModelCount =
        outputIndex;
}

void LOAD_LoadLooseRacerModels(int racerCount)
{
    int outputIndex = 0;

    if (racerCount > LOAD_CHARACTER_ID_COUNT)
    {
        racerCount = LOAD_CHARACTER_ID_COUNT;
    }

    memset(sLooseRacerFileBases, 0, sizeof(sLooseRacerFileBases));
    memset(sLooseRacerModels, 0, sizeof(sLooseRacerModels));

    for (int racerIndex = 0;
		 racerIndex < racerCount;
		 racerIndex++)
	{
		int characterID =
			data.characterIDs[racerIndex];

		if (CharacterRegistry_GetByID(
				characterID) == NULL)
		{
			continue;
		}

		void *fileBase =
			LOAD_ReadLooseRacerModel(
				characterID);

		if (fileBase == NULL)
			continue;

		if (outputIndex >= CHARACTER_ID_COUNT)
			break;

		sLooseRacerFileBases[outputIndex] =
			fileBase;

		outputIndex++;
	}

    sLooseRacerModelCount = outputIndex;
}

void LOAD_FinalizeLooseRacerModels(void)
{
    for (int i = 0; i < sLooseRacerModelCount; i++)
    {
        sLooseRacerModels[i] =
			(struct Model *)sLooseRacerFileBases[i];
    }

    sLooseRacerModels[sLooseRacerModelCount] = NULL;
}

struct Model **LOAD_GetLooseRacerModelList(void)
{
    return sLooseRacerModels;
}

void LOAD_ClearLooseRacerModels(void)
{
    memset(sLooseRacerFileBases, 0, sizeof(sLooseRacerFileBases));
    memset(sLooseRacerModels, 0, sizeof(sLooseRacerModels));
    sLooseRacerModelCount = 0;
}

void LOAD_Robots2P(struct BigHeader *bigfile, int p1, int p2, void (*callback)(struct LoadQueueSlot *))
{
	int setIndex;
	u8 *robotSet;
	b32 boolFoundRepeat = false;

	// 8 sets, but only check 7 because the last is the Gem Cups pack (4 bosses).
	for (setIndex = 0; setIndex < LOAD_2P_AI_SET_COUNT; setIndex++)
	{
		robotSet = data.characterIDs_2P_AIs[setIndex];

		boolFoundRepeat = false;
		for (int racerIndex = 0; racerIndex < LOAD_2P_AI_SET_RACER_COUNT; racerIndex++)
		{
			if ((robotSet[racerIndex] == p1) || (robotSet[racerIndex] == p2))
			{
				boolFoundRepeat = true;
				break;
			}
		}

		if (!boolFoundRepeat)
		{
			break;
		}
	}

	if (setIndex >= LOAD_2P_AI_SET_COUNT)
	{
		return;
	}

	data.characterIDs[2] = robotSet[0];
	data.characterIDs[3] = robotSet[1];
	data.characterIDs[4] = robotSet[2];
	data.characterIDs[5] = robotSet[3];

	LOAD_AppendQueue(bigfile, LT_GETADDR, BI_2PARCADEPACK + setIndex, NULL, callback);
	LOAD_LoadLooseRacerModels(6);
}

void LOAD_Robots1P(int characterID, int levelID)
{
	const u8 *trackRoster = sTrackRosters[levelID];
	const int *customRoster = NULL;
	int customRosterCount = 0;
	int outputSlot = 1;

#if defined(CTR_NATIVE)
	customRoster = LevelRegistry_GetRacers(levelID, &customRosterCount);
#endif

	data.characterIDs[0] = characterID;

	 for (int rosterSlot = 0;
         rosterSlot < LOAD_CHARACTER_ID_COUNT &&
         outputSlot < LOAD_CHARACTER_ID_COUNT;
         rosterSlot++)
    {
		int rosterCharacter = customRosterCount == LOAD_CHARACTER_ID_COUNT ?
			customRoster[rosterSlot] : trackRoster[rosterSlot];

        if (rosterCharacter == characterID)
        {
            continue;
        }

        data.characterIDs[outputSlot] = rosterCharacter;
        outputSlot++;
    }

    /*
     * The player wasn't in the track roster, so only seven entries
     * were copied. The roster's eighth entry is consequently omitted.
     */
    LOAD_LoadLooseRacerModels(LOAD_CHARACTER_ID_COUNT);
}

static void (*const LOAD_DriverMPK_SetPointer)(struct LoadQueueSlot *) = LOAD_QUEUE_CALLBACK_SET_POINTER;

int LOAD_DriverMPK(struct BigHeader *bigfile, int levelLOD, void (*callback)(struct LoadQueueSlot *))
{
	int gameMode1;

	struct GameTracker *gGT = sdata->gGT;
	gameMode1 = gGT->gameMode1;

	int lastFileIndexMPK;

	// 3P/4P
	if ((u32)(levelLOD - LOAD_LEVEL_LOD_3P) < LOAD_LEVEL_LOD_3P4P_COUNT)
	{
		// load 4P MPK of fourth player
		lastFileIndexMPK = BI_4PARCADEPACK + CharacterRegistry_GetDriverPackID(data.characterIDs[3]);
		LOAD_LoadLooseRacerModels(4);
	}

	else if (levelLOD == LOAD_LEVEL_LOD_1P)
	{
		if ((gameMode1 & (TIME_TRIAL | MAIN_MENU)) == TIME_TRIAL)
		{
			goto LoadHighAndPack;
		}

		if (
		    // adv/cutscene mpk when we just need text from MPK
		    ((gameMode1 & (GAME_CUTSCENE | ADVENTURE_ARENA)) != 0) ||

		    // credits
		    ((gGT->gameMode2 & CREDITS) != 0) ||

		    // adventure character select
		    (gGT->levelID == ADVENTURE_GARAGE))
		{
			lastFileIndexMPK = BI_ADVENTUREPACK + CharacterRegistry_GetDriverPackID(data.characterIDs[0]);
			goto QueueLastPack;
		}

		if ((gameMode1 & ADVENTURE_BOSS) != 0)
		{
			goto LoadHighAndPack;
		}

		if (
		    // If you are in Adventure cup
		    ((gameMode1 & ADVENTURE_CUP) != 0) &&

		    // purple gem cup
		    (gGT->cup.cupID == 4))
		{

			// pack of four AIs with bosses
			LOAD_AppendQueue(bigfile, LT_GETADDR, BI_2PARCADEPACK + LOAD_PURPLE_GEM_CUP_AI_SET_INDEX, NULL, callback);

			data.characterIDs[1] = RIPPER_ROO;
			data.characterIDs[2] = PAPU_PAPU;
			data.characterIDs[3] = KOMODO_JOE;
			data.characterIDs[4] = PINSTRIPE;

			LOAD_LoadLooseRacerModels(5);

			return sdata->ptrMPK;
		}

		if ((gameMode1 & MAIN_MENU) != 0)
		{
			// Character select needs every racer model available.
			LOAD_LoadAllLooseRacerModels();
		}
		else if ((gameMode1 & (TIME_TRIAL | MAIN_MENU)) != MAIN_MENU)
		{
			LOAD_Robots1P(data.characterIDs[0],gGT->levelID);
		}

		// arcade mpk
		lastFileIndexMPK = BI_1PARCADEPACK + CharacterRegistry_GetDriverPackID(data.characterIDs[0]);
	}

	else if ((levelLOD == LOAD_LEVEL_LOD_RELIC) || ((gameMode1 & TIME_TRIAL) != 0))
	{
	LoadHighAndPack:
		if ((gameMode1 & TIME_TRIAL) != 0)
		{
			/*
			 * Load the human player's loose racer model.
			 * characterIDs[1] is reserved for the ghost.
			 */
			LOAD_LoadLooseRacerModels(1);
		}

		/*
		 * Load boss or ghost model.
		 */
		lastFileIndexMPK = BI_TIMETRIALPACK + CharacterRegistry_GetDriverPackID(data.characterIDs[1]);
	}

	// else if (levelLOD == LOAD_LEVEL_LOD_2P)
	else
	{
		LOAD_Robots2P(bigfile, data.characterIDs[0], data.characterIDs[1], callback);
		return sdata->ptrMPK;
	}

QueueLastPack:
	Platform_Log(
    "Driver MPK: character=%d packCharacter=%d file=%d\n",
    data.characterIDs[0],
    CharacterRegistry_GetDriverPackID(
        data.characterIDs[0]),
    lastFileIndexMPK);
	LOAD_AppendQueue(bigfile, LT_GETADDR, lastFileIndexMPK, NULL, callback);
	return sdata->ptrMPK;
}

struct LngFile
{
	int numStrings;
	int offsetToPtrArr;
	char strings[1];
};

// param_1 - Pointer to "cd position of bigfile"
// param_2 - language index - 0 ja, 1 en, 2 en2, 3 fr, 4 de, 5 it, 6 es, 7 ne
void LOAD_LangFile(int bigfilePtr, int lang)
{
	struct LngFile *lngFile;
	u32 size;

	int i;
	int numStrings;
	char **strArray;


	if (sdata->lngFile == 0)
	{
		sdata->lngFile = MEMPACK_AllocMem(sdata->langBufferSize /* "lang buffer" */);
	}

	lngFile = sdata->lngFile;

	lngFile = LOAD_ReadFile_ex((struct BigHeader *)bigfilePtr, LT_SETADDR, BI_LANGUAGEFILE + lang, lngFile, &size, NULL);
	if (lngFile == NULL)
	{
		return;
	}

	numStrings = lngFile->numStrings;
	strArray = (char **)((u32)lngFile + lngFile->offsetToPtrArr);

	sdata->numLngStrings = numStrings;
	sdata->lngStrings = strArray;

	for (i = 0; i < numStrings; i++)
	{
		strArray[i] = (char *)((u32)strArray[i] + (u32)lngFile);
	}
}

int LOAD_GetBigfileIndex(u32 levelID, int lod, int fileIndexInGroup)
{
	if (levelID < NITRO_COURT)
	{
		return BI_ARCADETRACKS + levelID * LOAD_TRACK_FILES_PER_LOD_GROUP + sdata->levBigLodIndex[lod - 1] + fileIndexInGroup;
	}

	if ((u32)(levelID - NITRO_COURT) < LOAD_BATTLE_TRACK_COUNT)
	{
		return BI_BATTLETRACKS + (levelID - NITRO_COURT) * LOAD_TRACK_FILES_PER_LOD_GROUP + sdata->levBigLodIndex[lod - 1] + fileIndexInGroup;
	}

	if ((u32)(levelID - INTRO_RACE_TODAY) < LOAD_INTRO_CUTSCENE_COUNT)
	{
		return BI_CUTSCENES_INTRO + (levelID - INTRO_RACE_TODAY) * LOAD_CUTSCENE_FILES_PER_LEVEL + fileIndexInGroup;
	}

	if ((u32)(levelID - OXIDE_ENDING) < LOAD_OUTRO_CUTSCENE_COUNT)
	{
		return BI_CUTSCENES_OUTRO + (levelID - OXIDE_ENDING) * LOAD_OUTRO_FILES_PER_LEVEL + fileIndexInGroup;
	}

	if (levelID == ADVENTURE_GARAGE)
	{
		return BI_MAINMENUFILE + LOAD_MAIN_MENU_GARAGE_FILE_OFFSET + fileIndexInGroup;
	}

	if (levelID == NAUGHTY_DOG_CRATE)
	{
		return BI_NDBOX + fileIndexInGroup;
	}

	if ((u32)(levelID - CREDITS_CRASH) < LOAD_CREDIT_LEVEL_COUNT)
	{
		return BI_CREDITS + (levelID - CREDITS_CRASH) * LOAD_CUTSCENE_FILES_PER_LEVEL + fileIndexInGroup;
	}

	if (levelID == MAIN_MENU_LEVEL)
	{
		return BI_MAINMENUFILE + fileIndexInGroup;
	}

	if (levelID == SCRAPBOOK)
	{
		return BI_SCRAPBOOK + fileIndexInGroup;
	}

	return BI_ADVENTUREHUB + (levelID - GEM_STONE_VALLEY) * LOAD_CUTSCENE_FILES_PER_LEVEL + fileIndexInGroup;
}

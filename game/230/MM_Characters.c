#include <common.h>
#include <CharacterRegistry.h>
#include <CharacterIconCache.h>

enum
{
	MM_CHARACTER_SELECT_SCREEN_W = 0x200,
	MM_CHARACTER_SELECT_SCREEN_H = 0xd8,
	MM_CHARACTER_SELECT_DISTANCE_TO_SCREEN = 0x100,
	MM_CHARACTER_SELECT_MODEL_MOVE_FP = 0x1000,
	MM_CHARACTER_SELECT_MODEL_MOVE_FP_SHIFT = 0xc,
	MM_CHARACTER_SELECT_MODEL_MOVE_NEXT = 1,
	MM_CHARACTER_SELECT_MODEL_MOVE_PREV = -1,
	MM_CHARACTER_SELECT_ICON_COUNT = 0xf,
	MM_CHARACTER_SELECT_EXPANSION_ICON_FIRST = 0xc,
	MM_CHARACTER_SELECT_DEFAULT_DRIVER_COUNT = 8,
	MM_CHARACTER_SELECT_MAX_PLAYERS = 4,
	MM_CHARACTER_SELECT_LIMITED_LAYOUT_OFFSET = 4,
	MM_CHARACTER_SELECT_FULL_LAYOUT_COUNT = 2,
	MM_CHARACTER_SELECT_TRANSITION_FRAMES = 0xc,
	MM_CHARACTER_SELECT_TRANSITION_STEP = 8,
	MM_CHARACTER_SELECT_ANGLE_STEP = 0x400,
	MM_CHARACTER_SELECT_ANGLE_OFFSET = 400,
	MM_CHARACTER_SELECT_SPIN_STEP = 0x40,
	MM_CHARACTER_SELECT_LAYOUT_3P = 2,
	MM_CHARACTER_SELECT_LAYOUT_4P = 3,
	MM_CHARACTER_SELECT_LAYOUT_1P_LIMITED = 4,
	MM_CHARACTER_SELECT_LAYOUT_2P_LIMITED = 5,
	MM_CHARACTER_SELECT_TITLE_TRANSITION_INDEX = 15,
	MM_CHARACTER_SELECT_DRIVER_WINDOW_TRANSITION_FIRST = 0x10,
	MM_CHARACTER_SELECT_3P_TITLE_X = 0x9c,
	MM_CHARACTER_SELECT_3P_SELECT_Y = 0x14,
	MM_CHARACTER_SELECT_3P_CHARACTER_Y = 0x26,
	MM_CHARACTER_SELECT_4P_TITLE_X = 0xfc,
	MM_CHARACTER_SELECT_4P_SELECT_Y = 8,
	MM_CHARACTER_SELECT_4P_CHARACTER_Y = 0x18,
	MM_CHARACTER_SELECT_LIMITED_TITLE_X = 0xfc,
	MM_CHARACTER_SELECT_LIMITED_TITLE_Y = 10,
	MM_CHARACTER_SELECT_INPUT_DPAD = BTN_RIGHT | BTN_LEFT | BTN_DOWN | BTN_UP,
	MM_CHARACTER_SELECT_INPUT_MENU = BTN_TRIANGLE | BTN_CIRCLE | BTN_SQUARE_one | BTN_CROSS_one,
	MM_CHARACTER_SELECT_INPUT_CONFIRM = BTN_CIRCLE | BTN_CROSS_one,
	MM_CHARACTER_SELECT_INPUT_BACK = BTN_TRIANGLE | BTN_SQUARE_one,
	MM_CHARACTER_SELECT_ICON_DECAL_OFFSET_X = 6,
	MM_CHARACTER_SELECT_ICON_DECAL_OFFSET_Y = 4,
	MM_CHARACTER_SELECT_ICON_RECT_W = 0x34,
	MM_CHARACTER_SELECT_ICON_RECT_H = 0x21,
	MM_CHARACTER_SELECT_CURSOR_LABEL_OFFSET_X = -6,
	MM_CHARACTER_SELECT_CURSOR_LABEL_OFFSET_Y = -3,
	MM_CHARACTER_SELECT_HIGHLIGHT_OFFSET_X = 3,
	MM_CHARACTER_SELECT_HIGHLIGHT_OFFSET_Y = 2,
	MM_CHARACTER_SELECT_HIGHLIGHT_W = 0x2e,
	MM_CHARACTER_SELECT_HIGHLIGHT_H = 0x1d,
	MM_CHARACTER_SELECT_SELECTED_BORDER_COUNT = 2,
	MM_CHARACTER_SELECT_SELECTED_BORDER_INSET_X = 3,
	MM_CHARACTER_SELECT_SELECTED_BORDER_INSET_Y = 2,
	MM_CHARACTER_SELECT_SELECTED_BORDER_SHRINK_W = 6,
	MM_CHARACTER_SELECT_SELECTED_BORDER_SHRINK_H = 4,
	MM_CHARACTER_SELECT_4P_NAME_BOTTOM_OFFSET = -6,
	MM_CHARACTER_SELECT_COLOR_PHASE_FRAME_STEP = 0x100,
	MM_CHARACTER_SELECT_COLOR_PHASE_PLAYER_STEP = 0x400,
	MM_CHARACTER_SELECT_COLOR_TRIG_MASK = 0x3ff,
	MM_CHARACTER_SELECT_COLOR_TRIG_HIGH_HALF_BIT = 0x400,
	MM_CHARACTER_SELECT_COLOR_TRIG_NEGATE_BIT = 0x800,
	MM_CHARACTER_SELECT_COLOR_PULSE_THRESHOLD = 0xc00,
	MM_CHARACTER_SELECT_COLOR_PULSE_SCALE_SHIFT = 7,
	MM_CHARACTER_SELECT_COLOR_PULSE_FP_SHIFT = 0xc,
	MM_CHARACTER_SELECT_PAGE_HINT_X = 0x100,
    MM_CHARACTER_SELECT_PAGE_HINT_Y = 0xc6,
    MM_CHARACTER_SELECT_PAGE_HINT_BUFFER_SIZE = 32,
};

enum
{
    MM_CHARACTER_PAGE_SIZE = 8,
};

static s16 s_characterPage;
static s16 s_characterCursor;

static int MM_Characters_GetPageCount(void)
{
    return (CharacterRegistry_GetCount() +
            MM_CHARACTER_PAGE_SIZE - 1) /
           MM_CHARACTER_PAGE_SIZE;
}

static int MM_Characters_GetPageCharacterCount(int page)
{
    int remaining =
        CharacterRegistry_GetCount() -
        page * MM_CHARACTER_PAGE_SIZE;

    if (remaining <= 0)
        return 0;

    if (remaining > MM_CHARACTER_PAGE_SIZE)
        return MM_CHARACTER_PAGE_SIZE;

    return remaining;
}

static int MM_Characters_GetHighlightedID(void)
{
    int rosterIndex =
    s_characterPage * MM_CHARACTER_PAGE_SIZE +
    s_characterCursor;

	const struct CharacterDef *character =
		CharacterRegistry_GetByRosterIndex(rosterIndex);

	return character != NULL
		? character->id
		: CRASH_BANDICOOT;
}

static void MM_Characters_DrawPageHint(void)
{
    char pageText[
        MM_CHARACTER_SELECT_PAGE_HINT_BUFFER_SIZE];

    int pageCount =
        MM_Characters_GetPageCount();

    /*
     * No need to display paging controls when the roster
     * fits on a single page.
     */
    if (pageCount <= 1)
    {
        return;
    }

    snprintf(
        pageText,
        sizeof(pageText),
        "L1    %d / %d    R1",
        (int)s_characterPage + 1,
        pageCount);

    DecalFont_DrawLine(
        pageText,
        MM_CHARACTER_SELECT_PAGE_HINT_X,
        MM_CHARACTER_SELECT_PAGE_HINT_Y,
        FONT_SMALL,
        JUSTIFY_CENTER | WHITE);
}

void MM_Characters_ReloadPageIcons(void)
{
    struct GameTracker *gGT =
        sdata->gGT;

    if (gGT == NULL)
        return;

    CharacterIconCache_LoadRosterPage(
        gGT,
        s_characterPage);
}

void MM_Characters_AnimateColors(u8 *colorData, s16 playerID, s16 flag)
{
	u8 colorAdjustmentValue;
	u32 trigApproximationIndex;
	u32 trigApprox;

	// access int RGBA as a char array,
	// for editing components of color
	u8 *ptrColor = (u8 *)data.ptrColor[playerID + PLAYER_BLUE];

	trigApprox = 0;

	// if player has not selected character yet
	// see MM_Characters_MenuProc
	if (flag == 0)
	{
		trigApproximationIndex = sdata->frameCounter * MM_CHARACTER_SELECT_COLOR_PHASE_FRAME_STEP + playerID * MM_CHARACTER_SELECT_COLOR_PHASE_PLAYER_STEP;

		// approximate trigonometry
		trigApprox = CTR_ReadU32LE(&data.trigApprox[trigApproximationIndex & MM_CHARACTER_SELECT_COLOR_TRIG_MASK]);

		if ((trigApproximationIndex & MM_CHARACTER_SELECT_COLOR_TRIG_HIGH_HALF_BIT) == 0)
		{
			trigApprox = trigApprox << 0x10;
		}
		trigApprox = trigApprox >> 0x10;

		if ((trigApproximationIndex & MM_CHARACTER_SELECT_COLOR_TRIG_NEGATE_BIT) != 0)
		{
			trigApprox = -(int)trigApprox;
		}
	}

	colorAdjustmentValue = 0;
	if (MM_CHARACTER_SELECT_COLOR_PULSE_THRESHOLD < (int)trigApprox)
	{
		colorAdjustmentValue = ((trigApprox << MM_CHARACTER_SELECT_COLOR_PULSE_SCALE_SHIFT) >> MM_CHARACTER_SELECT_COLOR_PULSE_FP_SHIFT);
	}

	colorData[0] = ptrColor[0] | colorAdjustmentValue;
	colorData[1] = ptrColor[1] | colorAdjustmentValue;
	colorData[2] = ptrColor[2] | colorAdjustmentValue;
	colorData[3] = 0;

	return;
}

static void MM_Characters_MoveCursor(u32 button)
{
    int row = s_characterCursor / 4;
    int column = s_characterCursor % 4;
    int previous = s_characterCursor;

    if (button & BTN_LEFT)
        column = (column + 3) % 4;
    else if (button & BTN_RIGHT)
        column = (column + 1) % 4;
    else if (button & BTN_UP)
        row = (row + 1) % 2;
    else if (button & BTN_DOWN)
        row = (row + 1) % 2;

    s_characterCursor = row * 4 + column;

    int count =
        MM_Characters_GetPageCharacterCount(s_characterPage);

	D230.characterSelectPlayerState.modelMoveDir[0] =
    (button & (BTN_RIGHT | BTN_DOWN))
        ? MM_CHARACTER_SELECT_MODEL_MOVE_NEXT
        : MM_CHARACTER_SELECT_MODEL_MOVE_PREV;

    if (s_characterCursor >= count)
        s_characterCursor = count - 1;

    if (s_characterCursor != previous)
        OtherFX_Play(0, 1);
}

int MM_Characters_GetNextDriver(s16 direction, s16 characterID)
{
	u8 nextIcon = D230.activeCharacterSelectMeta[(s32)characterID].nextIconByDirection[direction];
	s16 unlocked = D230.activeCharacterSelectMeta[(s32)nextIcon].unlockFlags;

	// set new driver to the driver
	// you'd get when pressing Up button
	s16 newDriver = nextIcon;

	if (
	    // if desired driver is not unlocked by default
	    (unlocked != MM_CHARACTER_UNLOCK_ALWAYS) &&

	    !CHECK_ADV_BIT(sdata->gameProgress.unlocks, unlocked))
	{
		// set new driver to the driver you already have
		newDriver = characterID;
	}

	// return new driver
	return newDriver;
}

// used for preventing players highlighting the same character
// also for when you go left of komodo joe's icon
b32 MM_Characters_boolIsInvalid(s16 *iconPerPlayer, s16 characterID, s16 player)
{
	// if there are players
	if (sdata->gGT->numPlyrNextGame)
	{
		// loop through players
		for (s16 playerIndex = 0; playerIndex < sdata->gGT->numPlyrNextGame; playerIndex++)
		{
			// if driver is taken
			if ((playerIndex != player) && (characterID == s_characterCursor))
			{
				return 1;
			}
		}
	}

	// if driver is not taken
	return 0;
}

// Search for character model by string,
// specific to main menu lev, altered in oxide mod
struct Model *MM_Characters_GetModelByName(const char *name)
{
	struct Model **models;
	struct Model *model;
	struct Level *level1 = sdata->gGT->level1;
	struct Model **looseModels = LOAD_GetLooseRacerModelList();

	// if LEV is invalid
	if (level1 == NULL)
	{
		return NULL;
	}

	if ((looseModels != NULL) && (looseModels[0] != NULL))
	{
		for (int i = 0; looseModels[i] != NULL; i++)
		{
			model = looseModels[i];

			if ((ModelName_ReadWord(model->name, 0) == ModelName_ReadWord(name, 0)) &&
				(ModelName_ReadWord(model->name, 1) == ModelName_ReadWord(name, 1)) &&
				(ModelName_ReadWord(model->name, 2) == ModelName_ReadWord(name, 2)) &&
				(ModelName_ReadWord(model->name, 3) == ModelName_ReadWord(name, 3)))
			{
				return model;
			}
		}
	}

	models = level1->ptrModelsPtrArray;
	if (models == NULL)
	{
		return NULL;
	}

	// loop through all models in array
	// of model pointers, until nullptr
	for (model = models[0]; model != NULL; models++, model = models[0])
	{
		if ((ModelName_ReadWord(model->name, 0) == ModelName_ReadWord(name, 0)) && (ModelName_ReadWord(model->name, 1) == ModelName_ReadWord(name, 1)) &&
		    (ModelName_ReadWord(model->name, 2) == ModelName_ReadWord(name, 2)) && (ModelName_ReadWord(model->name, 3) == ModelName_ReadWord(name, 3)))
		{
			// found it
			return model;
		}
	}
	return NULL;
}

void MM_Characters_DrawWindows(b32 boolShowDrivers)
{
	struct GameTracker *gGT = sdata->gGT;
	SVec3 rot;

	if (boolShowDrivers != 0)
	{
		// enable drawing wheels
		gGT->renderFlags |= RENDER_FLAG_TIRES;
	}

	for (s32 playerIndex = 0; playerIndex < gGT->numPlyrNextGame; playerIndex++)
	{
		SVec2 *windowPos = &D230.activeCharacterSelectWindowPos[playerIndex];
		struct TransitionMeta *tMeta = &D230.characterSelectTransitionMeta[playerIndex];

		struct PushBuffer *pb = &gGT->pushBuffer[playerIndex];
		pb->rect.x = windowPos->x + tMeta[MM_CHARACTER_SELECT_DRIVER_WINDOW_TRANSITION_FIRST].currX;
		pb->rect.y = windowPos->y + tMeta[MM_CHARACTER_SELECT_DRIVER_WINDOW_TRANSITION_FIRST].currY;
		pb->rect.w = D230.characterSelectWindowWidth;
		pb->rect.h = D230.characterSelectWindowHeight;

		// negative StartX
		if ((s16)pb->rect.x < 0)
		{
			pb->rect.w -= pb->rect.x;
			pb->rect.x = 0;
			if ((s16)pb->rect.w < 0)
			{
				pb->rect.w = 0;
			}
		}

		// negative StartY
		if ((s16)pb->rect.y < 0)
		{
			pb->rect.h -= pb->rect.y;
			pb->rect.y = 0;
			if ((s16)pb->rect.h < 0)
			{
				pb->rect.h = 0;
			}
		}

		// startX + sizeX out of bounds
		if ((MM_CHARACTER_SELECT_SCREEN_W < pb->rect.x + pb->rect.w) && (pb->rect.w = MM_CHARACTER_SELECT_SCREEN_W - pb->rect.x, pb->rect.w < 0))
		{
			pb->rect.x = MM_CHARACTER_SELECT_SCREEN_W;
			pb->rect.w = 0;

#ifdef CTR_NATIVE
			// NOTE(aalhendi): Native renderer guard; retail leaves w at zero.
			pb->rect.w = 1;
#endif
		}

		// startY + sizeY out of bounds
		if ((MM_CHARACTER_SELECT_SCREEN_H < pb->rect.y + pb->rect.h) && (pb->rect.h = MM_CHARACTER_SELECT_SCREEN_H - pb->rect.y, pb->rect.h < 0))
		{
			pb->rect.y = MM_CHARACTER_SELECT_SCREEN_H;
			pb->rect.h = 0;

#ifdef CTR_NATIVE
			// NOTE(aalhendi): Native renderer guard; retail leaves h at zero.
			pb->rect.h = 1;
#endif
		}

		// distanceToScreen
		pb->distanceToScreen_CURR = MM_CHARACTER_SELECT_DISTANCE_TO_SCREEN;
		pb->distanceToScreen_PREV = MM_CHARACTER_SELECT_DISTANCE_TO_SCREEN;

		// pushBuffer pos and rot to all zero
		pb->pos.x = 0;
		pb->pos.y = 0;
		pb->pos.z = 0;
		pb->rot.x = 0;
		pb->rot.y = 0;
		pb->rot.z = 0;

		// player -> instance
		struct Driver *driver = gGT->drivers[playerIndex];

		struct Instance *driverInst = driver->instSelf;

		// Make Visible
		driverInst->flags &= ~HIDE_MODEL;

		// if driver is off-screen
		if ((gGT->numPlyrNextGame <= playerIndex) || (boolShowDrivers == 0))
		{
			// invisible
			driverInst->flags |= HIDE_MODEL;
		}

		struct InstDrawPerPlayer *idpp = INST_GETIDPP(driverInst);

		// clear pushBuffer in every InstDrawPerPlayer
		idpp[0].pushBuffer = 0;
		idpp[1].pushBuffer = 0;
		idpp[2].pushBuffer = 0;
		idpp[3].pushBuffer = 0;

		// set pushBuffer in InstDrawPerPlayer,
		// so that each camera can only see one driver
		idpp[playerIndex].pushBuffer = pb;

		s16 *currCharacterID = &D230.characterSelectPlayerState.currentCharacterID[playerIndex];

		/*
		 * Character select reuses an existing Driver while replacing its
		 * model, so wheel size must be refreshed for the displayed character.
		 */
		driver->wheelSize =
			CharacterRegistry_HasWheels(
				*currCharacterID)
				? 0xccc
				: 0;

		driverInst->animFrame = 0;
		driverInst->animIndex = 0;

		struct Model *model = MM_Characters_GetModelByName(CharacterRegistry_GetAssetName(*currCharacterID));

		// set modelPtr in Instance
		driverInst->model = model;

		// CameraDC, freecam mode
		gGT->cameraDC[playerIndex].cameraMode = CAMERA_MODE_FREECAM;

		// Set position of player
		driverInst->matrix.t[0] = D230.characterSelectDriverModel.pos.x;
		driverInst->matrix.t[1] = D230.characterSelectDriverModel.pos.y;
		driverInst->matrix.t[2] = D230.characterSelectDriverModel.pos.z;

		s16 *moveTimer = &D230.characterSelectModelMoveTimer[playerIndex];
		s16 nextMoveTimer = *moveTimer + -1;

		// If no transition between players
		if (*moveTimer == 0)
		{
			// compare to character ID
			if (*currCharacterID != data.characterIDs[playerIndex])
			{
				*moveTimer = D230.characterSelectDriverModel.moveFrames << 1;
				D230.characterSelectPlayerState.desiredCharacterID[playerIndex] = data.characterIDs[playerIndex];
			}
		}

		// if transition between players
		else
		{
			// get timer
			*moveTimer = nextMoveTimer;

			s32 slideDirection;
			s32 slideOffset;

			// if timer is before midpoint
			if ((int)nextMoveTimer < (int)D230.characterSelectDriverModel.moveFrames)
			{
				// make driver fly off screen
				*currCharacterID = D230.characterSelectPlayerState.desiredCharacterID[playerIndex];
				s32 moveFrameScale = RaceFlag_MoveModels((int)nextMoveTimer, (int)D230.characterSelectDriverModel.moveFrames);

				// direction moving
				slideDirection = -D230.characterSelectPlayerState.modelMoveDir[playerIndex];
				slideOffset = moveFrameScale * D230.characterSelectDriverModel.slideDistance >> MM_CHARACTER_SELECT_MODEL_MOVE_FP_SHIFT;
			}

			// if timer is after midpoint
			else
			{
				// make new driver fly on screen
				s32 moveFrameScale =
				    RaceFlag_MoveModels((int)nextMoveTimer - (int)D230.characterSelectDriverModel.moveFrames, (int)D230.characterSelectDriverModel.moveFrames);

				// direction moving
				slideDirection = D230.characterSelectPlayerState.modelMoveDir[playerIndex];
				slideOffset = (MM_CHARACTER_SELECT_MODEL_MOVE_FP - moveFrameScale) * (int)D230.characterSelectDriverModel.slideDistance >>
				              MM_CHARACTER_SELECT_MODEL_MOVE_FP_SHIFT;
			}

			driverInst->matrix.t[0] += slideDirection * slideOffset;
		}

		// driver rotation
		rot.x = D230.characterSelectDriverModel.rot.x;
		rot.y = D230.characterSelectDriverModel.rot.y + D230.characterSelectPlayerState.angle[playerIndex];
		rot.z = D230.characterSelectDriverModel.rot.z;

		ConvertRotToMatrix(&driverInst->matrix, &rot);
	}
	return;
}

void MM_Characters_SetMenuLayout(void)
{
	s32 layoutIndex = 0;

	D230.characterSelectRosterExpanded = 1;
	D230.characterSelectLayoutIndex = layoutIndex;

	D230.characterSelectDriverModel.pos.y =
		D230.characterSelectLayout.driverPosY[layoutIndex];

	D230.characterSelectDriverModel.pos.z =
		D230.characterSelectLayout.driverPosZ[layoutIndex];

	D230.characterSelectWindowWidth =
		D230.characterSelectLayout.windowW[layoutIndex];

	D230.characterSelectWindowHeight =
		D230.characterSelectLayout.windowH[layoutIndex];

	D230.activeCharacterSelectWindowPos =
		D230.characterSelectWindowPosByLayout[layoutIndex];

	D230.activeCharacterSelectMeta =
		D230.characterSelectMeta1P2P;

	D230.characterSelectNameTextY =
		D230.characterSelectLayout.textY[layoutIndex];

	D230.characterSelectTransitionMeta =
		D230.characterSelectTransitionByPlayerCount[0];

	return;
}

void MM_Characters_BackupIDs(void)
{
	for (s32 driverIndex = 0; driverIndex < MM_CHARACTER_SELECT_DEFAULT_DRIVER_COUNT; driverIndex++)
	{
		// make a backup when you leave character selection,
		// backup is restored when you go back to selection
		sdata->characterIDs_backup[driverIndex] = data.characterIDs[driverIndex];
	}
	return;
}

void MM_Characters_PreventOverlap(void)
{
	struct GameTracker *gGT = sdata->gGT;
	s8 availableDefaultCharacters[MM_CHARACTER_SELECT_DEFAULT_DRIVER_COUNT];

	// default 0,1,2,3,4,5,6,7
	CTR_WriteU32LE((u8 *)&availableDefaultCharacters[0], R230.packedDefaultCharacterIDWords[0]);
	CTR_WriteU32LE((u8 *)&availableDefaultCharacters[4], R230.packedDefaultCharacterIDWords[1]);

	for (s32 playerIndex = 0; playerIndex < gGT->numPlyrNextGame; playerIndex++)
	{
		// get character ID
		s32 characterID = data.characterIDs[playerIndex];

		// if not a secret character
		if (characterID < MM_CHARACTER_SELECT_DEFAULT_DRIVER_COUNT)
		{
			// character is taken
			availableDefaultCharacters[characterID] = -1;
		}
	}

	for (s32 playerIndex = 1; playerIndex < gGT->numPlyrNextGame; playerIndex++)
	{
		for (s32 previousPlayer = 0; previousPlayer < playerIndex; previousPlayer++)
		{
			// if two characters are the same
			if (data.characterIDs[playerIndex] == data.characterIDs[previousPlayer])
			{
				// look for a new character
				for (s32 defaultIndex = 0; defaultIndex < MM_CHARACTER_SELECT_DEFAULT_DRIVER_COUNT; defaultIndex++)
				{
					// get default character
					s8 *defaultCharacter = &availableDefaultCharacters[defaultIndex];
					s8 freeCharacter = *defaultCharacter;

					// if character is not taken
					if (-1 < freeCharacter)
					{
						// assign free character
						data.characterIDs[playerIndex] = (s16)freeCharacter;

						// character is now taken
						*defaultCharacter = -1;

						break;
					}
				}
			}
		}
	}
	return;
}

void MM_Characters_RestoreIDs(void)
{
	struct GameTracker *gGT = sdata->gGT;

	// erase select bits
	sdata->characterSelectFlags = 0;
	D230.characterSelectTransitionFrame = MM_CHARACTER_SELECT_TRANSITION_FRAMES;
	D230.characterSelectMenuState = ENTERING_MENU;

	// This uses 80086e84, which controls character IDs
	for (s32 driverIndex = 0; driverIndex < MM_CHARACTER_SELECT_DEFAULT_DRIVER_COUNT; driverIndex++)
	{
		// set character ID to the last ID you entered
		data.characterIDs[driverIndex] = sdata->characterIDs_backup[driverIndex];
	}

	int characterID = data.characterIDs[0];

	if ((characterID < 0) ||
		(characterID >= CharacterRegistry_GetCount()))
	{
		characterID = CRASH_BANDICOOT;
	}

	s_characterPage =
		characterID / MM_CHARACTER_PAGE_SIZE;

	s_characterCursor =
		characterID % MM_CHARACTER_PAGE_SIZE;

	data.characterIDs[0] = characterID;

	MM_Characters_SetMenuLayout();

	CharacterIconCache_LoadRosterPage(
    gGT,
    s_characterPage);

	for (s32 playerIndex = 0; playerIndex < gGT->numPlyrNextGame; playerIndex++)
	{
		// set name string ID to the character ID of each player.
		// The string will only draw if both these variables match
		D230.characterSelectPlayerState.currentCharacterID[playerIndex] = data.characterIDs[playerIndex];
		D230.characterSelectPlayerState.desiredCharacterID[playerIndex] = data.characterIDs[playerIndex];

		// something to do with transitioning between icons
		D230.characterSelectModelMoveTimer[playerIndex] = 0;

		// rotation of each driver, 90 degrees difference
		D230.characterSelectPlayerState.angle[playerIndex] = (playerIndex * MM_CHARACTER_SELECT_ANGLE_STEP) + MM_CHARACTER_SELECT_ANGLE_OFFSET;
	}

	MM_Characters_DrawWindows(0);
	return;
}

static void MM_Characters_ChangePage(int direction)
{
    int pageCount = MM_Characters_GetPageCount();

    s_characterPage =
        (s_characterPage + direction + pageCount) %
        pageCount;

	CharacterIconCache_LoadRosterPage(sdata->gGT,s_characterPage);

    int count =
        MM_Characters_GetPageCharacterCount(
            s_characterPage);

    if (s_characterCursor >= count)
    {
        s_characterCursor = count - 1;
    }

    int characterID =
        MM_Characters_GetHighlightedID();

    data.characterIDs[0] = characterID;

    D230.characterSelectPlayerState
        .desiredCharacterID[0] = characterID;

    D230.characterSelectPlayerState
        .modelMoveDir[0] =
            direction > 0
                ? MM_CHARACTER_SELECT_MODEL_MOVE_NEXT
                : MM_CHARACTER_SELECT_MODEL_MOVE_PREV;

    OtherFX_Play(0, 1);
}

void MM_Characters_HideDrivers(void)
{
	struct GameTracker *gGT = sdata->gGT;

	for (s32 playerIndex = 0; playerIndex < MM_CHARACTER_SELECT_MAX_PLAYERS; playerIndex++)
	{
		PushBuffer_Init(&gGT->pushBuffer[playerIndex], 0, 1);

		gGT->drivers[playerIndex]->instSelf->flags |= HIDE_MODEL;
	}

	return;
}

void MM_Characters_MenuProc(struct RectMenu *unused)
{
	(void)unused;

	RECT drawRect;

	struct GameTracker *gGT = sdata->gGT;

	u32 *ot = gGT->backBuffer->otMem.uiOT;

	// if menu is not in focus
	if (D230.characterSelectMenuState != IN_MENU)
	{
		MM_TransitionInOut(D230.characterSelectTransitionMeta, (int)D230.characterSelectTransitionFrame, MM_CHARACTER_SELECT_TRANSITION_STEP);
	}

	MM_Characters_SetMenuLayout();
	MM_Characters_DrawWindows(1);

	// if transitioning in
	if (D230.characterSelectMenuState == ENTERING_MENU)
	{
		// if no more frames
		if (D230.characterSelectTransitionFrame == 0)
		{
			// menu is now in focus
			D230.characterSelectMenuState = IN_MENU;
		}
		else
		{
			D230.characterSelectTransitionFrame--;
		}
	}

	// if transitioning out
	if (D230.characterSelectMenuState == EXITING_MENU)
	{
		// increase frame
		D230.characterSelectTransitionFrame++;

		// if more than 12 frames
		if (D230.characterSelectTransitionFrame > MM_CHARACTER_SELECT_TRANSITION_FRAMES)
		{
			// Make a backup of the characters
			// you selected in character selection screen
			MM_Characters_BackupIDs();

			// if returning to main menu
			if (D230.characterSelectExitsForward == 0)
			{
				MM_JumpTo_Title_Returning();
				MM_Characters_HideDrivers();
				return;
			}

			MM_Characters_HideDrivers();

			// if you are in a cup
			if ((gGT->gameMode2 & CUP_ANY_KIND) != 0)
			{
				sdata->ptrDesiredMenu = &D230.menuCupSelect;
				MM_CupSelect_Init();
				return;
			}

			// if going to track selection
			sdata->ptrDesiredMenu = &D230.menuTrackSelect;
			MM_TrackSelect_Init();
			return;
		}
	}

	int posX = D230.characterSelectTransitionMeta[MM_CHARACTER_SELECT_TITLE_TRANSITION_INDEX].currX;
	int posY = D230.characterSelectTransitionMeta[MM_CHARACTER_SELECT_TITLE_TRANSITION_INDEX].currY;

	u32 characterSelectType;
	char *characterSelectString;
	switch (D230.characterSelectLayoutIndex)
	{
	// 3P character selection
	case MM_CHARACTER_SELECT_LAYOUT_3P:

		// If you have a lot of characters unlocked, do not draw SELECT CHARACTER
		if (D230.characterSelectRosterExpanded)
		{
			goto dontDrawSelectCharacter;
		}

		DecalFont_DrawLine(sdata->lngStrings[LNG_SELECT_CHARACTER_SELECT], posX + MM_CHARACTER_SELECT_3P_TITLE_X, posY + MM_CHARACTER_SELECT_3P_SELECT_Y,
		                   FONT_BIG, (JUSTIFY_CENTER | ORANGE));
		characterSelectType = FONT_BIG;

		characterSelectString = sdata->lngStrings[LNG_CHARACTER];

		posX = posX + MM_CHARACTER_SELECT_3P_TITLE_X;
		posY = posY + MM_CHARACTER_SELECT_3P_CHARACTER_Y;
		break;

	// 4P character selection
	case MM_CHARACTER_SELECT_LAYOUT_4P:

		// If Fake Crash is unlocked, do not draw "Select Character"
		if (sdata->gameProgress.unlocks[0] & UNLOCK_FAKE_CRASH)
		{
			goto dontDrawSelectCharacter;
		}

		DecalFont_DrawLine(sdata->lngStrings[LNG_SELECT_CHARACTER_SELECT], posX + MM_CHARACTER_SELECT_4P_TITLE_X, posY + MM_CHARACTER_SELECT_4P_SELECT_Y,
		                   FONT_CREDITS, (JUSTIFY_CENTER | ORANGE));
		characterSelectType = FONT_CREDITS;

		characterSelectString = sdata->lngStrings[LNG_CHARACTER];

		posX = posX + MM_CHARACTER_SELECT_4P_TITLE_X;
		posY = posY + MM_CHARACTER_SELECT_4P_CHARACTER_Y;
		break;

	// If you are in 1P or 2P character selection,
	// when you do NOT have a lot of characters selected
	case MM_CHARACTER_SELECT_LAYOUT_1P_LIMITED:
	case MM_CHARACTER_SELECT_LAYOUT_2P_LIMITED:
		characterSelectType = FONT_BIG;

		characterSelectString = sdata->lngStrings[LNG_SELECT_CHARACTER];

		posX = posX + MM_CHARACTER_SELECT_LIMITED_TITLE_X;
		posY = posY + MM_CHARACTER_SELECT_LIMITED_TITLE_Y;
		break;

	default:
		goto dontDrawSelectCharacter;
	}

	// Draw String
	DecalFont_DrawLine(characterSelectString, posX, posY, characterSelectType, (JUSTIFY_CENTER | ORANGE));

dontDrawSelectCharacter:

	for (s32 playerIndex = 0;
     playerIndex < gGT->numPlyrNextGame;
     playerIndex++)
	{
		u16 playerSelectFlag = (u16)(1 << playerIndex);

		Color playerColor;
		MM_Characters_AnimateColors(
			(u8 *)&playerColor,
			playerIndex,
			(int)(s16)(
				sdata->characterSelectFlags &
				playerSelectFlag));

		u32 button = sdata->buttonTapPerPlayer[0];

		if ((D230.characterSelectMenuState == IN_MENU) &&
			((sdata->characterSelectFlags & 1) == 0))
		{
			if (button & BTN_L1)
			{
				MM_Characters_ChangePage(-1);
			}
			else if (button & BTN_R1)
			{
				MM_Characters_ChangePage(1);
			}
			else if (button & MM_CHARACTER_SELECT_INPUT_DPAD)
			{
				MM_Characters_MoveCursor(button);

				data.characterIDs[0] =
					MM_Characters_GetHighlightedID();

				D230.characterSelectPlayerState
					.desiredCharacterID[0] =
						data.characterIDs[0];
			}

			if (button & MM_CHARACTER_SELECT_INPUT_CONFIRM)
			{
				data.characterIDs[0] =
					MM_Characters_GetHighlightedID();

				sdata->characterSelectFlags |= 1;
				D230.characterSelectExitsForward = 1;
				D230.characterSelectMenuState = EXITING_MENU;
				OtherFX_Play(1, 1);
			}

			if (button & MM_CHARACTER_SELECT_INPUT_BACK)
			{
				D230.characterSelectExitsForward = 0;
				D230.characterSelectMenuState = EXITING_MENU;
				OtherFX_Play(2, 1);
			}

			sdata->buttonTapPerPlayer[0] = 0;
		}

		/*
		 * Resolve these after input because input may have
		 * changed the page or cursor.
		 */
		s16 currentIcon = s_characterCursor;

		struct CharacterSelectMeta *visualSlot =
			&D230.characterSelectMeta1P2P[currentIcon];

		struct TransitionMeta *currentIconTransition =
			&D230.characterSelectTransitionMeta[currentIcon];


		// if player has not selected a character
		b32 playerSelectedAfterInput = ((sdata->characterSelectFlags >> playerIndex) & 1U) != 0;
		Color outlineColor;
		if (!playerSelectedAfterInput)
		{
			// draw string
			// "1", "2", "3", "4", above the character icon
			DecalFont_DrawLine(D230.playerNumberStrings[playerIndex], currentIconTransition->currX + (u32)visualSlot->posX - 6,
			                   currentIconTransition->currY + (u32)visualSlot->posY - 3, FONT_BIG, WHITE);
			outlineColor = playerColor;
		}
		else
		{
			outlineColor = D230.characterSelect_Outline;
		}

		drawRect.x = currentIconTransition->currX + visualSlot->posX;
		drawRect.y = currentIconTransition->currY + visualSlot->posY;
		drawRect.w = MM_CHARACTER_SELECT_ICON_RECT_W;
		drawRect.h = MM_CHARACTER_SELECT_ICON_RECT_H;

		RECTMENU_DrawOuterRect_HighLevel(&drawRect, outlineColor, 0, ot);
	}

	// loop through character icons
	for (int slot = 0;
		 slot < MM_CHARACTER_PAGE_SIZE;
		 slot++)
	{
		int characterID =
			s_characterPage * MM_CHARACTER_PAGE_SIZE +
			slot;

		if (characterID >= CharacterRegistry_GetCount())
			continue;

		struct CharacterSelectMeta *visualSlot =
			&D230.characterSelectMeta1P2P[slot];

		struct TransitionMeta *transition =
			&D230.characterSelectTransitionMeta[slot];

		struct Icon *icon = CharacterIconCache_Get(characterID);

		if (icon == NULL)
			continue;

		Color iconColor = D230.characterSelect_NeutralColor;

		if (((s16)slot == s_characterCursor) &&
			((sdata->characterSelectFlags & 1) != 0))
		{
			iconColor = D230.characterSelect_ChosenColor;
		}

		RECTMENU_DrawPolyGT4(
			icon,
			transition->currX +
				visualSlot->posX +
				MM_CHARACTER_SELECT_ICON_DECAL_OFFSET_X,
			transition->currY +
				visualSlot->posY +
				MM_CHARACTER_SELECT_ICON_DECAL_OFFSET_Y,
			&gGT->backBuffer->primMem,
			gGT->pushBuffer_UI.ptrOT,
			ColorCode_GetPacked(&iconColor),
			ColorCode_GetPacked(&iconColor),
			ColorCode_GetPacked(&iconColor),
			ColorCode_GetPacked(&iconColor),
			TRANS_50_DECAL,
			FP(1.0));
	}

	// reset
	struct CharacterSelectMeta *activeCharacterSelectMeta = D230.activeCharacterSelectMeta;

	for (s32 playerIndex = 0; playerIndex < gGT->numPlyrNextGame; playerIndex++)
	{
		s16 playerIcon = s_characterCursor;
		activeCharacterSelectMeta = &D230.activeCharacterSelectMeta[playerIcon];
		b32 playerSelected = (((int)(s16)sdata->characterSelectFlags >> playerIndex) & 1U) != 0;

		// if player has not selected a character
		if (!playerSelected)
		{
			Color animatedColor;
			u16 selectedPlayerFlag = (u16)(1 << playerIndex);
			MM_Characters_AnimateColors((u8 *)&animatedColor, playerIndex,

			                            // flags of which characters are selected
			                            (int)(s16)(sdata->characterSelectFlags & selectedPlayerFlag));

			animatedColor.r = (u8)((int)((u32)animatedColor.r << 2) / 5);
			animatedColor.g = (u8)((int)((u32)animatedColor.g << 2) / 5);
			animatedColor.b = (u8)((int)((u32)animatedColor.b << 2) / 5);

			struct TransitionMeta *selectedIconTransition = &D230.characterSelectTransitionMeta[playerIcon];

			drawRect.x = selectedIconTransition->currX + activeCharacterSelectMeta->posX + MM_CHARACTER_SELECT_HIGHLIGHT_OFFSET_X;
			drawRect.y = selectedIconTransition->currY + activeCharacterSelectMeta->posY + MM_CHARACTER_SELECT_HIGHLIGHT_OFFSET_Y;
			drawRect.w = MM_CHARACTER_SELECT_HIGHLIGHT_W;
			drawRect.h = MM_CHARACTER_SELECT_HIGHLIGHT_H;

			// this draws the flashing blue square that appears when you highlight a character in the character select screen
			CTR_Box_DrawSolidBox(&drawRect, animatedColor, ot);
		}
		if ((D230.characterSelectModelMoveTimer[playerIndex] == 0) &&
		    (D230.characterSelectPlayerState.currentCharacterID[playerIndex] == data.characterIDs[playerIndex]))
		{
			// get number of players
			u8 numPlyrNextGame = gGT->numPlyrNextGame;

			// if number of players is 1 or 2
			u32 fontType = FONT_CREDITS;

			// if number of players is 3 or 4
			if (numPlyrNextGame >= 3)
			{
				fontType = FONT_SMALL;
			}

			struct TransitionMeta *driverWindowTransition =
			    &D230.characterSelectTransitionMeta[playerIndex + MM_CHARACTER_SELECT_DRIVER_WINDOW_TRANSITION_FIRST];
			SVec2 *windowPos = &D230.activeCharacterSelectWindowPos[playerIndex];
			s16 nameBaseY = driverWindowTransition->currY + windowPos->y;
			s16 nameYOffset = (s16)((((u32)(numPlyrNextGame < 3) ^ 1) << 0x12) >> 0x10);
			s16 nameY;

			if ((numPlyrNextGame == 4) && (playerIndex > 1))
			{
				nameY = nameBaseY + nameYOffset + MM_CHARACTER_SELECT_4P_NAME_BOTTOM_OFFSET;
			}
			else
			{
				nameY = nameBaseY + D230.characterSelectNameTextY + nameYOffset;
			}

			// draw string
			int characterID =
				MM_Characters_GetHighlightedID();

			const struct CharacterDef *character =
				CharacterRegistry_GetByID(characterID);

			if (character != NULL)
			{
				DecalFont_DrawLine(
					(char *)character->displayName,
					(int)driverWindowTransition->currX +
						windowPos->x +
						(int)((u32)
							D230.characterSelectWindowWidth >> 1),
					(int)nameY,
					fontType,
					JUSTIFY_CENTER | ORANGE);
			}
		}

		// spin the character
		D230.characterSelectPlayerState.angle[playerIndex] += MM_CHARACTER_SELECT_SPIN_STEP;
	}

	// reset
	activeCharacterSelectMeta = D230.activeCharacterSelectMeta;

	// loop through all icons
	for (int slot = 0;
		 slot < MM_CHARACTER_PAGE_SIZE;
		 slot++)
	{
		int rosterIndex =
			s_characterPage * MM_CHARACTER_PAGE_SIZE +
			slot;

		const struct CharacterDef *character =
			CharacterRegistry_GetByRosterIndex(
				rosterIndex);

		if (character == NULL)
			continue;

		int characterID = character->id;

		struct CharacterSelectMeta *visualSlot =
			&D230.characterSelectMeta1P2P[slot];

		struct TransitionMeta *transition =
			&D230.characterSelectTransitionMeta[slot];

		struct Icon *icon =
			CharacterIconCache_Get(characterID);

		if (icon == NULL)
			continue;

		drawRect.x = transition->currX + visualSlot->posX;
		drawRect.y = transition->currY + visualSlot->posY;
		drawRect.w = MM_CHARACTER_SELECT_ICON_RECT_W;
		drawRect.h = MM_CHARACTER_SELECT_ICON_RECT_H;

		RECTMENU_DrawInnerRect(&drawRect, 0, ot);
	}

	SVec2 *windowPos = D230.activeCharacterSelectWindowPos;

	for (s32 playerIndex = 0; playerIndex < gGT->numPlyrNextGame; playerIndex++)
	{
		struct TransitionMeta *driverWindowTransition = &D230.characterSelectTransitionMeta[playerIndex + MM_CHARACTER_SELECT_DRIVER_WINDOW_TRANSITION_FIRST];
		b32 playerSelected = (((int)(s16)sdata->characterSelectFlags >> playerIndex) & 1U) != 0;
		Color animatedColor;

		// store window width and height in one 4-byte variable
		drawRect.x = driverWindowTransition->currX + windowPos->x;
		drawRect.y = driverWindowTransition->currY + windowPos->y;
		drawRect.w = D230.characterSelectWindowWidth;
		drawRect.h = D230.characterSelectWindowHeight;

		MM_Characters_AnimateColors((u8 *)&animatedColor, playerIndex,

		                            // flags of which characters are selected
		                            playerSelected ^ 1);

		RECTMENU_DrawOuterRect_HighLevel(&drawRect, animatedColor, 0, ot);

		// if player selected a character
		if (playerSelected)
		{
			RECT r58;
			r58.x = drawRect.x;
			r58.y = drawRect.y;
			r58.w = drawRect.w;
			r58.h = drawRect.h;

			for (s32 borderIndex = 0; borderIndex < MM_CHARACTER_SELECT_SELECTED_BORDER_COUNT; borderIndex++)
			{
				r58.x += MM_CHARACTER_SELECT_SELECTED_BORDER_INSET_X;
				r58.y += MM_CHARACTER_SELECT_SELECTED_BORDER_INSET_Y;
				r58.w -= MM_CHARACTER_SELECT_SELECTED_BORDER_SHRINK_W;
				r58.h -= MM_CHARACTER_SELECT_SELECTED_BORDER_SHRINK_H;

				animatedColor.r = (u8)((int)((u32)animatedColor.r << 2) / 5);
				animatedColor.g = (u8)((int)((u32)animatedColor.g << 2) / 5);
				animatedColor.b = (u8)((int)((u32)animatedColor.b << 2) / 5);

				RECTMENU_DrawOuterRect_HighLevel(&r58, animatedColor, 0, ot);
			}
		}
		windowPos++;

		// Draw 2D Menu rectangle background
		RECTMENU_DrawInnerRect(&drawRect, 9, &ot[3]);

		// not screen-space anymore,
		// this is viewport-space
		drawRect.x = 0;
		drawRect.y = 0;

		RECTMENU_DrawRwdBlueRect(&drawRect, &D230.characterSelect_BlueRectColors[0], &gGT->pushBuffer[playerIndex].ptrOT[0x3ff], &gGT->backBuffer->primMem);
	}

	if (D230.characterSelectMenuState != EXITING_MENU)
	{
		MM_Characters_DrawPageHint();
	}

	return;
}

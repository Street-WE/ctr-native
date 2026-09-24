#include <common.h>
#include <VehExhaust.h>

enum
{
	TURBO_FIRE_SIZE_MIN = 4,
	TURBO_FIRE_SIZE_MAX = 8,
	TURBO_FIRE_MATRIX_SCALE_SHIFT = 3,
	TURBO_COOLDOWN_SIGN_SCALE = 0x10000,
	TURBO_ALPHA_RUMBLE_THRESHOLD = 2500,
	TURBO_RUMBLE_FRAMES = 4,
	TURBO_RUMBLE_FORCE = 4,
	TURBO_SECONDARY_MODEL_FRAME_OFFSET = 3,
	TURBO_ANIM_FRAME_COUNT = 8,
	TURBO_ANIM_FRAME_MASK = 7,
	TURBO_AUDIO_SLOT = 3,
	TURBO_AUDIO_VOLUME_BASE = 0x100,
	TURBO_AUDIO_ALPHA_SHIFT = 4,
	TURBO_AUDIO_VOLUME_MAX = 0x82,
	TURBO_AUDIO_DISTORT_STEP = 0x10,
	TURBO_AUDIO_DISTORT_MAX = 0x80,
	TURBO_AUDIO_DISTORT_INCREMENT_LIMIT = 0xc0,
	TURBO_AUDIO_SFX_ID = 0xe,
	TURBO_RESERVES_DISAPPEAR_THRESHOLD = 0x10,
	TURBO_ALPHA_FULL_MINUS_ONE = 0xfff,
	TURBO_FADE_FAST_STEP = 0x100,
	TURBO_FADE_SLOW_STEP = 0x40,
	TURBO_STOP_SFX_ID = -1,
};


void VehTurbo_ProcessBucket(struct Thread *turboThread)
{
	while (turboThread != NULL)
	{
		struct Turbo *turbo = turboThread->object;
		for (int outlet = 0; outlet < VEH_EXHAUST_MAX_OUTLETS; outlet++)
		{
			struct Instance *flame = VehExhaust_GetFlame(turboThread, outlet);
			if (flame == NULL) continue;
			struct InstDrawPerPlayer *draw = INST_GETIDPP(flame);
			struct InstDrawPerPlayer *driver = INST_GETIDPP(turbo->driver->instSelf);
			for (int i = 0; i < sdata->gGT->numPlyrCurrGame; i++, draw++, driver++)
			{
				if ((driver->instFlags & PUSHBUFFER_EXISTS) == 0)
				{
					draw->instFlags &= driver->instFlags | ~DRAW_SUCCESSFUL;
					draw->otRangeNormal = driver->otRangeNormal;
					draw->otRangeSecondary = driver->otRangeSecondary;
					draw->depthOffset[0] = driver->depthOffset[0];
					draw->depthOffset[1] = driver->depthOffset[1];
				}
			}
		}
		turboThread = turboThread->siblingThread;
	}
}

void VehTurbo_ThDestroy(struct Thread *t)
{
	struct Turbo *turbo = t->object;
	turbo->driver->actionsFlagSet &= ~ACTION_TURBO_ITEM;
	for (int i = VEH_EXHAUST_MAX_OUTLETS - 1; i >= 0; i--)
	{
		struct Instance *flame = VehExhaust_GetFlame(t, i);
		if (flame != NULL) INSTANCE_Death(flame);
	}
}

static void VehTurbo_PositionFlame(struct Instance *kart, struct Instance *flame,
    const struct ExhaustOutlet *outlet, int fireSize)
{
	MATRIX rotation;
	SVECTOR offset;
	VehExhaust_GetRotation(outlet, kart, &rotation);
	int scale = VehExhaust_GetScale(outlet->flameScale);
	for (int row = 0; row < 3; row++)
	{
		for (int col = 0; col < 3; col++)
		{
			int component = rotation.m[row][col];
			if (col == 0 && outlet->mirrorFlame) component = -component;
			flame->matrix.m[row][col] = (s16)(((component * fireSize >> TURBO_FIRE_MATRIX_SCALE_SHIFT) * scale) / 256);
		}
	}
	VehExhaust_GetLocalPosition(outlet, kart, true, &offset);
	gte_SetRotMatrix(&kart->matrix);
	gte_SetTransMatrix(&kart->matrix);
	CTR_GteLoadSV0(&offset);
	gte_rt();
	CTR_GteStoreIR(&flame->matrix.t[0]);
}

static void VehTurbo_UpdateFlames(struct Thread *turboThread, const struct ExhaustTemplate *layout)
{
	struct Turbo *turbo = turboThread->object;
	struct Instance *instanceDriver = turbo->driver->instSelf;
	int outletCount = VehExhaust_GetCount(layout);
	int fireSize = turbo->fireSize;
	if (fireSize > TURBO_FIRE_SIZE_MAX) fireSize = TURBO_FIRE_SIZE_MAX;
	if (fireSize < TURBO_FIRE_SIZE_MIN) fireSize = TURBO_FIRE_SIZE_MIN;

	for (int i = 0; i < VEH_EXHAUST_MAX_OUTLETS; i++)
	{
		struct Instance *flame = VehExhaust_GetFlame(turboThread, i);
		if (flame == NULL) continue;
		flame->flags = (flame->flags & ~(SPLIT_LINE | REFLECTIVE)) |
		    (instanceDriver->flags & (SPLIT_LINE | REFLECTIVE));
		if (instanceDriver->flags & (SPLIT_LINE | REFLECTIVE))
			flame->vertSplit = instanceDriver->vertSplit;
		if (i < outletCount)
			VehTurbo_PositionFlame(instanceDriver, flame, &layout->outlets[i], fireSize);
		if (turbo->fireVisibilityCooldown == 0 && i < outletCount &&
		    VehExhaust_GetScale(layout->outlets[i].flameScale) != 0)
			flame->flags &= ~HIDE_MODEL;
		else
			flame->flags |= HIDE_MODEL;
		int frame = (turbo->fireAnimIndex + i * TURBO_SECONDARY_MODEL_FRAME_OFFSET) & TURBO_ANIM_FRAME_MASK;
		flame->model = sdata->gGT->modelPtr[frame + STATIC_TURBO_EFFECT];
	}

}

void VehTurbo_ThTick(struct Thread *turboThread)
{
	struct GameTracker *gGT = sdata->gGT;

	struct Turbo *turbo = (struct Turbo *)turboThread->object;
	struct Driver *driver = turbo->driver;
	struct Instance *instance = turboThread->inst;
	struct Instance *instanceDriver = driver->instSelf;

	if ((
	        // if not burnt
	        (driver->burnTimer == 0) &&

	        // if alpha of turbo is zero
	        (instance->alphaScale == 0)) &&


	    (instanceDriver->thread->modelIndex != DYNAMIC_GHOST))
	{
		// cut driverInst transparency in half
		instanceDriver->alphaScale = instanceDriver->alphaScale >> 1;
	}


	// decrease turbo visibility cooldown by elapsed milliseconds per frame, ~32
	s16 elapsedTime = turbo->fireVisibilityCooldown - gGT->elapsedTimeMS;
	turbo->fireVisibilityCooldown = elapsedTime;

	// don't allow negatives
	if (elapsedTime * TURBO_COOLDOWN_SIGN_SCALE < 0)
	{
		turbo->fireVisibilityCooldown = 0;
	}

	VehTurbo_UpdateFlames(turboThread,
	    VehExhaust_GetTemplate(data.characterIDs[driver->driverID]));

	if (instance->alphaScale < TURBO_ALPHA_RUMBLE_THRESHOLD)
	{
		// gamepad vibration
		GAMEPAD_ShockFreq(driver, TURBO_RUMBLE_FRAMES, TURBO_RUMBLE_FORCE);
	}


	turbo->fireAnimIndex++;

	// if eight or higher, back to zero
	if (turbo->fireAnimIndex >= TURBO_ANIM_FRAME_COUNT)
	{
		turbo->fireAnimIndex = 0;
	}

	if (turbo->fireDisappearCountdown > 0)
	{
		turbo->fireDisappearCountdown--;
	}

	// player of any kind
	if (instanceDriver->thread->modelIndex == DYNAMIC_PLAYER)
	{
		int fireSfxVolume = TURBO_AUDIO_VOLUME_BASE - (u32)(instance->alphaScale >> TURBO_AUDIO_ALPHA_SHIFT);

		if (fireSfxVolume < 0)
		{
			fireSfxVolume = 0;
		}
		else
		{
			if (TURBO_AUDIO_VOLUME_MAX < fireSfxVolume)
			{
				fireSfxVolume = TURBO_AUDIO_VOLUME_MAX;
			}
		}

		u32 fireAudioDistort = (u32)turbo->fireAudioDistort + TURBO_AUDIO_DISTORT_STEP;

		if ((int)fireAudioDistort < 0)
		{
			fireAudioDistort = 0;
		}
		else
		{
			if (fireAudioDistort > TURBO_AUDIO_DISTORT_MAX)
			{
				fireAudioDistort = TURBO_AUDIO_DISTORT_MAX;
			}
		}

		// if echo is required
		u32 echo = ((driver->actionsFlagSet & ACTION_ENGINE_ECHO) != 0);

		// driver audio
		OtherFX_RecycleNew(&driver->driverAudioPtrs[TURBO_AUDIO_SLOT], TURBO_AUDIO_SFX_ID,
		                   HowlSfx_Pack(HOWL_SFX_LR_CENTER, fireAudioDistort, fireSfxVolume, echo));

		// manipulate turbo audio distort to change sound each frame
		if (turbo->fireAudioDistort < TURBO_AUDIO_DISTORT_INCREMENT_LIMIT)
		{
			turbo->fireAudioDistort++;
		}
	}

	u8 kartState = driver->kartState;

	if (
	    // if this is a ghost
	    (instanceDriver->thread->modelIndex == DYNAMIC_GHOST) ||

	    ((kartState != KS_MASK_GRABBED) &&
	     (kartState != KS_CRASHING)

	     // lol they found a glitch with this
	     && (kartState != KS_WARP_PAD)))
	{
		// if reserves are nearing zero
		if ((driver->reserves < TURBO_RESERVES_DISAPPEAR_THRESHOLD) || (turbo->fireDisappearCountdown == 0))
		{
			// if fully transparent, skip lines
			if (TURBO_ALPHA_FULL_MINUS_ONE < instance->alphaScale)
			{
				goto LAB_80069b50;
			}

			if (turbo->fireDisappearCountdown == 0)
			{
				// increase transparency
				instance->alphaScale += TURBO_FADE_FAST_STEP;
			}
			else
			{
				// increase transparency
				instance->alphaScale += TURBO_FADE_SLOW_STEP;
			}
		}
		else
		{
			// if scale is big, skip lines
			if (TURBO_ALPHA_FULL_MINUS_ONE < instance->alphaScale)
			{
				goto LAB_80069b50;
			}
		}
	}

	// if not a ghost, and
	// kart state is mask grab, crashed, or warped
	else
	{
		// restore backup of alpha
		instanceDriver->alphaScale = driver->alphaScaleBackup;
	LAB_80069b50:

		// player of any kind
		if (instanceDriver->thread->modelIndex == DYNAMIC_PLAYER)
		{
			// volume, distortion, left/right
			u32 stopSfxParams = HOWL_SFX_CENTER_NO_DISTORTION;

			// if echo is required
			if ((driver->actionsFlagSet & ACTION_ENGINE_ECHO) != 0)
			{
				// add echo, volume, distortion, left/right
				stopSfxParams = HOWL_SFX_CENTER_NO_DISTORTION | HOWL_SFX_ECHO_FLAG;
			}

			// driver audio
			OtherFX_RecycleNew(&driver->driverAudioPtrs[TURBO_AUDIO_SLOT], TURBO_STOP_SFX_ID, stopSfxParams);
		}

		// 0x800 = this thread needs to be deleted
		turboThread->flags |= THREAD_FLAG_DEAD;
	}

	// All outlets follow the primary instance's shared fade state.
	for (int i = 1; i < VEH_EXHAUST_MAX_OUTLETS; i++)
	{
		struct Instance *flame = VehExhaust_GetFlame(turboThread, i);
		if (flame != NULL) flame->alphaScale = instance->alphaScale;
	}

	// do not use infinite loop optimization,
	// modern GCC "without" the $RA skip is more
	// optimized than PSYQ "with" the $RA skip
}

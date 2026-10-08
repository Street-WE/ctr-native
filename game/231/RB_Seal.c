#include <common.h>
#include <math.h>
#ifdef CTR_NATIVE
#include <platform/native_audio.h>
#endif

enum
{
	SEAL_TRAIL_SAMPLE_INTERVAL = 2,
	SEAL_TRAIL_ALPHA_START = 0x200,
	SEAL_TRAIL_ALPHA_STEP = 0x180,
	SEAL_TRAIL_FADE_STEP = 0x20,
	SEAL_TRAIL_SCALE_STEP = 0x80,
};

static void RB_SealTrail_CopyPose(struct Instance *dst, const struct Instance *src)
{
	dst->model = src->model;
	dst->matrix = src->matrix;
	dst->animFrame = src->animFrame;
	dst->animIndex = src->animIndex;
	dst->flags = DRAW_INSTANCE | GHOST_DRAW_TRANSPARENT | (src->flags & HIDE_MODEL);
}

static void RB_SealTrail_ThDestroy(struct Thread *t)
{
	struct SealTrail *trail = t->object;
	for (int i = 0; i < SEAL_TRAIL_GHOST_COUNT; i++)
	{
		if (trail->ghosts[i] != NULL)
		{
			INSTANCE_Death(trail->ghosts[i]);
			trail->ghosts[i] = NULL;
		}
	}
}

void RB_SealTrail_ThTick(struct Thread *t)
{
	struct SealTrail *trail = t->object;
	struct Instance *seal = t->parentThread != NULL ? t->parentThread->inst : NULL;
	if (seal == NULL)
	{
		t->flags |= THREAD_FLAG_DEAD;
		return;
	}

	// Children tick after their parent, including both movement and turning.
	// Copy oldest first so each ghost retains the preceding sampled pose.
	if ((trail->sampleTicks++ % SEAL_TRAIL_SAMPLE_INTERVAL) == 0)
	{
		for (int i = SEAL_TRAIL_GHOST_COUNT - 1; i >= 0; i--)
		{
			struct Instance *ghost = trail->ghosts[i];
			const struct Instance *src = i == 0 ? seal : trail->ghosts[i - 1];
			if (ghost != NULL && src != NULL)
			{
				RB_SealTrail_CopyPose(ghost, src);
				ghost->alphaScale = SEAL_TRAIL_ALPHA_START + i * SEAL_TRAIL_ALPHA_STEP;
			}
		}
	}
	else
	{
		for (int i = 0; i < SEAL_TRAIL_GHOST_COUNT; i++)
		{
			if (trail->ghosts[i] != NULL)
			{
				trail->ghosts[i]->alphaScale += SEAL_TRAIL_FADE_STEP;
			}
		}
	}

	for (int i = 0; i < SEAL_TRAIL_GHOST_COUNT; i++)
	{
		struct Instance *ghost = trail->ghosts[i];
		if (ghost == NULL)
		{
			continue;
		}
		ghost->scale.x = seal->scale.x + (i + 1) * SEAL_TRAIL_SCALE_STEP;
		ghost->scale.y = seal->scale.y + (i + 1) * SEAL_TRAIL_SCALE_STEP;
		ghost->scale.z = seal->scale.z + (i + 1) * SEAL_TRAIL_SCALE_STEP;
		if ((seal->flags & HIDE_MODEL) != 0)
		{
			ghost->flags |= HIDE_MODEL;
		}
	}
}

static void RB_SealTrail_Init(struct Thread *sealThread)
{
	static const char ghostName[16] = "sealghost";
	struct Thread *t = PROC_BirthWithObject(
	    SIZE_RELATIVE_POOL_BUCKET(sizeof(struct SealTrail), CHILD_SIBLING, SMALL, STATIC),
	    RB_SealTrail_ThTick, "sealtrail", sealThread);
	if (t == NULL)
	{
		return;
	}
	t->flags |= THREAD_FLAG_DISABLE_COLLISION;
	t->funcThDestroy = RB_SealTrail_ThDestroy;
	struct SealTrail *trail = t->object;
	memset(trail, 0, sizeof(*trail));
	for (int i = 0; i < SEAL_TRAIL_GHOST_COUNT; i++)
	{
		struct Instance *ghost = INSTANCE_Birth3D(sealThread->inst->model, ghostName, t);
		if (ghost == NULL)
		{
			break; // A full instance pool must not prevent the hazard from working.
		}
		trail->ghosts[i] = ghost;
		RB_SealTrail_CopyPose(ghost, sealThread->inst);
		ghost->flags |= HIDE_MODEL;
		ghost->scale = sealThread->inst->scale;
	}
}

// one seal can not collide with more than one other thread,
// then quits, it was like that in the original game too,
// but one seal likely-wont collide with two threads at the same time
void Seal_CheckColl(struct Instance *sealInst, struct Thread *sealTh, int damage, int radius, int sound)
{
	struct GameTracker *gGT;
	struct Instance *hitInst;
	struct Driver *hitDriver;
	b32 boolHurt;
	u8 kartStatePrev;

	gGT = sdata->gGT;

	// check players
	hitInst = (struct Instance *)LinkedCollide_Radius(sealInst, sealTh, gGT->threadBuckets[PLAYER].thread, radius);

	// if hit a player
	if (hitInst != 0)
	{
		// get driver from instance
		hitDriver = (struct Driver *)hitInst->thread->object;

		// backup
		kartStatePrev = hitDriver->kartState;

		// attempt to harm driver (spin out)
		boolHurt = RB_Hazard_HurtDriver(hitDriver, damage, 0, 0);

		// if failed, due to mask grab or mask weapon
		if (boolHurt == 0)
		{
			return;
		}

		// if driver was already spinning out
		if (kartStatePrev == KS_SPINNING)
		{
			return;
		}

		if (sound == 0)
		{
			return;
		}

		// play seal sound, with echo if driver is on an echo quadblock
		OtherFX_Play_Echo(sound, 1, hitDriver->actionsFlagSet & ACTION_ENGINE_ECHO);

		// dont check other buckets
		return;
	}

	// check robots
	hitInst = (struct Instance *)LinkedCollide_Radius(sealInst, sealTh, gGT->threadBuckets[ROBOT].thread, radius);

	// if hit a robot
	if (hitInst != 0)
	{
		// get driver from instance
		hitDriver = (struct Driver *)hitInst->thread->object;

		// attempt to harm driver (spin out)
		RB_Hazard_HurtDriver(hitDriver, damage, 0, 0);

		// dont check other buckets
		return;
	}

	// check mines
	hitInst = (struct Instance *)LinkedCollide_Radius(sealInst, sealTh, gGT->threadBuckets[MINE].thread, radius);

	// if hit a mine
	if (hitInst != 0)
	{
		// all mine ThCollide functions only take one parameter,
		// all other ThCollide functions are erased due to redundancy
		((ThreadSimpleCollideFunc)hitInst->thread->funcThCollide)(hitInst->thread);

		// dont check other bucket
		return;
	}
}

int RB_Seal_ThCollide(struct Thread *sealThread, struct Thread *driverTh, void *funcThCollide, struct ScratchpadStruct *sps)
{
	(void)sealThread;
	(void)driverTh;
	(void)funcThCollide;

	return sps->Input1.modelID == DYNAMIC_PLAYER;
}

static struct SpawnType2 *RB_Seal_GetPath(const struct Seal *seal)
{
	struct Level *level = sdata->gGT->level1;
	if (level->ptrSpawnType2 == NULL || seal->sealID < 0 || seal->sealID >= level->numSpawnType2)
	{
		return NULL;
	}
	struct SpawnType2 *path = &level->ptrSpawnType2[seal->sealID];
	return path->numCoords > 0 && path->coords.positions != NULL ? path : NULL;
}

static int RB_Seal_SetNextSegment(struct Seal *seal)
{
	struct SpawnType2 *path = RB_Seal_GetPath(seal);
	seal->segmentFrames = 0;
	seal->distFromSpawn = 0;
	if (path == NULL || path->numCoords < 2 || path->numCoords > 32767) return 0;
	seal->spawnPos = path->coords.positions[seal->direction];
	for (int skipped = 0; skipped < path->numCoords; skipped++)
	{
		seal->direction = (seal->direction + 1) % path->numCoords;
		seal->endPos = path->coords.positions[seal->direction];
		int dx = (int)seal->endPos.x - seal->spawnPos.x;
		int dy = (int)seal->endPos.y - seal->spawnPos.y;
		int dz = (int)seal->endPos.z - seal->spawnPos.z;
		if (dx == 0 && dy == 0 && dz == 0) continue;
		int length = (int)sqrt((double)dx * dx + (double)dy * dy + (double)dz * dz);
		// Preserve the old first-leg speed (45 updates), then use it for every leg.
		if (seal->speedQ8 == 0) seal->speedQ8 = (length * 256 + 44) / 45;
		seal->segmentFrames = (length * 256 + seal->speedQ8 - 1) / seal->speedQ8;
		if (seal->segmentFrames < 1) seal->segmentFrames = 1;
		if (seal->segmentFrames > 32767) seal->segmentFrames = 32767;
		if (dx != 0 || dz != 0) seal->turnAroundRot.y = ratan2(-dx, -dz) & 0xfff;
		seal->turnAroundRot.x = seal->rotCurr.x;
		seal->turnAroundRot.z = seal->rotCurr.z;
		return 1;
	}
	return 0;
}

static void RB_Seal_Animate(struct Instance *inst)
{
	int frames = INSTANCE_GetNumAnimFrames(inst, 0);
	inst->animFrame = inst->animFrame + 2 < frames ? inst->animFrame + 2 : 0;
}

static void RB_Seal_ThDestroy(struct Thread *t)
{
#ifdef CTR_NATIVE
	NativeAudio_StopSealLoop(t->inst);
#endif
	if (t->inst != NULL) t->inst->thread = NULL;
}

void RB_Seal_ThTick_TurnAround(struct Thread *t)
{
	struct Instance *inst = t->inst;
	struct Seal *seal = t->object;
	RB_Seal_Animate(inst);
	seal->rotCurr.y = RB_Hazard_InterpolateValue(seal->rotCurr.y, seal->turnAroundRot.y, 0x80);
	ConvertRotToMatrix(&inst->matrix, &seal->rotCurr);
	seal->numFramesSpinning++;
	if (seal->rotCurr.y == seal->turnAroundRot.y)
	{
		seal->numFramesSpinning = 0;
		seal->rotDesired = seal->rotCurr;
		ThTick_Set(t, RB_Seal_ThTick_Move);
	}
	if (!PlaySealAmbientSound(inst) && inst->animFrame == 0) PlaySound3D(0x77, inst);
	Seal_CheckColl(inst, t, 1, 0x4000, 0x78);
}

void RB_Seal_ThTick_Move(struct Thread *t)
{
	struct Instance *inst = t->inst;
	struct Seal *seal = t->object;
	RB_Seal_Animate(inst);
	if (seal->segmentFrames > 0)
	{
		seal->distFromSpawn++;
		for (int axis = 0; axis < 3; axis++)
		{
			int start = CTR_VECTOR_DATA(&seal->spawnPos)[axis];
			int delta = (int)CTR_VECTOR_DATA(&seal->endPos)[axis] - start;
			inst->matrix.t[axis] = start + (s32)((s64)delta * seal->distFromSpawn / seal->segmentFrames);
		}
		if (seal->distFromSpawn >= seal->segmentFrames && RB_Seal_SetNextSegment(seal))
		{
			ThTick_Set(t, RB_Seal_ThTick_TurnAround);
		}
	}
	PlaySealAmbientSound(inst);
	Seal_CheckColl(inst, t, 1, 0x4000, 0x78);
}

void RB_Seal_LInB(struct Instance *inst)
{
	if (inst->thread != NULL) return;
	struct Thread *t = PROC_BirthWithObject(
	    SIZE_RELATIVE_POOL_BUCKET(sizeof(struct Seal), NONE, SMALL, STATIC),
	    RB_Seal_ThTick_Move, "seal", NULL);
	if (t == NULL) return;
	inst->thread = t;
	t->inst = inst;
	t->funcThCollide = (void *)RB_Seal_ThCollide;
	t->funcThDestroy = RB_Seal_ThDestroy;
	inst->scale = (SVec3){0x2000, 0x2000, 0x2000};
	struct Seal *seal = t->object;
	memset(seal, 0, sizeof(*seal));
	// Support multi-digit path indices while validating the spawn table.
	int nameLength = (int)strlen(inst->name);
	int suffix = nameLength;
	while (suffix > 0 && inst->name[suffix - 1] >= '0' && inst->name[suffix - 1] <= '9') suffix--;
	int pathIndex = suffix == nameLength ? -1 : atoi(inst->name + suffix);
	seal->sealID = pathIndex >= 0 && pathIndex < sdata->gGT->level1->numSpawnType2 ? pathIndex : -1;
	if (inst->instDef != NULL) seal->rotCurr = inst->instDef->rot;
	seal->rotCurr.y &= 0xfff;
	seal->rotDesired = seal->rotCurr;
	seal->turnAroundRot = seal->rotCurr;
	struct SpawnType2 *path = RB_Seal_GetPath(seal);
	if (path != NULL)
	{
		CTR_COPY_VEC3(inst->matrix.t, CTR_VECTOR_DATA(&path->coords.positions[0]));
		if (RB_Seal_SetNextSegment(seal)) ThTick_Set(t, RB_Seal_ThTick_TurnAround);
	}
	ConvertRotToMatrix(&inst->matrix, &seal->rotCurr);
	RB_SealTrail_Init(t);
	PlaySealAmbientSound(inst);
}

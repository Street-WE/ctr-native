#include <common.h>


int RB_Fireball_ThCollide(struct Thread *fireballThread, struct Thread *driverTh, void *funcThCollide, struct ScratchpadStruct *sps)
{
	(void)fireballThread;
	(void)driverTh;
	(void)funcThCollide;

	return sps->Input1.modelID == DYNAMIC_PLAYER;
}

struct ParticleEmitter emSet_Fireball[10] = {[0] =
                                                 {
                                                     .flags = 1,

                                                     // invalid axis, assume FuncInit
                                                     .initOffset = 0xC,

                                                     .InitTypes.FuncInit =
                                                         {
                                                             .particle_funcPtr = 0,
                                                             .particle_colorFlags = 0x4A1,
                                                             .particle_lifespan = 0x400,
                                                             .particle_Type = 0,
                                                         }

                                                     // last 0x10 bytes are blank
                                                 },

                                             [1] =
                                                 {
                                                     .flags = 1,

                                                     // posX
                                                     .initOffset = 0,

                                                     .InitTypes.AxisInit.baseValue.startVal = 1,

                                                     // The rest is blank

                                                     // last 0x10 are blank
                                                 },

                                             [2] =
                                                 {
                                                     .flags = 1,

                                                     // posZ
                                                     .initOffset = 2,

                                                     .InitTypes.AxisInit.baseValue.startVal = 1,

                                                     // The rest is blank

                                                     // last 0x10 are blank
                                                 },

                                             [3] = {.flags = 3,

                                                    // posY
                                                    .initOffset = 1,

                                                    .InitTypes.AxisInit = {.baseValue =
                                                                               {
                                                                                   .startVal = 1,
                                                                                   .velocity = 1,
                                                                                   .accel = 0,
                                                                               }}},

                                             [4] = {.flags = 9,

                                                    // rotY
                                                    .initOffset = 4,

                                                    .InitTypes.AxisInit = {.baseValue = {.startVal = 1, .velocity = 0, .accel = 0},

                                                                           .rngSeed = {.startVal = 0x1000, .velocity = 0, .accel = 0}}},

                                             [5] = {.flags = 3,

                                                    // scale
                                                    .initOffset = 5,

                                                    .InitTypes.AxisInit =
                                                        {
                                                            .baseValue = {.startVal = 0xA00, .velocity = -0xB0, .accel = 0},
                                                        }},

                                             [6] = {.flags = 0xB,

                                                    // colorR
                                                    .initOffset = 7,

                                                    .InitTypes.AxisInit = {.baseValue = {.startVal = 0xFF00, .velocity = 0xC000, .accel = 0},

                                                                           .rngSeed =
                                                                               {
                                                                                   .startVal = 0x5F00,
                                                                                   .velocity = 0,
                                                                                   .accel = 0,
                                                                               }}},

                                             [7] = {.flags = 3,

                                                    // colorG
                                                    .initOffset = 8,

                                                    .InitTypes.AxisInit = {.baseValue = {.startVal = 0x8000, .velocity = 0xE000, .accel = 0},

                                                                           .rngSeed =
                                                                               {
                                                                                   .startVal = 0,
                                                                                   .velocity = 0,
                                                                                   .accel = 0,
                                                                               }}},

                                             [8] = {.flags = 3,

                                                    // colorB
                                                    .initOffset = 9,

                                                    .InitTypes.AxisInit = {.baseValue = {.startVal = 0x4000, .velocity = 0xF000, .accel = 0}}},

                                             // null terminator
                                             [9] = {0}};

enum
{
	FIREBALL_HOP_MS = 1280,
	FIREBALL_HOP_HEIGHT = 2000,
	FIREBALL_WAIT_MS = 2000,
};

static struct SpawnType2 *RB_Fireball_GetPath(struct Fireball *fire)
{
	struct Level *level = sdata->gGT->level1;
	if (level == NULL || level->ptrSpawnType2 == NULL || fire->pathIndex < 0 ||
	    fire->pathIndex >= level->numSpawnType2) return NULL;
	struct SpawnType2 *path = &level->ptrSpawnType2[fire->pathIndex];
	return path->numCoords > 0 && path->coords.positions != NULL ? path : NULL;
}

static int RB_Fireball_MovePath(struct Instance *inst, struct Fireball *fire, struct SpawnType2 *path, int elapsed)
{
	if (elapsed <= 0) return 0;
	if (fire->cooldown > 0)
	{
		if (elapsed < fire->cooldown)
		{
			fire->cooldown -= elapsed;
			return 0;
		}
		elapsed -= fire->cooldown;
		fire->cooldown = 0;
	}
	if (elapsed == 0) return 0;
	if (fire->hopTime == 0) PlaySound3D(0x81, inst);
	fire->hopTime += elapsed;
	if (fire->hopTime > FIREBALL_HOP_MS) fire->hopTime = FIREBALL_HOP_MS;
	int next = (fire->pathNode + 1) % path->numCoords;
	SVec3 start = path->coords.positions[fire->pathNode];
	SVec3 end = path->coords.positions[next];
	int time = fire->hopTime;
	for (int axis = 0; axis < 3; axis++)
	{
		int from = CTR_VECTOR_DATA(&start)[axis];
		int delta = (int)CTR_VECTOR_DATA(&end)[axis] - from;
		inst->matrix.t[axis] = from + (s32)((s64)delta * time / FIREBALL_HOP_MS);
	}
	// Join two parabolas at their apex, exactly halfway along the leg.
	// This also preserves the midpoint peak for nodes at different heights.
	int apex = (start.y > end.y ? start.y : end.y) + FIREBALL_HOP_HEIGHT;
	int half = FIREBALL_HOP_MS / 2;
	int offset = time - half;
	int base = time <= half ? start.y : end.y;
	inst->matrix.t[1] = apex - (s32)((s64)(apex - base) * offset * offset / (half * half));
	fire->velY = (s16)(-(s64)2 * (apex - base) * offset * 32 / (half * half));
	if (time == FIREBALL_HOP_MS)
	{
		CTR_COPY_VEC3(inst->matrix.t, CTR_VECTOR_DATA(&end));
		fire->pathNode = next;
		fire->hopTime = 0;
		fire->cooldown = FIREBALL_WAIT_MS;
	}
	return 1;
}

void RB_Fireball_ThTick(struct Thread *t)
{
	struct Instance *fireInst;
	struct Fireball *fireObj;
	struct Particle *particle;
	int velY;
	int oldVelY;
	int resetPosY;

	struct GameTracker *gGT;
	int elapsedTimeMS;

	gGT = sdata->gGT;
	elapsedTimeMS = gGT->elapsedTimeMS;

	fireInst = t->inst;
	fireObj = t->object;

	struct SpawnType2 *path = RB_Fireball_GetPath(fireObj);
	if (path == NULL && fireObj->cooldown != 0)
	{
		fireObj->cooldown -= elapsedTimeMS;

		if (fireObj->cooldown < 0)
		{
			fireObj->cooldown = 0;
		}

		return;
	}

	fireInst->flags &= ~HIDE_MODEL;

	oldVelY = 0;
	resetPosY = path == NULL ? fireInst->instDef->pos.y - 0x440 : 0;
	int moving = path != NULL ? RB_Fireball_MovePath(fireInst, fireObj, path, elapsedTimeMS) : 0;
	// Hide immediately on landing and throughout the pause between hops.
	if (path != NULL && (fireObj->cooldown > 0 || !moving))
	{
		fireInst->flags |= HIDE_MODEL;
	}

	// if fireball isn't below the lava,
	// handle all particle spawning
	if (path != NULL ? moving : fireInst->matrix.t[1] >= resetPosY)
	{
		velY = fireObj->velY;
		if (path == NULL)
		{
			// move based on velocity
			velY = fireObj->velY;
			oldVelY = velY;
			fireInst->matrix.t[1] += (velY * elapsedTimeMS) >> 5;

			// reduce velocity (gravity)
			velY -= ((elapsedTimeMS * 10) >> 5);

			// terminal velocity
			if (velY < -200)
			{
				velY = -200;
			}

			// set new velY
			fireObj->velY = velY;

		}

		// fire particles
		particle = Particle_Init(0, gGT->iconGroup[0xA], &emSet_Fireball[0]);

		if (particle != 0)
		{
			// adjust positions,
			// dont bitshift, must multiply, or negatives break
			particle->axis[0].startVal += fireInst->matrix.t[0] * 0x100;
			particle->axis[1].startVal += fireInst->matrix.t[1] * 0x100;
			particle->axis[2].startVal += fireInst->matrix.t[2] * 0x100;

			particle->renderDepthLimit = 0x1e00;

			// reuse "velY" variable for particles
			velY *= -0x180;

			// range check
			if (velY < -0x7fff)
			{
				velY = -0x7fff;
			}
			if (velY > 0x7fff)
			{
				velY = 0x7fff;
			}
			velY = (s16)velY;

			particle->axis[1].velocity = (int)velY;
		}

		Seal_CheckColl(fireInst, t, 4, 0x10000, 0);
	}

	// === rest of movement behavior ===

	if (path == NULL) fireObj->cycleTimer -= elapsedTimeMS;

	// if animation is not over
	if ((fireInst->animFrame + 1) < INSTANCE_GetNumAnimFrames(fireInst, 0))
	{
		// increment frame
		fireInst->animFrame = fireInst->animFrame + 1;
	}

	// if animation ended
	else
	{
		// reset
		fireInst->animFrame = 0;
	}

	if (path != NULL) return;

	if ((oldVelY >= 0) && (fireObj->velY < 0))
	{
		fireObj->direction = 1;
	}

	// if cycle is over
	if (fireObj->cycleTimer < 1)
	{
		// == first frame of fireball rising ==

		// reset timer
		fireObj->cycleTimer = 0xb40;

		// upward velocity
		fireObj->velY = 200;
		fireObj->direction = 0;

		// reset position under lava
		fireInst->matrix.t[1] = resetPosY;

		// reset animation
		fireInst->animFrame = 0;

		// fwooooossssssssshhhh
		PlaySound3D(0x81, fireInst);
	}
}

void RB_Fireball_LInB(struct Instance *inst)
{
	struct Fireball *fireObj;
	struct Thread *t;
	int fireballID;

	if (inst->thread != 0)
	{
		return;
	}

	t = PROC_BirthWithObject(
	    // creation flags
	    SIZE_RELATIVE_POOL_BUCKET(sizeof(struct Fireball), NONE, SMALL, STATIC),

	    RB_Fireball_ThTick, // behavior
	    "fireball",         // debug name
	    0                   // thread relative
	);

	if (t == 0)
	{
		return;
	}
	inst->thread = t;
	t->inst = inst;
	t->funcThCollide = (void *)RB_Fireball_ThCollide;

	//inst->scale.x = 0x4000;
	//inst->scale.y = 0x4000;
	//inst->scale.z = 0x4000;

	inst->animFrame = 0;
	inst->animIndex = 0;

	fireObj = ((struct Fireball *)t->object);
	memset(fireObj, 0, sizeof(*fireObj));
	fireObj->pathIndex = -1;
	fireObj->cycleTimer = 0;
	fireObj->cooldown = 0;
	fireObj->rot_unused.x = 0;
	fireObj->velY = 96;
	fireObj->direction = 0;

	// Match seals: numeric suffix selects a zero-based instance path.
	int length = (int)strlen(inst->name);
	int suffix = length;
	while (suffix > 0 && inst->name[suffix - 1] >= '0' && inst->name[suffix - 1] <= '9') suffix--;
	fireballID = suffix == length ? -1 : atoi(inst->name + suffix);
	fireObj->fireballID = fireballID;
	fireObj->pathIndex = fireballID;
	struct SpawnType2 *path = RB_Fireball_GetPath(fireObj);
	if (path != NULL)
	{
		CTR_COPY_VEC3(inst->matrix.t, CTR_VECTOR_DATA(&path->coords.positions[0]));
		inst->flags &= ~HIDE_MODEL;
	}
	else if (fireballID >= 0 && (fireballID & 1) != 0)
	{
		fireObj->cooldown = 1440;
	}
}

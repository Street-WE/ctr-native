#include <common.h>

static char s_teeth[] = "teeth";

enum
{
	TEETH_ACCESS_ANIMATED = 1,
	TEETH_ACCESS_PERMANENT = 4,
	TEETH_FADE_DURATION_MS = 1000,
	TEETH_FULL_TRANSPARENCY = 0x1000,
	TEETH_PERMANENT_OPEN = -1,
};

static b32 RB_Teeth_IsPermanent(const struct Instance *inst)
{
	// The editor's instance name selects the variant without a new model ID.
	return inst->instDef != NULL && strncmp(inst->instDef->name, "break_", 6) == 0;
}

u32 RB_Teeth_GetAccessFlag(const struct Instance *inst)
{
	return RB_Teeth_IsPermanent(inst) ? TEETH_ACCESS_PERMANENT : TEETH_ACCESS_ANIMATED;
}

b32 RB_Teeth_IsPermanentOpen(const struct Instance *inst)
{
	return RB_Teeth_IsPermanent(inst) && (sdata->doorAccessFlags & TEETH_ACCESS_PERMANENT) != 0;
}

// Use a private collision workspace: weapon explosions also call this while
// the shared scratchpad contains the caller's collision search/callback state.
b32 RB_Teeth_CheckWeaponSweep(const SVec3 *from, const SVec3 *to, s16 radius)
{
	struct Level *level = sdata->gGT->level1;
	if (level == NULL || level->ptr_mesh_info == NULL || level->ptrInstDefs == NULL ||
	    (sdata->gGT->gameMode1 & RELIC_RACE) != 0)
	{
		return false;
	}
	struct mesh_info *mesh = level->ptr_mesh_info;
	struct ScratchpadStruct search = {0};
	search.ptr_mesh_info = mesh;
	search.Input1.pos = *to;
	search.Union.QuadBlockColl.pos = *from;
	search.Input1.hitRadius = radius;
	search.Input1.hitRadiusSquared = radius * radius;
	search.Union.QuadBlockColl.quadFlagsWanted = QUADBLOCK_FLAG_DOOR;
	search.Union.QuadBlockColl.searchFlags = COLL_SEARCH_HIGH_LOD;
	search.hitFraction = COLL_FRACTION_ONE;
	for (int axis = 0; axis < 3; axis++)
	{
		int a = CTR_VECTOR_DATA(from)[axis];
		int b = CTR_VECTOR_DATA(to)[axis];
		int lo = (a < b ? a : b) - radius;
		int hi = (a > b ? a : b) + radius;
		CTR_VECTOR_DATA(&search.bbox.min)[axis] = lo < -32768 ? -32768 : lo;
		CTR_VECTOR_DATA(&search.bbox.max)[axis] = hi > 32767 ? 32767 : hi;
	}
	u32 hitAccess = 0;
	for (int i = 0; i < mesh->numQuadBlock; i++)
	{
		const struct QuadBlock *quad = &mesh->ptrQuadBlockArray[i];
		u32 access = quad->terrain_type;
		if ((quad->quadFlags & QUADBLOCK_FLAG_DOOR) == 0 ||
		    (quad->quadFlags & (QUADBLOCK_FLAG_TRIGGER | QUADBLOCK_FLAG_NO_COLLISION_RESPONSE)) != 0 ||
		    (access != TEETH_ACCESS_ANIMATED && access != TEETH_ACCESS_PERMANENT) ||
		    (sdata->doorAccessFlags & access) != 0)
		{
			continue;
		}
		// Detect attacks from either side without changing the exported wall.
		struct QuadBlock barrier = *quad;
		barrier.draw_order_low |= QUADBLOCK_DRAW_ORDER_LOW_DOUBLE_SIDED;
		int previousFraction = search.hitFraction;
		COLL_MOVED_QUADBLK_TestTriangles(&barrier, &search);
		if (search.hitFraction < previousFraction)
		{
			hitAccess = access;
		}
	}
	if (hitAccess == 0)
	{
		return false;
	}
	// The access mask links barrier geometry to its door type. Pick the nearest
	// matching instance so the original and permanent variants stay independent.
	struct Instance *door = NULL;
	s64 nearestDistance = 0;
	for (u32 i = 0; i < level->numInstances; i++)
	{
		struct InstDef *def = &level->ptrInstDefs[i];
		struct Instance *inst = def->ptrInstance;
		if (def->modelID != STATIC_TEETH || inst == NULL || RB_Teeth_GetAccessFlag(inst) != hitAccess)
		{
			continue;
		}
		s64 dx = inst->matrix.t[0] - (s32)search.Union.QuadBlockColl.hitPos.x;
		s64 dy = inst->matrix.t[1] - (s32)search.Union.QuadBlockColl.hitPos.y;
		s64 dz = inst->matrix.t[2] - (s32)search.Union.QuadBlockColl.hitPos.z;
		s64 distance = dx * dx + dy * dy + dz * dz;
		if (door == NULL || distance < nearestDistance)
		{
			door = inst;
			nearestDistance = distance;
		}
	}
	if (door == NULL)
	{
		return false;
	}
	RB_Teeth_OpenDoor(door);
	return (sdata->doorAccessFlags & hitAccess) != 0;
}

void RB_Teeth_LInB(struct Instance *inst)
{
	inst->depthBiasNormal += 2;

	// If in relic race
	if ((sdata->gGT->gameMode1 & RELIC_RACE) != 0)
	{
		// enable access through a door (disable collision)
		sdata->doorAccessFlags |= RB_Teeth_GetAccessFlag(inst);

		// Make invisible
		inst->flags |= HIDE_MODEL;
	}
	return;
}

void RB_Teeth_BSP_Callback(struct ScratchpadStruct *sps, void *hitObject)
{
	struct Thread *weaponThread = hitObject;
	s16 model;
	struct Thread *teethTh;
	struct Instance *weaponInst;
	struct Instance *teethInst;

	model = weaponThread->modelIndex;

	b32 canOpenTeeth = (model == DYNAMIC_PLAYER) || (model == PU_EXPLOSIVE_CRATE) || (model == DYNAMIC_POISON) || (model == STATIC_CRATE_TNT);
	if (!canOpenTeeth)
	{
		return;
	}

	teethTh = sps->Union.ThBuckColl.thread;

	weaponInst = weaponThread->inst;

	teethInst = teethTh->inst;

	if ((weaponInst != NULL) && (teethInst != NULL))
	{
		int doorSideDistance = ((int)sps->Input1.pos.x - weaponInst->matrix.t[0]) * (int)teethInst->matrix.m[0][2] +
		                       ((int)sps->Input1.pos.z - weaponInst->matrix.t[2]) * (int)teethInst->matrix.m[2][2];

		// catch negative value
		if (doorSideDistance < 0)
		{
			doorSideDistance = -doorSideDistance;
		}

		if (0x100 < doorSideDistance >> 0xc)
		{
			return;
		}
	}

	if (RB_Teeth_IsPermanent(teethInst))
	{
		RB_Teeth_OpenDoor(teethInst);
	}
	else
	{
		((struct Teeth *)teethTh->object)->direction = TEETH_DIRECTION_OPENING;
	}

	return;
}

void RB_Teeth_ThTick(struct Thread *t)
{
	u32 flags;
	struct Teeth *teeth;
	struct Instance *inst;
	struct GameTracker *gGT;

	gGT = sdata->gGT;
	teeth = t->object;
	inst = t->inst;
	struct ScratchpadStruct *sps = CTR_SCRATCHPAD_PTR(struct ScratchpadStruct, 0x108);

	if (RB_Teeth_IsPermanent(inst))
	{
		if (teeth->direction == TEETH_DIRECTION_OPENING)
		{
			// alphaScale is transparency: zero is opaque, 0x1000 invisible.
			int elapsed = gGT->elapsedTimeMS;
			if (elapsed > 0)
			{
				teeth->timeOpen += elapsed;
			}
			if (teeth->timeOpen >= TEETH_FADE_DURATION_MS)
			{
				inst->alphaScale = TEETH_FULL_TRANSPARENCY;
				inst->flags |= HIDE_MODEL;
				teeth->direction = TEETH_DIRECTION_IDLE;
				teeth->timeOpen = TEETH_PERMANENT_OPEN;
			}
			else
			{
				inst->alphaScale = teeth->timeOpen * TEETH_FULL_TRANSPARENCY / TEETH_FADE_DURATION_MS;
			}
		}
		return;
	}

	// if door is not moving
	if (teeth->direction == TEETH_DIRECTION_IDLE)
	{
		// if timer is zero
		if (teeth->timeOpen == 0)
		{
			goto LAB_800b9ff8;
		}

		// reduce timer by milliseconds
		int remainingOpenTime = teeth->timeOpen - gGT->elapsedTimeMS;

		// set new timer
		teeth->timeOpen = remainingOpenTime;

		// if timer is up
		if (remainingOpenTime < 1)
		{
			// play sound
			// teeth closing
			PlaySound3D(0x75, inst);

			// timer is zero
			teeth->timeOpen = 0;

			// door is closing
			teeth->direction = TEETH_DIRECTION_CLOSING;

			goto LAB_800b9fe8;
		}
	}

	// if door is moving
	else
	{
		// modify animation index by direction
		inst->animFrame = inst->animFrame + teeth->direction;

		int numAnimFrames = VehFrameInst_GetNumAnimFrames(inst, 0);

		// if animation is not on last frame
		if ((int)inst->animFrame < numAnimFrames)
		{
			// if animation when backwards past beginning
			if ((int)inst->animFrame < 0)
			{
				// set animation to beginning
				inst->animFrame = 0;

				// door is not moving
				teeth->direction = TEETH_DIRECTION_IDLE;

				// timer is zero
				teeth->timeOpen = 0;

				// remove access (enable collision)
				sdata->doorAccessFlags &= 0xfffffffe;
			}
		}

		// if animation is on last frame (or past)
		else
		{
			// set animation to last frame
			inst->animFrame = (s16)numAnimFrames + -1;

			// door is not moving (fully open)
			teeth->direction = TEETH_DIRECTION_IDLE;

			// timer, 2 seconds
			teeth->timeOpen = 0x780;
		}
	LAB_800b9fe8:
		if (teeth->timeOpen == 0)
		{
		LAB_800b9ff8:
			if (TEETH_DIRECTION_CLOSING < teeth->direction)
			{
				goto LAB_800ba084;
			}
		}
	}

	// Teeth instance position
	sps->Input1.pos.x = inst->matrix.t[0];
	sps->Input1.pos.y = inst->matrix.t[1];
	sps->Input1.pos.z = inst->matrix.t[2];

	sps->Input1.hitRadius = 0x300;
	sps->Input1.hitRadiusSquared = 0x90000;

	sps->Input1.modelID = STATIC_TEETH;

	sps->Union.ThBuckColl.thread = t;
	sps->Union.ThBuckColl.funcCallback = RB_Teeth_BSP_Callback;

	// If door wants to close, but Player or Mine
	// is in the way, then do not force the doors to close

	PROC_CollideHitboxWithBucket(gGT->threadBuckets[PLAYER].thread, sps, 0);

	PROC_CollideHitboxWithBucket(gGT->threadBuckets[MINE].thread, sps, 0);

LAB_800ba084:

	// if no timer exists
	// (opening, closing, or full closed)
	if (teeth->timeOpen == 0)
	{
		flags = inst->flags & ~HIDE_MODEL;
	}

	// if a timer is active
	// (fully open)
	else
	{
		flags = inst->flags | HIDE_MODEL;
	}

	inst->flags = flags;

	return;
}

int RB_Teeth_LInC(struct Instance *teethInst, struct Thread *t, struct ScratchpadStruct *sps)
{
	struct Thread *teethTh;
	struct Teeth *teeth;
	struct Driver *d;

	// This is the door you can shoot in tiger temple

	// If in relic race, ignore the function,
	// there are no weapons to activate door anyways
	if ((sdata->gGT->gameMode1 & RELIC_RACE) != 0)
	{
		return 2;
	}

	teethTh = teethInst->thread;
	d = t->object;

	if (teethTh == NULL)
	{
		// 0x8 = size
		// 0 = no relation to param4
		// 0x300 = SmallStackPool
		// 0x3 = "static" thread bucket
		teethTh = PROC_BirthWithObject(SIZE_RELATIVE_POOL_BUCKET(sizeof(struct Teeth), NONE, SMALL, STATIC), RB_Teeth_ThTick, s_teeth, NULL);

		teethInst->thread = teethTh;

		if (teethTh == NULL)
		{
			return 2;
		}

		teeth = teethTh->object;

		teethTh->inst = teethInst;

		// door not moving
		teeth->direction = TEETH_DIRECTION_IDLE;

		// timer is zero
		teeth->timeOpen = 0;
	}

	teeth = teethTh->object;

	if (RB_Teeth_IsPermanent(teethInst) &&
	    (teeth->direction == TEETH_DIRECTION_OPENING || teeth->timeOpen == TEETH_PERMANENT_OPEN))
	{
		return 2;
	}

	// if collided object is a player
	if (sps->Input1.modelID == DYNAMIC_PLAYER)
	{
		// if driver is using mask weapon
		if ((d->actionsFlagSet & ACTION_MASK_WEAPON) != 0)
		{
			RB_Teeth_OpenDoor(teethInst);
		}

		return 2;
	}

	// If collide with something
	// that is not a player

	// time to close
	if (teeth->timeOpen == 0)
	{
		int doorSideDistance = ((int)sps->Input1.pos.x - teethInst->matrix.t[0]) * (int)teethInst->matrix.m[0][2] +
		                       ((int)sps->Input1.pos.z - teethInst->matrix.t[2]) * (int)teethInst->matrix.m[2][2];

		if (doorSideDistance < 0)
		{
			doorSideDistance = -doorSideDistance;
		}
		if (doorSideDistance >> 0xc < 0x81)
		{
			return 1;
		}
		return 2;
	}
	return 2;
}

void RB_Teeth_OpenDoor(struct Instance *inst)
{
	struct Thread *teethTh = inst->thread;
	if (teethTh == NULL)
	{
		teethTh = PROC_BirthWithObject(SIZE_RELATIVE_POOL_BUCKET(sizeof(struct Teeth), NONE, SMALL, STATIC), RB_Teeth_ThTick, s_teeth, NULL);
		inst->thread = teethTh;
		if (teethTh == NULL)
		{
			return;
		}
		teethTh->inst = inst;
		((struct Teeth *)teethTh->object)->timeOpen = 0;
		((struct Teeth *)teethTh->object)->direction = TEETH_DIRECTION_IDLE;
	}
	if (RB_Teeth_IsPermanent(inst))
	{
		struct Teeth *teeth = teethTh->object;
		if (teeth->direction == TEETH_DIRECTION_OPENING || teeth->timeOpen == TEETH_PERMANENT_OPEN)
		{
			return;
		}
		teeth->timeOpen = 0;
		inst->alphaScale = 0;
		inst->flags |= GHOST_DRAW_TRANSPARENT;
	}
	PlaySound3D(0x75, inst); // play sound, teeth opening
	((struct Teeth *)teethTh->object)->direction = TEETH_DIRECTION_OPENING;
	sdata->doorAccessFlags |= RB_Teeth_GetAccessFlag(inst);
}

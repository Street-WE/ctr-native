#include <common.h>

enum
{
	SWINGING_PERIOD_MS = 4000,
	SWINGING_ANGLE = 0x1000 / 6, // 60 degrees, in the game's angle units
	SWINGING_DRIVER_RADIUS = 0x60,
};

struct SwingingHazard
{
	int timeMS;
	int min[3];
	int max[3];
	u32 touchingDrivers;
};

// Static LEV models store sequential XYZ byte triples; render Y and Z are
// swapped. Cached-vertex and colour commands do not consume a new triple.
static b32 RB_Swinging_ReadBounds(struct Instance *inst, struct SwingingHazard *hazard)
{
	if (inst->model == NULL || inst->model->numHeaders <= 0)
		return 0;
	struct ModelHeader *header = inst->model->headers;
	if (header == NULL || header->ptrFrameData == NULL || header->ptrCommandList == 0 || header->unk3 != 0 || header->ptrAnimations != NULL)
		return 0;
	struct ModelFrame *frame = header->ptrFrameData;
	const u8 *vertices = (const u8 *)frame + frame->vertexOffset;
	const u32 *commands = (const u32 *)header->ptrCommandList + 1;
	int count = 0;
	for (; *commands != 0xffffffff; commands++)
	{
		if ((*commands >> 16) == 0 || (*commands & 0x04000000) != 0)
			continue;
		int point[3] = {
		    (s16)((u16)(frame->pos.x + vertices[0]) * 4) / 4,
		    (s16)((u16)(frame->pos.y + vertices[2]) * 4) / 4,
		    (s16)((u16)(frame->pos.z + vertices[1]) * 4) / 4,
		};
		for (int axis = 0; axis < 3; axis++)
		{
			// The renderer's vertex << 2 cancels its near translation << 2.
			s64 scaled = (s64)point[axis] * CTR_VECTOR_DATA(&header->scale)[axis] * CTR_VECTOR_DATA(&inst->scale)[axis];
			if ((inst->flags & DRAW_HUGE) != 0) scaled *= 4;
			int value = (int)(scaled / ((s64)0x1000 * 0x1000));
			if (count == 0 || value < hazard->min[axis]) hazard->min[axis] = value;
			if (count == 0 || value > hazard->max[axis]) hazard->max[axis] = value;
		}
		vertices += 3;
		count++;
	}
	return count != 0;
}

static void RB_Swinging_SetRotation(struct Instance *inst, int timeMS)
{
	MATRIX base;
	MATRIX swing;
	SVec3 rot = {0};
	ConvertRotToMatrix(&base, &inst->instDef->rot);
	rot.z = (s16)((MATH_Sin((u32)((s64)timeMS * 0x1000 / SWINGING_PERIOD_MS) + 0x400) * SWINGING_ANGLE) / 0x1000);
	ConvertRotToMatrix(&swing, &rot);
	// Compose in model space so the authored pivot and orientation are retained.
	for (int row = 0; row < 3; row++)
		for (int col = 0; col < 3; col++)
		{
			int value = 0;
			for (int k = 0; k < 3; k++) value += base.m[row][k] * swing.m[k][col];
			inst->matrix.m[row][col] = (s16)(value / 0x1000);
		}
}

static b32 RB_Swinging_Touches(struct Instance *inst, struct SwingingHazard *hazard, struct Driver *driver)
{
	int delta[3];
	for (int axis = 0; axis < 3; axis++)
		delta[axis] = driver->instSelf->matrix.t[axis] - inst->matrix.t[axis];
	delta[1] += SWINGING_DRIVER_RADIUS / 2;
	s64 distanceSquared = 0;
	for (int axis = 0; axis < 3; axis++)
	{
		// Inverse of the pure rotation is its transpose. Bounds already include scale.
		s64 value = 0;
		for (int k = 0; k < 3; k++) value += (s64)inst->matrix.m[k][axis] * delta[k];
		value /= 0x1000;
		s64 gap = 0;
		if (value < hazard->min[axis]) gap = hazard->min[axis] - value;
		if (value > hazard->max[axis]) gap = value - hazard->max[axis];
		distanceSquared += gap * gap;
	}
	return distanceSquared <= SWINGING_DRIVER_RADIUS * SWINGING_DRIVER_RADIUS;
}

void RB_Swinging_ThTick(struct Thread *thread)
{
	struct Instance *inst = thread->inst;
	struct SwingingHazard *hazard = (struct SwingingHazard *)thread->object;
	int elapsed = sdata->gGT->elapsedTimeMS;
	if (elapsed <= 0) return;
	// Sample the swing between frames as well, catching a fast-moving head.
	int steps = elapsed / 8 + 1;
	if (steps > 32) steps = 32;
	u32 contacts = 0;
	for (int step = 0; step <= steps; step++)
	{
		int time = (int)((hazard->timeMS + (s64)elapsed * step / steps) % SWINGING_PERIOD_MS);
		RB_Swinging_SetRotation(inst, time);
		for (int i = 0; i < 8; i++)
		{
			struct Driver *driver = sdata->gGT->drivers[i];
			if (driver == NULL || driver->instSelf == NULL) continue;
			if (RB_Swinging_Touches(inst, hazard, driver))
			{
				u32 bit = 1u << i;
				if (((hazard->touchingDrivers | contacts) & bit) == 0)
					RB_Hazard_HurtDriver(driver, 3, NULL, 0);
				contacts |= bit;
			}
		}
	}

	hazard->timeMS = (int)((hazard->timeMS + (s64)elapsed) % SWINGING_PERIOD_MS);
	hazard->touchingDrivers = contacts;
}

b32 RB_Swinging_LInB(struct Instance *inst)
{
	if (inst == NULL || inst->instDef == NULL)
		return 0;
	// Exporters may keep the object name (e.g. Hammer) on the instance and
	// place the authored hazard name (SwingingHammer) on its shared model.
	if (strncmp(inst->instDef->name, "Swinging", 8) != 0 &&
	    (inst->model == NULL || strncmp(inst->model->name, "Swinging", 8) != 0))
		return 0;
	if (inst->thread != NULL) return 1;
	struct SwingingHazard bounds = {0};
	if (!RB_Swinging_ReadBounds(inst, &bounds)) return 0;
	struct Thread *thread = PROC_BirthWithObject(
	    SIZE_RELATIVE_POOL_BUCKET(sizeof(struct SwingingHazard), NONE, SMALL, STATIC),
	    RB_Swinging_ThTick, "swinging", 0);
	if (thread == NULL) return 0;
	inst->thread = thread;
	thread->inst = inst;
	*(struct SwingingHazard *)thread->object = bounds;
	RB_Swinging_SetRotation(inst, 0);
	return 1;
}

#include <VehExhaust.h>
#include <CharacterRegistry.h>

static const struct ExhaustTemplate sExhaustTemplates[] =
{
    [DEFAULT_EXHAUST] = {
        .count = 2,
        .outlets = {
            { .position = {18, 56, -56}, .smokeScale = 256, .flameScale = 256 },
            { .position = {-18, 56, -56}, .smokeScale = 256, .flameScale = 256, .mirrorFlame = true },
        },
    },
    [PAIR_LOW] = {
        .count = 2,
        .outlets = {
            { .position = {18, 34, -56}, .smokeScale = 256, .flameScale = 256 },
            { .position = {-18, 34, -56}, .smokeScale = 256, .flameScale = 256, .mirrorFlame = true },
        },
    },
    [SINGLE_BASIL] = {
        .count = 2,
        .outlets = {
            { .position = {0, 56, -80}, .smokeScale = 400, .flameScale = 400 },
            { .position = {0, 56, -80}, .smokeScale = 400, .flameScale = 400, .mirrorFlame = true },
        },
    },
    [HIGH_EXHAUST] = {
        .count = 2,
        .outlets = {
            { .position = {26, 48, -56}, .smokeScale = 256, .flameScale = 256 },
            { .position = {-26, 48, -56}, .smokeScale = 256, .flameScale = 256, .mirrorFlame = true },
        },
    },
    [V_HIGH_EXHAUST] = {
        .count = 2,
        .outlets = {
            { .position = {30, 64, -60}, .smokeScale = 256, .flameScale = 256 },
            { .position = {-30, 64, -60}, .smokeScale = 256, .flameScale = 256, .mirrorFlame = true },
        },
    },
    [RED_BEARD] = {
        .count = 2,
        .outlets = {
            { .position = {20, 50, -50}, .smokeScale = 256, .flameScale = 256 },
            { .position = {-20, 50, -50}, .smokeScale = 256, .flameScale = 256, .mirrorFlame = true },
        },
    },
    [ROBO_RACER] = {
        .count = 2,
        .outlets = {
            { .position = {0, 50, -50}, .smokeScale = 256, .flameScale = 256 },
            { .position = {0, 50, -50}, .smokeScale = 256, .flameScale = 256, .mirrorFlame = true },
        },
    },
    [GYPSY_MOTH] = {
        .count = 12,
        .outlets = {
            { .position = {6, 60, -72}, .smokeScale = 128, .flameScale = 128 },
            { .position = {-6, 60, -72}, .smokeScale = 128, .flameScale = 128, .mirrorFlame = true },
            { .position = {6, 72, -72}, .smokeScale = 128, .flameScale = 128 },
            { .position = {-6, 72, -72}, .smokeScale = 128, .flameScale = 128, .mirrorFlame = true },
            { .position = {6, 84, -72}, .smokeScale = 128, .flameScale = 128 },
            { .position = {-6, 84, -72}, .smokeScale = 128, .flameScale = 128, .mirrorFlame = true },
            { .position = {6, 96, -72}, .smokeScale = 128, .flameScale = 128 },
            { .position = {-6, 96, -72}, .smokeScale = 128, .flameScale = 128, .mirrorFlame = true },
            { .position = {18, 72, -72}, .smokeScale = 128, .flameScale = 128 },
            { .position = {-18, 72, -72}, .smokeScale = 128, .flameScale = 128, .mirrorFlame = true },
            { .position = {18, 84, -72}, .smokeScale = 128, .flameScale = 128 },
            { .position = {-18, 84, -72}, .smokeScale = 128, .flameScale = 128, .mirrorFlame = true },
        },
    },
};

const struct ExhaustTemplate *VehExhaust_GetTemplate(int characterID)
{
    int type = CharacterRegistry_GetDriverExhaustType(characterID);
    if ((unsigned int)type >= len(sExhaustTemplates))
        type = DEFAULT_EXHAUST;
    return &sExhaustTemplates[type];
}

int VehExhaust_GetCount(const struct ExhaustTemplate *layout)
{
    if (layout->count < 0) return 0;
    if (layout->count > VEH_EXHAUST_MAX_OUTLETS) return VEH_EXHAUST_MAX_OUTLETS;
    return layout->count;
}

int VehExhaust_GetScale(int scale)
{
    if (scale < 0) return 0;
    if (scale > 1024) return 1024;
    return scale;
}

static s16 VehExhaust_ClampShort(int value)
{
    if (value < -32768) return -32768;
    if (value > 32767) return 32767;
    return (s16)value;
}

void VehExhaust_GetLocalPosition(const struct ExhaustOutlet *outlet,
    const struct Instance *kart, b32 flame, SVECTOR *position)
{
    VECTOR correction = {0, 0, 0, 0};
    int shift = flame ? 12 : 4;
    if (flame)
    {
        // The flame model's origin differs from the smoke attachment point.
        // Rotate this artwork correction with the outlet, not its position.
        correction.vy = -8;
        correction.vz = 4;
        if (outlet->rotation.x || outlet->rotation.y || outlet->rotation.z)
        {
            MATRIX rotation;
            SVECTOR offset = {0, -8, 4, 0};
            ConvertRotToMatrix(&rotation, &outlet->rotation);
            gte_SetRotMatrix(&rotation);
            CTR_GteLoadSV0(&offset);
            gte_rtv0();
            CTR_GteStoreMAC(&correction.vx);
        }
    }
    position->vx = VehExhaust_ClampShort((kart->scale.x * (outlet->position.x + correction.vx)) >> shift);
    position->vy = VehExhaust_ClampShort((kart->scale.y * (outlet->position.y + correction.vy)) >> shift);
    position->vz = VehExhaust_ClampShort((kart->scale.z * (outlet->position.z + correction.vz)) >> shift);
    position->pad = 0;
}

void VehExhaust_GetRotation(const struct ExhaustOutlet *outlet,
    struct Instance *kart, MATRIX *rotation)
{
    *rotation = kart->matrix;
    if (outlet->rotation.x || outlet->rotation.y || outlet->rotation.z)
    {
        MATRIX local;
        ConvertRotToMatrix(&local, &outlet->rotation);
        MulMatrix0(&kart->matrix, &local, rotation);
    }
}

void VehExhaust_ScaleSmoke(struct Particle *particle, int scale)
{
    scale = VehExhaust_GetScale(scale);
    for (int i = PARTICLE_AXIS_SCALE_X_OR_LINE_SCALE; i <= PARTICLE_AXIS_SCALE_Y_OR_LINE_PREV_Y; i++)
    {
        struct ParticleAxis *axis = &particle->axis[i];
        axis->startVal = (int)(((s64)axis->startVal * scale) / 256);
        axis->velocity = VehExhaust_ClampShort((axis->velocity * scale) / 256);
        axis->accel = VehExhaust_ClampShort((axis->accel * scale) / 256);
    }
}

struct Instance *VehExhaust_GetFlame(struct Thread *thread, int index)
{
    struct Turbo *turbo = thread->object;
    if (index == 0) return thread->inst;
    if (index == 1) return turbo->inst;
    if (index >= 2 && index < VEH_EXHAUST_MAX_OUTLETS) return turbo->extraInst[index - 2];
    return NULL;
}

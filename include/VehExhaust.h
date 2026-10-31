#ifndef VEH_EXHAUST_H
#define VEH_EXHAUST_H

#include <common.h>

// Positions are model units at kart scale 4096. Angles use 4096 per turn.
// Effect scales use 256 = original size; zero disables that effect.
struct ExhaustOutlet
{
    SVec3 position;
    SVec3 rotation;
    int smokeScale;
    int flameScale;
    b32 mirrorFlame;
};

struct ExhaustTemplate
{
    int count;
    struct ExhaustOutlet outlets[VEH_EXHAUST_MAX_OUTLETS];
};

const struct ExhaustTemplate *VehExhaust_GetTemplate(int characterID);
int VehExhaust_GetCount(const struct ExhaustTemplate *layout);
int VehExhaust_GetScale(int scale);
void VehExhaust_GetLocalPosition(const struct ExhaustOutlet *outlet,
    const struct Instance *kart, b32 flame, SVECTOR *position);
void VehExhaust_GetRotation(const struct ExhaustOutlet *outlet,
    struct Instance *kart, MATRIX *rotation);
void VehExhaust_ScaleSmoke(struct Particle *particle, int scale);
struct Instance *VehExhaust_GetFlame(struct Thread *thread, int index);

#endif

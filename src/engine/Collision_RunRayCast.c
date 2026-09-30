/* Casts a ray against the world's collision models; returns the shared hit record with the nearest
 * distance, or NULL. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct CollCastParams {
    VecFx32 *origin;
    VecFx32 *direction;
    s32 radius;
    u16 directionIsUnit;
    u16 flags;
    void *exclude;
} CollCastParams;

typedef struct CollCastState {
    s32 mode00;
    void *modelDataA04;
    void *modelDataB08;
    u8 pad0c[4];
    VecFx32 modelStart10;
    VecFx32 direction1c;
    VecFx32 origin28;
    s32 minX34;
    s32 minZ38;
    s32 maxX3c;
    s32 maxZ40;
    VecFx32 unitDirection44;
    s32 directionLength50;
    VecFx32 modelEnd54;
    u8 pad60[0x18];
    s32 nearestHit78;
    u8 pad7c[0x10];
} CollCastState;

typedef struct CollisionWorld {
    void *unknown00;
    void **modelList;
} CollisionWorld;

typedef struct CollisionHitRecord {
    void *model;
    void *face;
    s32 unknown08;
    s32 distance;
} CollisionHitRecord;

extern void CollCast_InitRayState(CollCastState *state, CollCastParams *params);
extern int CollCast_TestModelRay(CollCastState *state, void *modelList);
extern CollisionHitRecord data_027e0764;

CollisionHitRecord *Collision_RunRayCast(CollisionWorld *world, CollCastParams *params)
{
    CollCastState state;

    CollCast_InitRayState(&state, params);
    if (CollCast_TestModelRay(&state, *world->modelList) != 0) {
        data_027e0764.distance = state.nearestHit78;
        return &data_027e0764;
    }
    return 0;
}

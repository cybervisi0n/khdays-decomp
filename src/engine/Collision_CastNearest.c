/* Runs a collision cast against the world's models; returns the shared hit record with the nearest
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
    u8 pad00[0x78];
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

extern void CollCast_Init(CollCastState *state, CollCastParams *params);
extern int CollCast_TestModel(CollCastState *state, void *modelList);
extern CollisionHitRecord data_027e0764;

CollisionHitRecord *Collision_CastNearest(CollisionWorld *world, CollCastParams *params)
{
    CollCastState state;

    CollCast_Init(&state, params);
    if (CollCast_TestModel(&state, *world->modelList) != 0) {
        data_027e0764.distance = state.nearestHit78;
        return &data_027e0764;
    }
    return 0;
}

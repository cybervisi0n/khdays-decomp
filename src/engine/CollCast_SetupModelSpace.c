/* Moves the cast segment into the model's space and computes its XZ bounds. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

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
} CollCastState;

typedef struct CollisionModelBlob {
    u8 pad00[0x64];
    VecFx32 castOffset64;
} CollisionModelBlob;

extern void VEC_Add(const VecFx32 *, const VecFx32 *, VecFx32 *);

void CollCast_SetupModelSpace(CollCastState *state, CollisionModelBlob *model)
{
    VEC_Add(&state->origin28, &model->castOffset64, &state->modelStart10);
    VEC_Add(&state->modelStart10, &state->direction1c, &state->modelEnd54);

    if (state->modelStart10.x < state->modelEnd54.x) {
        state->minX34 = state->modelStart10.x;
        state->maxX3c = state->modelEnd54.x;
    } else {
        state->minX34 = state->modelEnd54.x;
        state->maxX3c = state->modelStart10.x;
    }

    if (state->modelStart10.z < state->modelEnd54.z) {
        state->minZ38 = state->modelStart10.z;
        state->maxZ40 = state->modelEnd54.z;
    } else {
        state->minZ38 = state->modelEnd54.z;
        state->maxZ40 = state->modelStart10.z;
    }
}

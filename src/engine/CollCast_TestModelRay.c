/* Tests a ray cast against one collision model: moves the cast into model space, rejects it when it
 * misses the model's area, and walks the model's quad tree (vertical casts use the vertical
 * walker); returns whether it hit. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct CollisionRegion { s32 centerX00; s32 centerZ04; s32 size08; } CollisionRegion;

typedef struct CollCastState {
    s32 mode00;
    void *modelDataA04;
    void *modelDataB08;
    u8 pad0c[0x1c - 0x0c];
    VecFx32 direction1c;
    u8 pad28[0x34 - 0x28];
    s32 bound34;
    s32 bound38;
    s32 bound3c;
    s32 bound40;
} CollCastState;

typedef struct CollisionTraversalFrame {
    s16 childIndex00;
    u16 nodeFlags02;
    CollisionRegion region04;
} CollisionTraversalFrame;

typedef struct CollisionModel {
    u8 pad00[0x84];
    CollisionRegion region84;
    u8 pad90[0x9c - 0x90];
    u16 *root9c;
    void *modelDataA0;
} CollisionModel;

typedef struct CollisionHitRecord {
    CollisionModel *model00;
    void *face04;
    u32 unknown08;
    s32 distance0c;
} CollisionHitRecord;

extern CollisionTraversalFrame data_027e06e4;
extern CollisionTraversalFrame *data_027e06e0;
extern CollisionHitRecord data_027e0764;
extern void CollCast_SetupModelSpace(CollCastState *state, CollisionModel *model);
extern void *Coll_WalkVerticalTreeForHit(u16 *root, CollCastState *state);
extern void *Coll_WalkTreeForHit(u16 *root, CollCastState *state);

int CollCast_TestModelRay(CollCastState *state, CollisionModel *model)
{
    int hit = 0;
    s32 size;
    int i;
    void *face;

    state->modelDataA04 = model->modelDataA0;
    CollCast_SetupModelSpace(state, model);
    size = model->region84.size08;

    if (state->bound34 - model->region84.centerX00 <= size &&
        state->bound38 - model->region84.centerZ04 <= size &&
        model->region84.centerX00 - state->bound3c <= size &&
        model->region84.centerZ04 - state->bound40 <= size) {
        CollisionTraversalFrame *frames = &data_027e06e4;
        s32 levelSize = size;

        for (i = 0; i < 8; i++) {
            frames[i].region04.size08 = levelSize;
            levelSize /= 2;
        }

        data_027e06e0 = frames;
        frames[0].region04 = model->region84;

        if ((state->direction1c.x | state->direction1c.z) == 0) {
            face = Coll_WalkVerticalTreeForHit(model->root9c, state);
        } else {
            face = Coll_WalkTreeForHit(model->root9c, state);
        }
        if (face != 0) {
            hit = 1;
            data_027e0764.model00 = model;
            data_027e0764.face04 = face;
        }
    }

    return hit;
}

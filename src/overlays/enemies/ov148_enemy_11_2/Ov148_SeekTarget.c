/* Charging attack step: clears bit 7 of the high flag byte, then hits whatever it overlaps (sphere
 * contacts or, in the alternate mode, a hit query with a hit command) and ends with an effect and
 * update 0x53; otherwise advances along its movement, ending when a ray cast hits the world. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/actor.h"
#include "game/ai_task.h"

struct AuxData {
    char pad00[0x1c4];
    u8 flags1c4;
    char pad1c5[0xcb];
    u16 field290;
};

struct Obj {
    Actor base;                  /* 0x000 */
    u8 pad38c[0x4];
    struct AuxData *aux390;
};

struct State {
    struct Obj *owner;
    VecFx32 *position;
    char pad08[0x18];
    int alternateQuery;
    int travel;
    VecFx32 previousPosition;
};

struct Node {
    AI_TASK_FIELDS(struct State)
};

struct Zero44 {
    int words[11];
};

struct HitCommand {
    u32 flags00;
    VecFx32 vector04;
    u32 field10;
    u32 field14;
    void *hit18;
    int pad1c[4];
};

struct CollisionHit {
    int pad00[2];
    u32 flags08 : 8;
};

struct CollisionResult {
    int pad00[2];
    int field08;
};

struct Hw60 {
    u16 low : 8;
    u16 high : 8;
};

extern const VecFx32 data_02041dc8;
extern int Ov107_CollectSphereOverlaps(struct Obj *owner, ActorSphere *position, struct Obj **results);
extern void VEC_Subtract(VecFx32 *a, VecFx32 *b, VecFx32 *out);
extern void VEC_Normalize(VecFx32 *a, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, VecFx32 *a, VecFx32 *out);
extern int Ov107_InvokeHitCallback(struct Obj *candidate, struct Obj *owner,
                              struct AuxData *aux, int mode, VecFx32 *direction, int zero);
extern void func_ov107_020c0b90(struct Obj *owner, int mode, VecFx32 position, int zero);
extern void Ov107_BuildAndSendUpdate(struct Obj *owner, int zero, int event, VecFx32 *position);
extern void SetIndexedSlot(struct Node *node, int action, void *next);
extern struct Obj *Ov107_FindEntityHitBySphere(struct Obj *owner, ActorSphere *position,
                                      void *result);
extern int Ov107_AiState_ApplyHit(struct Obj *target, int value, struct HitCommand *command);
extern void *Collision_CastRay(void *collision, VecFx32 *position, VecFx32 *direction);
extern struct CollisionResult *Collision_CastSphereEx(void *collision, VecFx32 *position,
                                             VecFx32 *direction, int radius, void *ignore);
extern int VEC_Mag(VecFx32 *vector);

void Ov148_SeekTarget(struct Node *node)
{
    struct State *state = node->pState;
    void *collisionOwner = state->owner->base.pScene;
    ActorSphere origin;
    VecFx32 movement;
    VecFx32 ray;

    ((struct Hw60 *)&state->owner->base.flags60.raw)->high &= ~0x80;
    origin = state->owner->base.sphere;

    if (state->alternateQuery == 0) {
        struct Obj *results[4];
        VecFx32 direction;
        int i;
        int count;

        count = Ov107_CollectSphereOverlaps(state->owner, &origin, results);
        for (i = 0; i < count; i++) {
            VEC_Subtract(&results[i]->base.sphere.center,
                         &state->owner->base.sphere.center, &direction);
            direction.y = 0;
            VEC_Normalize(&direction, &direction);
            ScaleVec3Fx12(0x800, &direction, &direction);
            if (Ov107_InvokeHitCallback(results[i], state->owner,
                                    state->owner->aux390, 0,
                                    &direction, 0) != 0) {
                func_ov107_020c0b90(state->owner, 0, *state->position, 0);
                Ov107_BuildAndSendUpdate(state->owner, 0, 0x53, state->position);
                state->owner->base.nextState = 0;
                SetIndexedSlot(node, node->slot, 0);
                return;
            }
        }
    } else {
        /* The helper consumes the object at &queryHit; this adjacent work area
         * is part of that query workspace and must be initialized first. */
        struct Zero44 scratch = {{0}};
        void *queryHit;
        struct Obj *target;

        target = Ov107_FindEntityHitBySphere(state->owner, &origin, &queryHit);
        if (target != 0 && (*(u16 *)((char *)target + 0x1ac) & 4) == 0) {
            struct HitCommand command = {0};

            command.flags00 = (command.flags00 & 0xffff0000) | 0x2004;
            command.vector04 = data_02041dc8;
            command.field10 = (command.field10 & 0xffff0000) |
                              (((u32)state->owner->aux390->field290 << 18) >> 16);
            command.field14 = (command.field14 & 0xffff0000) |
                              (((u32)state->owner->base.field_258 << 16) >> 16);
            command.hit18 = queryHit;
            if ((((struct CollisionHit *)queryHit)->flags08 & 1) != 0 &&
                Ov107_AiState_ApplyHit(target, ((int)state->owner->base.field_25c),
                                    &command) != 0) {
                func_ov107_020c0b90(state->owner, 0, *state->position, 0);
                Ov107_BuildAndSendUpdate(state->owner, 0, 0x53, state->position);
                state->owner->base.nextState = 0;
                SetIndexedSlot(node, node->slot, 0);
                return;
            }
        }
    }

    {
        struct CollisionResult *result;

        VEC_Subtract(state->position, &state->previousPosition, &movement);
        state->previousPosition = *state->position;
        ray = movement;
        ray.y -= origin.radius;

        if (Collision_CastRay(*(void **)((char *)collisionOwner + 0x7c),
                            state->position, &ray) != 0) {
            func_ov107_020c0b90(state->owner, 1, *state->position, 0);
            state->owner->base.nextState = 0;
            SetIndexedSlot(node, node->slot, 0);
            return;
        }

        result = Collision_CastSphereEx(*(void **)((char *)collisionOwner + 0x7c),
                               state->position, &movement, 0x300, 0);
        if (result != 0 && result->field08 == 0) {
            func_ov107_020c0b90(state->owner, 1, *state->position, 0);
            state->owner->base.nextState = 0;
            SetIndexedSlot(node, node->slot, 0);
            return;
        }

        state->travel += VEC_Mag(&movement);
        if (state->travel < 0x15000 && (state->owner->aux390->flags1c4 & 0xa) == 0) {
            return;
        }
        if ((state->owner->aux390->flags1c4 & 0xa) == 0) {
            func_ov107_020c0b90(state->owner, 1, *state->position, 0);
        }
        state->owner->base.nextState = 0;
        SetIndexedSlot(node, node->slot, 0);
    }
}


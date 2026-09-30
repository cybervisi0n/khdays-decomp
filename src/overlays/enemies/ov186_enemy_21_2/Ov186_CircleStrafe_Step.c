
#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/actor.h"
#include "game/ai_task.h"
#include "game/engine.h"

struct Hw60 { u16 lo : 8; u16 hi : 8; };

struct Ov185ActionState {
    Actor *pOwner;
    Actor *pTarget;
    int aRotation8[4];
    VecFx32 vPos18;
    char pad024[8];
    VecFx32 vForward2c;
    char pad038[0x24];
    int nTurnRate5c;
    int nSpeed60;
};

struct Ov185ActionNode {
    AI_TASK_FIELDS(struct Ov185ActionState)
};

extern VecFx32 data_02042258;
extern VecFx32 data_02042264;

extern Actor *Ov107_FindNearestObject(Actor *owner, int *pDistSq);
extern int FX_Sqrt(int x);
extern void Ov186_LookAtQuat(struct Ov185ActionState *state, VecFx32 *pos);
extern void ScaleVec3Fx12(int scale, void *src, void *dst);
extern void VEC_CrossProduct(VecFx32 *a, VecFx32 *b, VecFx32 *out);
extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *unit);
extern void SetIndexedSlot(void *node, int idx, void *value);

/*
 * Per-frame step of the circle-strafe approach.
 *
 * Re-acquires the target and turns the squared distance into the gap between
 * the two body surfaces, setting the proximity bit of the actor flag byte while
 * that gap is within 0x1000 and clearing it beyond.  Past the actor's reach it
 * does nothing more.  Otherwise it runs the aim helper and rebuilds the forward
 * vector, then picks one of three behaviours by gap: inside 0x800 it advances
 * straight ahead at half speed; between 0x800 and 0x1000 it crosses the forward
 * vector with the world up axis and adds the result back, which swings the
 * movement sideways into a circling arc scaled by speed times turn rate; past
 * 0x1000 it ends the action.
 *
 * Three shapes here are deliberate.  The halfword load feeding the proximity
 * flag sits inside the taken arm, which is what keeps the pair of arms as a
 * branch instead of predicated code.  The two stack vectors are declared with
 * the forward vector first so they land at the offsets the ROM uses.  And the
 * last test is written the way round that leaves the arc as the fall-through
 * and the end-of-action as the branch target.
 */
void Ov186_CircleStrafe_Step(struct Ov185ActionNode *node)
{
    int nDist;
    VecFx32 vForward;
    VecFx32 vCross;
    struct Ov185ActionState *state;
    Actor *owner;
    Actor *target;
    u16 v;

    state = node->pState;
    state->pTarget = Ov107_FindNearestObject(state->pOwner, &nDist);
    target = state->pTarget;
    if (target == 0) {
        return;
    }
    owner = state->pOwner;
    nDist = FX_Sqrt(nDist) - (target->sphere.radius + owner->sphere.radius);
    if (nDist <= 0x1000) {
        v = state->pOwner->flags60.raw;
        state->pOwner->flags60.raw = (u16)((v & ~0xff00)
                                    | ((((((unsigned int)v << 0x10) >> 0x18) | 2)
                                        << 0x18) >> 0x10));
    } else {
        ((struct Hw60 *)&state->pOwner->flags60.raw)->hi &= ~2;
    }
    if (nDist > state->pOwner->range) {
        return;
    }
    Ov186_LookAtQuat(state, &state->vPos18);
    Vec3TransformViaTempMtx(&vForward, state->aRotation8, &data_02042258);
    if (nDist <= 0x800) {
        ScaleVec3Fx12(state->nSpeed60 / 2, &vForward, &state->vForward2c);
        return;
    }
    if (nDist <= 0x1000) {
        VEC_CrossProduct(&vForward, &data_02042264, &vCross);
        VEC_Add(&vCross, &vForward, &state->vForward2c);
        VEC_Normalize(&state->vForward2c, &state->vForward2c);
        ScaleVec3Fx12(state->nSpeed60 * state->nTurnRate5c, &state->vForward2c,
                      &state->vForward2c);
        return;
    }
    state->pOwner->nextState = 4;
    SetIndexedSlot(node, node->slot, 0);
}

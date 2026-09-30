
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
};

struct Ov120ActionNode {
    AI_TASK_FIELDS(struct Ov185ActionState)
};

extern VecFx32 data_02042258;

extern Actor *Ov107_FindNearestObject(Actor *owner, int *pDistSq);
extern void SetIndexedSlot(void *node, int idx, void *value);
extern int FX_Sqrt(int x);
extern void Ov187_LookAtQuat(struct Ov185ActionState *state, VecFx32 *pos,
                                int nDist, int nRadius);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *unit);
extern void ScaleVec3Fx12(int scale, void *src, void *dst);

/*
 * Per-frame step of the approach-target action.
 *
 * Re-acquires the lock-on target and stores it in the action state; a null
 * target ends the action.  Otherwise the squared distance the acquire call
 * returns is turned into a real distance and both body radii are subtracted,
 * leaving the gap between the two surfaces.  That gap drives the approach
 * helper, then the forward unit vector is rebuilt from the state rotation and
 * rescaled to 0x100.  Bit 1 of the actor flag byte is set while the gap is
 * within 0x1000 and cleared beyond it.  The action ends when the gap reaches
 * the actor's reach or closes to 0x800.
 *
 * The actor and node layers are the shared layouts, typed Ov120Actor and
 * Ov120ActionNode in Ghidra; only the action state is specific to this overlay.
 *
 * Two spellings of the flag edit are deliberate and not interchangeable: the
 * set arm is the explicit extract-and-reassemble, which the compiler leaves
 * untruncated, and the clear arm is the bitfield form, which carries the extra
 * halfword truncation.  The ROM has exactly that asymmetry.
 */
void Ov187_ApproachTarget_Step(struct Ov120ActionNode *node)
{
    int nDist;
    int nTargetRadius;
    int nRange;
    struct Ov185ActionState *state;
    Actor *owner;
    Actor *target;
    Actor *pTail;

    state = node->pState;
    state->pTarget = Ov107_FindNearestObject(state->pOwner, &nDist);
    target = state->pTarget;
    if (target == 0) {
        state->pOwner->nextState = 2;
        SetIndexedSlot(node, node->slot, 0);
        return;
    }
    owner = state->pOwner;
    nRange = FX_Sqrt(nDist);
    nTargetRadius = target->sphere.radius;
    nDist = nRange - (nTargetRadius + owner->sphere.radius);
    Ov187_LookAtQuat(state, &state->vPos18, nDist, nTargetRadius);
    Vec3TransformViaTempMtx(&state->vForward2c, state->aRotation8, &data_02042258);
    VEC_Normalize(&state->vForward2c, &state->vForward2c);
    ScaleVec3Fx12(0x100, &state->vForward2c, &state->vForward2c);
    if (nDist <= 0x1000) {
        u16 v = state->pOwner->flags60.raw;
        state->pOwner->flags60.raw = (u16)((v & ~0xff00)
                                    | ((((((unsigned int)v << 0x10) >> 0x18) | 2)
                                        << 0x18) >> 0x10));
    } else {
        ((struct Hw60 *)&state->pOwner->flags60.raw)->hi &= ~2;
    }
    pTail = state->pOwner;
    if (nDist >= pTail->range) {
        pTail->nextState = 2;
        SetIndexedSlot(node, node->slot, 0);
        return;
    }
    if (nDist > 0x800) {
        return;
    }
    pTail->nextState = 2;
    SetIndexedSlot(node, node->slot, 0);
}

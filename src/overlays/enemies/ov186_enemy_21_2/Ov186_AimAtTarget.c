
#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/actor.h"
#include "game/ai_task.h"

struct Ov185ActionState {
    Actor *pOwner;
    Actor *pTarget;
    char pad008[0x10];
    VecFx32 vPos18;
    char pad024[8];
    VecFx32 vForward2c;
    char pad038[0xc];
    struct Ov185Anim *pAnim44;
    char pad048[0x18];
    int nElapsed60;
    VecFx32 vLaunch64;
    int nAnim70;
};

struct Ov185Anim { int nPad0; int nId4; };

struct Ov120ActionNode {
    AI_TASK_FIELDS(struct Ov185ActionState)
};

extern void Ov186_OrbitStep(void);

extern Actor *Ov107_FindNearestObject(Actor *owner, int *pDistSq);
extern void SetIndexedSlot(void *node, int idx, void *value);
extern void VEC_Subtract(VecFx32 *a, VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *unit);
extern void Ov186_LookAtQuat_2(struct Ov185ActionState *state, VecFx32 *pos);
extern void ScaleVec3Fx12(int scale, void *src, void *dst);

/*
 * Per-frame step of the launch-at-target action.
 *
 * Re-acquires the lock-on target; a null target ends the action.  Otherwise it
 * takes the vector from the actor to the target and normalises it in place,
 * which also yields the distance, clamps that to 0x8000 and runs the aim
 * helper, then resets the vertical component of the advance vector to -0x200.
 *
 * The launch half only runs while bit 0 of the actor flag byte at 0x17a is set.
 * It flattens the delta on Y, normalises it and scales it by the clamped
 * distance over thirty to get the launch velocity, clears the timer, latches
 * the animation id and sets the proximity flag before handing the node to the
 * follow-up action.
 *
 * The two components of the flattened delta are read into locals before any of
 * the stores: that is what keeps both loads ahead of the write-back, which is
 * the order the ROM uses.
 */
void Ov186_AimAtTarget(struct Ov120ActionNode *node)
{
    VecFx32 vDelta;
    struct Ov185ActionState *state;
    int nLen;
    int nFlatZ;
    int nFlatX;

    state = node->pState;
    state->pTarget = Ov107_FindNearestObject(state->pOwner, 0);
    if (state->pTarget == 0) {
        state->pOwner->nextState = 2;
        SetIndexedSlot(node, node->slot, 0);
        return;
    }
    VEC_Subtract(&state->pTarget->vChaseTarget, &state->pOwner->srt.translation, &vDelta);
    nLen = VEC_Normalize(&vDelta, &vDelta);
    if (nLen > 0x8000) {
        nLen = 0x8000;
    }
    Ov186_LookAtQuat_2(state, &state->vPos18);
    state->vForward2c.y = -0x200;
    if (state->pOwner->contact17a.bits.bit0 == 0) {
        return;
    }
    nFlatZ = vDelta.z;
    nFlatX = vDelta.x;
    state->vLaunch64.x = nFlatX;
    state->vLaunch64.y = 0;
    state->vLaunch64.z = nFlatZ;
    VEC_Normalize(&state->vLaunch64, &state->vLaunch64);
    ScaleVec3Fx12(nLen / 30, &state->vLaunch64, &state->vLaunch64);
    state->nElapsed60 = 0;
    state->nAnim70 = state->pAnim44->nId4;
    {
        u16 v = state->pOwner->flags60.raw;
        state->pOwner->flags60.raw = (u16)((v & ~0xff00)
                                   | ((((((unsigned int)v << 0x10) >> 0x18) | 2)
                                       << 0x18) >> 0x10));
    }
    SetIndexedSlot(node, node->slot, Ov186_OrbitStep);
}

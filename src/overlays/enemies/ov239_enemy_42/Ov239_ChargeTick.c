/* Charge tick of the ov239 enemy: a negative distance to the target ends the state; the +0x14
 * velocity is the +0x398 part's +0x2c motion turned by the +0x10 yaw, its y the height of the
 * target's +0x194 above the actor's +0xb4 clamped to +-0x200 (0 without a target); the contact
 * sweep runs with kind 1 at the actor's own sphere and the +0x2c travel grows by the motion's
 * length. Past 0x10000 of travel, with the +0x30 flag set or on a wall contact (bit 1 of the
 * actor's +0x17a), sub-state 7 is configured (part action 3) and the tick hands off to cd6c8. */

#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;
struct Bits17a { unsigned char bit0 : 1, bit1 : 1; };

extern int Ov239_DistanceToTarget(int *node);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void MTX_RotY33_(Mtx33 *m, int sin, int cos);
extern void MTX_MultVec33(const VecFx32 *v, Mtx33 *m, VecFx32 *d);
extern void Ov239_ContactSweep(int *state, int kind, void *sphere);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern void Ov239_ConfigSubStateThenAdvanceSlot(int *node, int anim, int action, int flag, void *next);
extern void Ov239_stateFixedAngleMatrix(int *node);
extern short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov239_ChargeTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 mtx;
    VecFx32 dir;
    int dy;
    unsigned int idx;

    if (Ov239_DistanceToTarget(node) < 0) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    idx = ANG2IDX(state[4]);
    MTX_RotY33_(&mtx, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
    MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x398) + 0x2c), &mtx, (VecFx32 *)(state + 5));
    if (*(int *)(*state + 0x394) != 0) {
        dy = *(int *)(*(int *)(*state + 0x394) + 0x194) - *(int *)(*state + 0xb4);
        state[6] = dy;
        if (dy > 0x200) {
            dy = 0x200;
        } else if (dy < -0x200) {
            dy = -0x200;
        }
        state[6] = dy;
    } else {
        state[6] = 0;
    }
    Ov239_ContactSweep(state, 1, 0);
    state[0xb] += VEC_Normalize((VecFx32 *)(*(int *)(*state + 0x398) + 0x2c), &dir);
    if (state[0xb] < 0x10000 && *(unsigned char *)(state + 0xc) == 0 && ((struct Bits17a *)(*state + 0x17a))->bit1 == 0) {
        return;
    }
    Ov239_ConfigSubStateThenAdvanceSlot(node, 7, 3, 0, Ov239_stateFixedAngleMatrix);
}

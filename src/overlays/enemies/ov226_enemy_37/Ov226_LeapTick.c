/* Leap tick of the ov221 enemy. With a +0x7c lock, losing the target (ov221 132c below zero)
 * ends the action. The +0x5c timer accumulates the owner's rate and, once it is not negative,
 * reaction 0x14c mode 0xc fires at the +8 point (the +0x75 byte marks it). Until the timer
 * reaches the +0x70 duration the +0x14 step is the +0x34 speed along z turned by the +0x54
 * heading (the +0x50 one without a lock), its y the +0x2c base plus the +0x30 height scaled by
 * the sine of the timer's fraction of the duration (nothing without a duration) minus the +8
 * point's own y, scaled by the inverse of the rate's 0x88 fraction; the step is then handed to
 * ov221 0a2c with mode 3. Past the duration the tick waits for the +4 owner's +0xad byte to
 * clear, requests sub-state 2 -- or 8 with a lock when none of the +0x3ec family's four
 * members has a +0x38c item -- and ends the action. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;
struct Ov221Family { char pad[0x3ec]; char *aMembers[4]; };

extern int Ov226_MeasureTargetGap(int *node, VecFx32 *dir);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov107_BuildAndSendUpdate(int actor, int id, int mode, void *at);
extern void MTX_RotY33_(Mtx33 *m, int sin, int cos);
extern void MTX_MultVec33(const VecFx32 *v, Mtx33 *m, VecFx32 *d);
extern int func_02020400(int nNum, int nDen);
extern int FX_Div(int a, int b);
extern int FX_Inv(int v);
extern void Ov226_StrikeSweepEntities(int *state, int mode, int b);
extern short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)
#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

void Ov226_LeapTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 mtx;
    unsigned int idx;
    int lift;
    long i;
    int z;

    if (state[0x1f] != 0 && Ov226_MeasureTargetGap(node, 0) < 0) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    state[0x17] += *(int *)(*node + 0x2c);
    if (*(u8 *)((char *)state + 0x75) == 0 && state[0x17] >= 0) {
        Ov107_BuildAndSendUpdate(*state, 0x14c, 0xc, (void *)state[2]);
        *(u8 *)((char *)state + 0x75) = 1;
    }
    if (state[0x17] < state[0x1c]) {
        if (state[0x1f] != 0) {
            idx = ANG2IDX(state[0x15]);
            MTX_RotY33_(&mtx, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
        } else {
            idx = ANG2IDX(state[0x14]);
            MTX_RotY33_(&mtx, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
        }
        z = state[0xd];
        ((VecFx32 *)(state + 5))->x = 0;
        ((VecFx32 *)(state + 5))->y = 0;
        ((VecFx32 *)(state + 5))->z = z;
        MTX_MultVec33((VecFx32 *)(state + 5), &mtx, (VecFx32 *)(state + 5));
        if (state[0x1c] == 0) {
            lift = 0;
        } else {
            idx = ANG2IDX(func_02020400(state[0x17] * 0x3244, state[0x1c]));
            lift = state[0xc] * data_0203d210[idx * 2] / 4096;
        }
        state[6] = state[0xb] + lift;
        state[6] = state[6] - *(int *)(state[2] + 4);
        state[6] = FX_MUL(state[6], FX_Inv(FX_Div(*(int *)(*node + 0x2c), 0x88)));
        Ov226_StrikeSweepEntities(state, 3, 0);
        return;
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    *(u8 *)(*state + 0x1c7) = 2;
    if (state[0x1f] != 0) {
        struct Ov221Family *owner = (struct Ov221Family *)*state;
        for (i = 0; i < 4; i++) {
            if (owner->aMembers[i] != 0 && *(int *)(owner->aMembers[i] + 0x38c) != 0) {
                break;
            }
        }
        if (i >= 4) {
            *(u8 *)(*state + 0x1c7) = 8;
        }
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}

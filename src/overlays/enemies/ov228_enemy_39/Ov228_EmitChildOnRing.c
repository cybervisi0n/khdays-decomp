/*
 * Ov228_EmitChildOnRing -- x4 (ov228/229/233/249). AI-state tick: circle/orbit a tracked node.
 * Guard: if state[0x12]==0 and the target check (020cf344) fails, bail via 0203c634 with a 0 cb.
 * Advance timer state[0x13] -= owner_delta; while it stays positive, return. When it lapses, find
 * the first of 8 child nodes whose +0x60 bit0 is clear and drive it: build a heading vector from
 * state[0x10] (Q12 radians -> 16-bit angle -> sin/cos table), scale by state[0x12]*0x18000/8+0x2000,
 * add the owner origin (state[3]), and hand the node BOTH the world point and the raw heading via
 * 020d1ff4 (two vec3s by value). Re-arm via 0203c634 with the 020d1c14 continuation.
 */

#include "nitro/fx_types.h"

struct hw60 { unsigned short lo : 8, hi : 8; };
extern int  Ov228_MeasureTargetGap(int self);
extern void SetIndexedSlot(int self, int idx, int cb);
extern void ScaleVec3Fx12(int scale, void *in, void *out);
extern void VEC_Add(void *a, void *b, void *c);
extern void Ov228_SetTwoVecsAndFlag(int node, VecFx32 v, VecFx32 v2);
extern void Ov228_AiStep_QueueAction2OnAnimEnd_2(void);
extern short data_0203d210;

void Ov228_EmitChildOnRing(int *self, int p2, int p3, int p4) {
    int *state = (int *)self[1];
    VecFx32 out;
    VecFx32 vec;
    int t;
    int i;

    if (state[0x12] == 0 && Ov228_MeasureTargetGap((int)self) < 0) {
        SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), 0);
        return;
    }
    t = state[0x13] - *(int *)(*self + 0x2c);
    state[0x13] = t;
    if (t > 0) {
        return;
    }
    i = 0;
    do {
        if ((((struct hw60 *)(((int *)*state)[i + 0xf0] + 0x60))->lo & 1) == 0) {
            unsigned short a =
                (unsigned short)((state[0x10] * 0x28be60db9391LL + 0x80000000000LL) >> 44);
            int idx = a >> 4;
            vec.x = (&data_0203d210)[idx * 2];
            vec.y = 0;
            vec.z = (&data_0203d210)[idx * 2 + 1];
            ScaleVec3Fx12(state[0x12] * 0x18000 / 8 + 0x2000, &vec, &out);
            VEC_Add(&out, (void *)state[3], &out);
            Ov228_SetTwoVecsAndFlag(*(int *)(*state + i * 4 + 0x3c0), out, vec);
            break;
        }
        i++;
    } while (i < 8);
    SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), (int)&Ov228_AiStep_QueueAction2OnAnimEnd_2);
}

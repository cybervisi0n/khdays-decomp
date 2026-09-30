/*
 * Ov126_AimedAttackTick -- aimed-attack tick: seed the +0x3c counter with 6 times the owner's
 * +0x2c rate (30/5), copy state[5..7] down to state[2..4] (field-to-field) and scale state[5..7]
 * by 0xb00 (01ffa724). If state[3] < 0x20, clear the +0x60 hi-byte bit 0x40. If the +0x28
 * target's flag byte is set, keep waiting; else if either facing bit 0 at *state+0x17a/+0x17c is
 * set, mark *state+0x1c7 = 2 and bail (0203c634 cb=0).
 */

#include "nitro/fx_types.h"

struct S125b { char pad[0x8]; VecFx32 dst; VecFx32 src; };
struct hw60 { unsigned short lo : 8, hi : 8; };
struct b1 { unsigned char b0 : 1; };
extern void ScaleVec3Fx12(int scale, void *in, void *out);
extern void SetIndexedSlot(int self, int idx, int cb);

void Ov126_AimedAttackTick(int *self) {
    int *state = (int *)self[1];

    state[0xf] = *(int *)(self[0] + 0x2c) * 0x1e / 5;
    ((struct S125b *)state)->dst = ((struct S125b *)state)->src;
    ScaleVec3Fx12(0xb00, (void *)(state + 5), (void *)(state + 5));
    if (state[3] < 0x20) {
        ((struct hw60 *)(*state + 0x60))->hi &= ~0x40;
    }
    if (*(unsigned char *)state[0xa] != 0) {
        return;
    }
    if (((struct b1 *)(*state + 0x17a))->b0 || ((struct b1 *)(*state + 0x17c))->b0) {
        *(char *)(*state + 0x1c7) = 2;
        SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), 0);
    }
}

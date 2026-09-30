/*
 * Ov187_DecayOffsetGiveUp -- x3 (ov185/186/187). AI-state tick: decay the offset and give up when told.
 * Copy state[0x14..0x16] down to state[0xb..0xd] (field-to-field) and scale state[0x14..0x16] by 0xb00
 * (01ffa724). If state[0xc] < 0x20, clear the +0x60 hi-byte bit 0x40. If the ready byte *(u8)state[0x12]
 * is set, keep waiting; else if either facing bit (bit0 at *state+0x17a or +0x17c) is set, mark
 * *state+0x1c7 = 2 and bail (0203c634 cb=0).
 */

#include "nitro/fx_types.h"

struct S185 { char pad[0x2c]; VecFx32 dst; char pad2[0x18]; VecFx32 src; };
struct hw60 { unsigned short lo : 8, hi : 8; };
struct b1 { unsigned char b0 : 1; };
extern void ScaleVec3Fx12(int scale, void *in, void *out);
extern void SetIndexedSlot(int self, int idx, int cb);

void Ov187_DecayOffsetGiveUp(int *self) {
    int *state = (int *)self[1];

    ((struct S185 *)state)->dst = ((struct S185 *)state)->src;
    ScaleVec3Fx12(0xb00, (void *)(state + 0x14), (void *)(state + 0x14));
    if (state[0xc] < 0x20) {
        ((struct hw60 *)(*state + 0x60))->hi &= ~0x40;
    }
    if (*(unsigned char *)state[0x12] != 0) {
        return;
    }
    if (((struct b1 *)(*state + 0x17a))->b0 || ((struct b1 *)(*state + 0x17c))->b0) {
        *(char *)(*state + 0x1c7) = 2;
        SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), 0);
    }
}

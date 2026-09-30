/*
 * Ov200_DecayOffsetPickGiveUp -- x3 (ov200/201/271). AI-state tick: decay the offset, then pick a give-up
 * mode when told.
 * Copy state[6..8] down to state[3..5] (field-to-field); scale state[6..8] by 0xb00 (01ffa724). If
 * state[4] < 0x20, clear the +0x60 hi-byte bit 0x40. If the ready byte *(u8)(state[1]+0xad) is set,
 * keep waiting; else if either facing bit0 at *state+0x17a/+0x17c is set, mark *state+0x1c7 = 6 when
 * state[0x1a] != 0 else 2, and bail (0203c634 cb=0).
 */

#include "nitro/fx_types.h"

struct S200b { char pad[0xc]; VecFx32 dst; VecFx32 src; };
struct hw60 { unsigned short lo : 8, hi : 8; };
struct b1 { unsigned char b0 : 1; };
extern void ScaleVec3Fx12(int scale, void *in, void *out);
extern void SetIndexedSlot(int self, int idx, int cb);

void Ov200_DecayOffsetPickGiveUp(int *self) {
    int *state = (int *)self[1];

    ((struct S200b *)state)->dst = ((struct S200b *)state)->src;
    ScaleVec3Fx12(0xb00, (void *)(state + 6), (void *)(state + 6));
    if (state[4] < 0x20) {
        ((struct hw60 *)(*state + 0x60))->hi &= ~0x40;
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    if (((struct b1 *)(*state + 0x17a))->b0 || ((struct b1 *)(*state + 0x17c))->b0) {
        if (state[0x1a] != 0) {
            *(char *)(*state + 0x1c7) = 6;
            SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), 0);
            return;
        }
        *(char *)(*state + 0x1c7) = 2;
        SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), 0);
    }
}

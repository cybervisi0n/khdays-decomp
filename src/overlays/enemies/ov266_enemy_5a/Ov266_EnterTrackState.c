/* Enter the aim/track state (and its byte-identical twins). Clear the phase counter (ctx+0x20),
 * raise bit 0 of the hw60 hi byte, then seed the current point (ctx+0x34) and all 3
 * history slots (ctx+0x40..) from the live anchor at ctx[1], kick the tracker and
 * advance state.
 *
 * Same array-in-a-struct point as Ov212_ResetPoseCache: 0x40 is not a multiple of the
 * 12-byte element, so `arr[i]` must come off a struct member. The ROM walks the ctx
 * base and re-adds +0x40 each iteration; a pre-offset walking pointer is 1 instruction
 * short. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Ctx2 { char pad[0x40]; VecFx32 arr[3]; };

extern void SetIndexedSlot(void *self, int idx, void *cb);
extern void Ov266_TrailTick(void);

void Ov266_EnterTrackState(void *self) {
    int *ctx = *(int **)((char *)self + 4);
    int i = 0;
    unsigned short v;

    ctx[8] = 0;
    v = *(unsigned short *)(*ctx + 0x60);
    *(unsigned short *)(*ctx + 0x60) =
        (unsigned short)((v & ~0xff00) | (((((unsigned int)v << 0x10) >> 0x18 | 1) << 0x18) >> 0x10));
    *(VecFx32 *)((char *)ctx + 0x34) = *(VecFx32 *)ctx[1];
    for (; i < 3; i++) {
        ((struct Ctx2 *)ctx)->arr[i] = *(VecFx32 *)ctx[1];
    }
    Ov107_PostTagUpdate((Actor *)(*ctx), 0, 1);
    SetIndexedSlot(self, *(signed char *)((char *)self + 0x20), Ov266_TrailTick);
}

/* Enter the wind-up/telegraph state (and its byte-identical twins). Raise flags 0x46 on the
 * hw60 hi byte and flag 1 on the owner's +0x1ae word, release the 3 tracked
 * sub-objects, spawn the telegraph effect at the owner's cached point (+0x508),
 * kick the 0x49 animation, reset the phase timer and advance state.
 *
 * Sibling of Ov212_EnterRecoveryState; note this hw60 write has NO lsl#0x10/lsr#0x10
 * trunc pair, so it takes the explicit extract/reassemble form. The vec3 goes to
 * ov107_020c0b90 BY VALUE (r2, r3 and [sp+0]), with the flag at [sp+4]. */

#include "nitro/fx_types.h"

struct b8 { unsigned f : 8; };

extern void func_ov107_020c0b90(int obj, int cmd, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int a, int b, int c, void *d);
extern void SetIndexedSlot(void *self, int idx, void *cb);
extern void Ov267_AiDownTick(void);

void Ov267_EnterWindUpState(void *self) {
    int *ctx = *(int **)((char *)self + 4);
    int i = 0;
    unsigned short v;

    v = *(unsigned short *)(*ctx + 0x60);
    *(unsigned short *)(*ctx + 0x60) =
        (unsigned short)((v & ~0xff00) | (((((unsigned int)v << 0x10) >> 0x18 | 0x46) << 0x18) >> 0x10));
    *(unsigned short *)(*ctx + 0x1ae) |= 1;
    for (; i < 3; i++) {
        ((struct b8 *)(((int *)*ctx)[i + 0x133] + 8))->f &= ~1;
    }
    func_ov107_020c0b90(*ctx, 1, *(VecFx32 *)(*ctx + 0x508), 0);
    Ov107_BuildAndSendUpdate(*ctx, 0, 0x49, (void *)(*ctx + 0x74));
    ctx[0x10] = 0;
    SetIndexedSlot(self, *(signed char *)((char *)self + 0x20), Ov267_AiDownTick);
}

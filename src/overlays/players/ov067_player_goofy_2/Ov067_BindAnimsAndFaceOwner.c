/* Starts the attack effect at the locked target's point: binds and rewinds its tracks, turns it
 * with the character and activates it; the local player also gets a short rumble. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void BindAnimTrack(int a, int b, int c, int d);
extern void Anim_SetFrameWrapped(int a, int b, int c);
extern void func_ov022_020ad44c(void *out, int self);

void Ov067_BindAnimsAndFaceOwner(int self, char *blk) {
    VecFx32 v;
    BindAnimTrack((int)(blk + 0x120), 0, (int)(blk + 0x200), 0);
    BindAnimTrack((int)(blk + 0x120), 2, (int)(blk + 0x200), 0);
    Anim_SetFrameWrapped((int)(blk + 0x120), 0, 0);
    Anim_SetFrameWrapped((int)(blk + 0x120), 2, 0);
    func_ov022_020ad44c(&v, self);
    *(unsigned short *)(blk + 0x19c) =
        (unsigned short)(*(unsigned short *)(*(int *)(self + 0x20) + 0x80) - 0x8000) + 0x8000;
    *(unsigned short *)(blk + 0x120) |= 0x20;
    *(VecFx32 *)(blk + 0x1c4) = v;
    *(int *)(blk + 0x11c) = 1;
    if (Session_GetLocalPlayerIndex() != 0) return;
    if ((*(int *)self & 0x10000) != 0) return;
    *(signed char *)(self + 0x47a) = 3;
    *(signed char *)(self + 0x47b) = 1;
}

/* Start of the ov258 beam effect: the beam rig's transform resets and moves to the stored start,
 * both rigs clear bit 1 of their +0x5c flags, the beam plays tracks 0, 2 and 3 and the glow rig loops
 * tracks 0 and 2, the beam is stretched 3.0 x 3.0 x 1.0 (glow 1.0) and posed; the +0x28 / +0x2c / +0x34
 * timers and the +0x48 flag clear and the brain waits on 020d0fe0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void SrtTransform_SetIdentity(void *transform);
extern void Srt_SetTranslation(void *transform, const VecFx32 *translation);
extern void Srt_SetScaleXYZ(void *transform, int x, int y, int z);
extern void SetSubitemState(int rig, int channel, int a, int b);
extern void RefreshObjectCallbacks(int rig, int a);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov258_BeamUpdate(void);

void Ov258_BeamStart(int *node)
{
    int *state = (int *)node[1];

    SrtTransform_SetIdentity((void *)(state[0] + 4));
    Srt_SetTranslation((void *)(state[0] + 4), (VecFx32 *)(state + 4));
    *(int *)(state[0] + 0x5c) &= ~2;
    SetSubitemState(state[0], 0, 0, 0);
    SetSubitemState(state[0], 2, 0, 0);
    SetSubitemState(state[0], 3, 0, 0);
    *(int *)(state[1] + 0x5c) &= ~2;
    SetSubitemState(state[1], 0, 0, 1);
    SetSubitemState(state[1], 2, 0, 1);
    Srt_SetScaleXYZ((void *)(state[0] + 4), 0x3000, 0x3000, 0x1000);
    Srt_SetScaleXYZ((void *)(state[1] + 4), 0x1000, 0x1000, 0x1000);
    RefreshObjectCallbacks(state[0], 0);
    state[0xa] = 0;
    state[0xb] = 0;
    state[0xd] = 0;
    *(u8 *)(state + 0x12) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov258_BeamUpdate);
}

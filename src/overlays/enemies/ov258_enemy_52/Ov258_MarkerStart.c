/* Start of the ov258 marker effect: its rig transform resets, scales to 1.375 and moves to the
 * stored position, bit 1 of the rig's +0x5c flags clears, tracks 0 and 2 play and the brain waits on
 * 020d1618. */

#include "nitro/fx_types.h"

extern void SrtTransform_SetIdentity(void *transform);
extern void Srt_SetScaleXYZ(void *transform, int x, int y, int z);
extern void Srt_SetTranslation(void *transform, const VecFx32 *translation);
extern void SetSubitemState(int rig, int channel, int a, int b);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov258_ShowReactionReady(void);

void Ov258_MarkerStart(int *node)
{
    int *state = (int *)node[1];

    SrtTransform_SetIdentity((void *)(state[1] + 4));
    Srt_SetScaleXYZ((void *)(state[1] + 4), 0x1600, 0x1600, 0x1600);
    Srt_SetTranslation((void *)(state[1] + 4), (VecFx32 *)(state + 2));
    *(int *)(state[1] + 0x5c) &= ~2;
    SetSubitemState(state[1], 0, 0, 0);
    SetSubitemState(state[1], 2, 0, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov258_ShowReactionReady);
}

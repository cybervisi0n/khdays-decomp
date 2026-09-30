/* Start of the ov258 shot effect: its rig transform resets, scales to 0.5 and moves to the stored
 * position, bit 1 of the rig's +0x5c flags clears, tracks 0 and 2 play and the rig is posed; the
 * +0x14 timer and +0x18 flag clear, +0x19 is set and the brain waits on 020d0800. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void SrtTransform_SetIdentity(void *transform);
extern void Srt_SetScaleXYZ(void *transform, int x, int y, int z);
extern void Srt_SetTranslation(void *transform, const VecFx32 *translation);
extern void SetSubitemState(int rig, int channel, int a, int b);
extern void RefreshObjectCallbacks(int rig, int a);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov258_ShotUpdate(void);

void Ov258_ShotEffectStart(int *node)
{
    int *state = (int *)node[1];

    SrtTransform_SetIdentity((void *)(*state + 4));
    Srt_SetScaleXYZ((void *)(*state + 4), 0x800, 0x800, 0x800);
    Srt_SetTranslation((void *)(*state + 4), (VecFx32 *)(state + 2));
    *(int *)(*state + 0x5c) &= ~2;
    SetSubitemState(*state, 0, 0, 0);
    SetSubitemState(*state, 2, 0, 0);
    RefreshObjectCallbacks(*state, 0);
    state[5] = 0;
    *(u8 *)(state + 6) = 0;
    *((u8 *)state + 0x19) = 1;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov258_ShotUpdate);
}

/* Start of an ov252 gem: its model's transform resets (0203c960) and moves to the gem's +8 point, bit 1
 * of the model's +0x5c flags clears, the owner's +0x57e mask gains the gem's bit (+0x24 index), layers
 * 0, 2, 4 and 1 play (mode 0, 0), the model is scaled 2.5/4.06 (big gem, +0x25) or 2.0/3.25, the rig pose
 * resets, +0x20 clears, the first-blink flag (+0x26) is set and the node moves on to 020d3d10. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void SrtTransform_SetIdentity(void *srt);
extern void Srt_SetTranslation(void *srt, const VecFx32 *v);
extern void SetSubitemState(int rig, int channel, int a, int b);
extern void Srt_SetScaleXYZ(void *placement, int x, int y, int z);
extern void RefreshObjectCallbacks(int rig, int a);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_GemBlinkTick(void);

void Ov252_GemStart(int *node)
{
    int *state = (int *)node[1];

    SrtTransform_SetIdentity((void *)(*state + 4));
    Srt_SetTranslation((void *)(*state + 4), (VecFx32 *)(state + 2));
    *(int *)(*state + 0x5c) &= ~2;
    *(u16 *)(state[1] + 0x57e) |= 1 << *((signed char *)state + 0x24);
    SetSubitemState(*state, 0, 0, 0);
    SetSubitemState(*state, 2, 0, 0);
    SetSubitemState(*state, 4, 0, 0);
    SetSubitemState(*state, 1, 0, 0);
    if (*((unsigned char *)state + 0x25) == 0) {
        state[5] = 0x2000;
        state[6] = 0x3400;
        state[7] = 0x2000;
    } else {
        state[5] = 0x2800;
        state[6] = 0x4100;
        state[7] = 0x2800;
    }
    Srt_SetScaleXYZ((void *)(*state + 4), state[5], state[6], state[7]);
    RefreshObjectCallbacks(*state, 0);
    state[8] = 0;
    *((unsigned char *)state + 0x26) = 1;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_GemBlinkTick);
}

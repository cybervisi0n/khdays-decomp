/* Start of an ov252 shard: its model's transform resets (0203c960) and moves to the spawner's +0x570
 * model point, bit 1 of the model's +0x5c flags clears, the spawner's +0x57a mask gains the shard's bit
 * (+0x24 index), layers 0, 2, 4 and 1 play (mode 0, 1), the rig pose resets, the +0x14 velocity is
 * scaled by 3.0, +0x20 clears and the node moves on to 020d2f6c. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void SrtTransform_SetIdentity(void *srt);
extern void Srt_SetTranslation(void *srt, const VecFx32 *v);
extern void SetSubitemState(int rig, int channel, int a, int b);
extern void RefreshObjectCallbacks(int rig, int a);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_ShotFlightTick(void);

void Ov252_ShardStart(int *node)
{
    int *state = (int *)node[1];

    SrtTransform_SetIdentity((void *)(*state + 4));
    Srt_SetTranslation((void *)(*state + 4), (VecFx32 *)(*(int *)(state[1] + 0x570) + 0x14));
    *(int *)(*state + 0x5c) &= ~2;
    *(u16 *)(state[1] + 0x57a) |= 1 << *((signed char *)state + 0x24);
    SetSubitemState(*state, 0, 0, 1);
    SetSubitemState(*state, 2, 0, 1);
    SetSubitemState(*state, 4, 0, 1);
    SetSubitemState(*state, 1, 0, 1);
    RefreshObjectCallbacks(*state, 0);
    ScaleVec3Fx12(0x3000, (VecFx32 *)(state + 5), (VecFx32 *)(state + 5));
    state[8] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_ShotFlightTick);
}

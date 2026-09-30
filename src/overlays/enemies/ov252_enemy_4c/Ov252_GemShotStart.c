/* Start of an ov252 gem shot: its model's transform resets and moves to the spawner's +0x570 model
 * point at scale 4.0, bit 1 of its +0x5c flags clears, the spawner's +0x57c mask gains its bit (+0x34
 * index), layers 0, 2, 4 and 1 play (mode 0, 0) and the rig pose resets. With no target (020cab14, into
 * +8) the layers fade out (mode 2) and the node moves on to 020d38e8; otherwise the spawner plays effect
 * 5 at the origin, a target below 10.0 sets +0x38, the +0x18 heading (and its +0x24 copy) points from
 * the +0xc point to the target, +0x30 clears and the node moves on to 020d346c. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern void SrtTransform_SetIdentity(void *srt);
extern void Srt_SetTranslation(void *srt, const VecFx32 *v);
extern void Srt_SetScaleXYZ(void *placement, int x, int y, int z);
extern void SetSubitemState(int rig, int channel, int a, int b);
extern void RefreshObjectCallbacks(int rig, int a);
extern int Ov107_FindNearestObject(int actor, int *distOut);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_GemFinishTick(void);
extern void Ov252_GemShotTick(void);
extern const VecFx32 data_02041dc8;

void Ov252_GemShotStart(int *node)
{
    int *state = (int *)node[1];

    SrtTransform_SetIdentity((void *)(*state + 4));
    Srt_SetTranslation((void *)(*state + 4), (VecFx32 *)(*(int *)(state[1] + 0x570) + 0x14));
    Srt_SetScaleXYZ((void *)(*state + 4), 0x4000, 0x4000, 0x4000);
    *(int *)(*state + 0x5c) &= ~2;
    *(u16 *)(state[1] + 0x57c) |= 1 << *((signed char *)state + 0x34);
    SetSubitemState(*state, 0, 0, 0);
    SetSubitemState(*state, 2, 0, 0);
    SetSubitemState(*state, 4, 0, 0);
    SetSubitemState(*state, 1, 0, 0);
    RefreshObjectCallbacks(*state, 0);
    state[2] = Ov107_FindNearestObject(state[1], 0);
    if (state[2] == 0) {
        SetSubitemState(*state, 0, 2, 0);
        SetSubitemState(*state, 2, 2, 0);
        SetSubitemState(*state, 4, 2, 0);
        SetSubitemState(*state, 1, 2, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_GemFinishTick);
    } else {
        func_ov107_020c0b90(state[1], 5, data_02041dc8, 0);
        if (*(int *)(state[2] + 0x194) < 0xa000) {
            state[0xe] = 1;
        }
        VEC_Subtract((VecFx32 *)(state[2] + 0x190), (VecFx32 *)(state + 3), (VecFx32 *)(state + 6));
        VEC_Normalize((VecFx32 *)(state + 6), (VecFx32 *)(state + 6));
        *(VecFx32 *)(state + 9) = *(VecFx32 *)(state + 6);
        state[0xc] = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_GemShotTick);
    }
}

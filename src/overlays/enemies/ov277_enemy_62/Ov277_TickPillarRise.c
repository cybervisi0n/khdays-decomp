/* Rise tick of the ov277 enemy's pillar: the pillar follows the +0xc owner's +0x3c8 bone, lifted to
 * 0.125 above the owner's +0xb4 floor; the +4 clock runs up at the owner's rate and the +0x10 stage
 * advances at 2.5, then at 3.13 and 3.33 with rumble effects 0xb and 0xc (sound 0x165) at the pillar.
 * Once the pillar's +0xad rig is idle its actions 0/2/4/1 are enabled, its animation stops and
 * 020ce734 follows. */

#include "nitro/fx_types.h"

extern void Srt_SetTranslation(int srt, VecFx32 *pos);
extern void Slot_Spawn(int id, int kind, VecFx32 *pos, int flag);
extern void SetSubitemState(int obj, int slot, int a, int b);
extern void RefreshObjectCallbacks(int obj, int a);
extern void SetIndexedSlot(int self, int idx, void *cb);
extern void Ov277_WaitRigIdleBlendOut(void);
void Ov277_TickPillarRise(int param_1) {
    int *node = *(int **)(param_1 + 4);
    VecFx32 pos;

    pos = *(VecFx32 *)(*(int *)(node[3] + 0x3c8) + 0x14);
    pos.y = *(int *)(node[3] + 0xb4) + 0x200;
    Srt_SetTranslation(node[0] + 4, &pos);
    node[1] += *(int *)(*(int *)param_1 + 0x2c);
    if (*((unsigned char *)node + 0x10) == 0 && node[1] >= 0x2800) {
        *((unsigned char *)node + 0x10) += 1;
    } else if (*((unsigned char *)node + 0x10) == 1 && node[1] >= 0x3214) {
        *((unsigned char *)node + 0x10) += 1;
        Slot_Spawn(0x165, 0xb, &pos, 0);
    } else if (*((unsigned char *)node + 0x10) == 2 && node[1] >= 0x3547) {
        *((unsigned char *)node + 0x10) += 1;
        Slot_Spawn(0x165, 0xc, &pos, 0);
    }
    if (*(unsigned char *)(node[0] + 0xad) == 0) {
        SetSubitemState(node[0], 0, 1, 0);
        SetSubitemState(node[0], 2, 1, 0);
        SetSubitemState(node[0], 4, 1, 0);
        SetSubitemState(node[0], 1, 1, 0);
        RefreshObjectCallbacks(node[0], 0);
        SetIndexedSlot(param_1, *(signed char *)((char *)param_1 + 0x20), &Ov277_WaitRigIdleBlendOut);
        return;
    }
}

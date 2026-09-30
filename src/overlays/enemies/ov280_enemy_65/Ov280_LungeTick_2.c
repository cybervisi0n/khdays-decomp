/* Lunge tick of the ov280 enemy (variant of ov231/ov232's). The +0x30
 * velocity is the +0x388 part's +0x2c vector turned by the heading, halved. The +0x28 timer
 * accumulates the owner's rate; past 0xff0 reaction +0x50 mode 9 fires once (+0x4c) at the +8
 * point. Between 0x1298 and 0x2b90 the target is re-acquired (Ov280_AcquireTarget) and a probe
 * segment from the +0xc point lifted 5.13 along data_02042240 (length 2.0, radius 4.81) hits kind 2
 * candidates towards the heading lifted 2.0 (Ov280_ProbeSpawnPoint); on a hit reaction 0/0x51 fires at
 * the owner's +0x74 point. Past 0xff0 a pending +0x54 flag spawns effect 4 at the origin once. When
 * the +0x10 idle byte clears, sub-state 2 is requested and the tick ends. */

#include "nitro/fx_types.h"

typedef struct { VecFx32 p0; VecFx32 dir; int nLength; int nRadius; } Segment;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

extern void Ov280_rotateVecByOwnerYaw(void *out, int *self, VecFx32 *vec);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void Ov280_AcquireTarget(int *node);
extern int Ov280_ProbeSpawnPoint(int *self, int kind, void *query, void *pt, int flags);
extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern const VecFx32 data_02042240;
extern const VecFx32 data_02041dc8;

void Ov280_LungeTick_2(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    Segment seg;
    VecFx32 v;

    Ov280_rotateVecByOwnerYaw(&v, node, (VecFx32 *)(*(int *)(*state + 0x388) + 0x2c));
    *(VecFx32 *)(state + 0xc) = v;
    ScaleVec3Fx12(0x800, (VecFx32 *)(state + 0xc), (VecFx32 *)(state + 0xc));
    state[0xa] += *(int *)(*node + 0x2c);
    if (state[0xa] >= 0xff0 && *((unsigned char *)state + 0x4c) != 0) {
        *((unsigned char *)state + 0x4c) = 0;
        Ov107_BuildAndSendUpdate(*state, *(short *)((char *)state + 0x50), 9, (void *)state[2]);
    }
    if (state[0xa] >= 0x1298 && state[0xa] < 0x2b90) {
        Ov280_AcquireTarget(node);
        dir.x = data_0203d210[ANG2IDX(state[6]) * 2];
        dir.y = 0x2000;
        dir.z = data_0203d210[ANG2IDX(state[6]) * 2 + 1];
        seg.p0 = *(VecFx32 *)state[3];
        seg.p0.y += 0x5210;
        seg.dir = data_02042240;
        seg.nLength = 0x2000;
        seg.nRadius = 0x4cef;
        if (Ov280_ProbeSpawnPoint(node, 0, &seg, &dir, 2) != 0) {
            Ov107_BuildAndSendUpdate(*state, 0, 0x51, (void *)(*state + 0x74));
        }
    }
    if (state[0xa] >= 0xff0 && state[0x15] != 0) {
        func_ov107_020c0b90(*state, 4, data_02041dc8, 0);
        state[0x15] = 0;
    }
    if (*(unsigned char *)state[4] != 0) {
        return;
    }
    *(unsigned char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}

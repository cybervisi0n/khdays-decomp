/* Lunge tick of the ov231 enemy (x2 with ov232). The target is re-acquired
 * (Ov232_AcquireTarget) and a sideways drift is chosen from the +0x20 distance (0x180 out past
 * 9.0, 0x280 in below 7.0). The +0x28 timer accumulates the owner's rate; past 0xff0 reaction
 * +0x50 mode 8 fires once (+0x4c) at the +8 point. Between 0x1100 and 0x1650 a sphere of radius
 * 1.55 at the +0x3d0 part's +0x14 point hits kind 1 candidates towards the heading lowered 1.125
 * (Ov232_ProbeSpawnPoint); on a hit reaction 0/0x50 fires at the owner's +0x74 point. Past 0x1100 a
 * pending +0x54 flag spawns effect 3 at the origin once. The +0x30 velocity is the +0x388 part's
 * +0x2c vector plus the drift, both turned by the heading (Ov232_rotateVecByOwnerYaw). When the +0x10
 * idle byte clears, sub-state 2 is requested and the tick ends. */

#include "nitro/fx_types.h"

typedef struct { VecFx32 pos; int radius; } Sphere;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

extern void Ov232_AcquireTarget(int *node);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern int Ov232_ProbeSpawnPoint(int *self, Sphere *sphere, void *query, void *pt, int flags);
extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void Ov232_rotateVecByOwnerYaw(void *out, int *self, VecFx32 *vec);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern const VecFx32 data_02041dc8;

static inline void VEC_Set(VecFx32 *vec, int x, int y, int z)
{
    int *components = (int *)vec;
    components[0] = x;
    components[1] = y;
    components[2] = z;
}

void Ov232_LungeTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 lift;
    VecFx32 dir;
    Sphere sphere;
    VecFx32 v;

    VEC_Set(&lift, 0, 0, 0);
    state[0xa] += *(int *)(*node + 0x2c);
    Ov232_AcquireTarget(node);
    if (state[8] > 0x2400) {
        lift.z = 0x180;
    } else if (state[8] < 0x1c00) {
        lift.z = -0x280;
    }
    if (state[0xa] >= 0xff0 && *((unsigned char *)state + 0x4c) != 0) {
        *((unsigned char *)state + 0x4c) = 0;
        Ov107_BuildAndSendUpdate(*state, *(short *)((char *)state + 0x50), 8, (void *)state[2]);
    }
    if (state[0xa] >= 0x1100 && state[0xa] < 0x1650) {
        dir.x = data_0203d210[ANG2IDX(state[6]) * 2];
        dir.y = -0x1200;
        dir.z = data_0203d210[ANG2IDX(state[6]) * 2 + 1];
        sphere.radius = 0x18cc;
        sphere.pos = *(VecFx32 *)(*(int *)(*state + 0x3d0) + 0x14);
        if (Ov232_ProbeSpawnPoint(node, &sphere, 0, &dir, 1) != 0) {
            Ov107_BuildAndSendUpdate(*state, 0, 0x50, (void *)(*state + 0x74));
        }
    }
    if (state[0xa] >= 0x1100 && state[0x15] != 0) {
        func_ov107_020c0b90(*state, 3, data_02041dc8, 0);
        state[0x15] = 0;
    }
    Ov232_rotateVecByOwnerYaw(&lift, node, &lift);
    Ov232_rotateVecByOwnerYaw(&v, node, (VecFx32 *)(*(int *)(*state + 0x388) + 0x2c));
    *(VecFx32 *)(state + 0xc) = v;
    VEC_Add((VecFx32 *)(state + 0xc), &lift, (VecFx32 *)(state + 0xc));
    if (*(unsigned char *)state[4] != 0) {
        return;
    }
    *(unsigned char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}

/* Orbit tick (slow): the +0xc velocity is the +0x430 partner's +0x2c vector turned by the +0x30
 * yaw and scaled by 0.75, kept level. Once the +4 item's +0xad byte clears and no move is pending,
 * a missing target (020cd080) requests move 2; the handler is then cleared. */

#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;

extern void MTX_RotY33_(Mtx33 *m, int sin, int cos);
extern void MTX_MultVec33(const VecFx32 *v, Mtx33 *m, VecFx32 *d);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern int Ov254_CheckTarget(int *node);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov254_SlowOrbitTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 m;
    unsigned int idx = ANG2IDX(state[0xc]);

    MTX_RotY33_(&m, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
    MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x430) + 0x2c), &m, (VecFx32 *)(state + 3));
    ScaleVec3Fx12(0xc00, (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    state[4] = 0;
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    if (*(signed char *)(*state + 0x100 + 0xc7) == -1 && Ov254_CheckTarget(node) == 0) {
        *(signed char *)(*state + 0x1c7) = 2;
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}

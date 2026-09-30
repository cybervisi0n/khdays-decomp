/* Swoop tick: the +0x44 timer accumulates the frame rate (clamped to 0..0x2a80); the +0xc velocity
 * is the +0x430 partner's +0x2c vector turned by the +0x30 yaw, and the +0x10 climb follows a
 * quarter sine of the +0x54 drop over the timer, relative to the +8 track's height above the
 * +0x50 start. Once the +4 item's +0xad byte clears the next move is 0xb. */

#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;

extern void MTX_RotY33_(Mtx33 *m, int sin, int cos);
extern void MTX_MultVec33(const VecFx32 *v, Mtx33 *m, VecFx32 *d);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov254_SwoopTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 m;
    int t;
    unsigned int idx;

    state[0x11] += *(int *)(node[0] + 0x2c);
    t = state[0x11] > 0x2a80 ? 0x2a80 : (state[0x11] < 0 ? 0 : state[0x11]);
    idx = ANG2IDX(state[0xc]);
    MTX_RotY33_(&m, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
    MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x430) + 0x2c), &m, (VecFx32 *)(state + 3));
    ScaleVec3Fx12(0x1000, (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    state[4] = FX_Mul(data_0203d210[ANG2IDX(t * 0x1922 / 0x2a80) * 2], state[0x15]) -
               (*(int *)(state[2] + 4) - state[0x14]);
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    *(signed char *)(*state + 0x1c7) = 0xb;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}

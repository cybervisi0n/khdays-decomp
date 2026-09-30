/* Orbit tick (animated): the +0xc velocity is the +0x430 partner's +0x2c vector turned by the
 * +0x30 yaw and scaled by 0.75, kept level. Once the +4 item's +0xad byte clears the actor plays
 * pose +0x75 + 1 and the partner motion +0x76 + 1, the +0x44 timer clears and the node moves to
 * 020ce8a0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;

extern void MTX_RotY33_(Mtx33 *m, int sin, int cos);
extern void MTX_MultVec33(const VecFx32 *v, Mtx33 *m, VecFx32 *d);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void Ov107_PostTagUpdate(int actor, int pose, int flag);
extern void Ov107_StartAnim(int part, int motion, int mode);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern void Ov254_PursuitTick(void);

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov254_AnimatedOrbitTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 m;
    unsigned int idx = ANG2IDX(state[0xc]);

    MTX_RotY33_(&m, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
    MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x430) + 0x2c), &m, (VecFx32 *)(state + 3));
    ScaleVec3Fx12(0xc00, (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    state[4] = 0;
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate(*state, *((u8 *)state + 0x75) + 1, 0);
    Ov107_StartAnim(*(int *)(*state + 0x430), *((u8 *)state + 0x76) + 1, 0);
    state[0x11] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov254_PursuitTick);
}

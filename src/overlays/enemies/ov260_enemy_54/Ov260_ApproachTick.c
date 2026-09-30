/* Approach tick of the ov260 actor: the +0x20 velocity is its +0x428 part's +0x2c vector turned by
 * the +0x64 heading; the nearest live entity becomes the +0x420 target and +0x50 its goal: the target's
 * +0x190 point when farther than 5.5, else the point 8.0 short of it along the approach (+0x44), or
 * none. Once the partner holds no queued move pose 2 plays, the part takes motion 1, +0x70 and the
 * +0x7b flag clear and the node moves on to 020cdecc. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int m[9]; } Mtx33;

extern void MTX_RotY33_(Mtx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pIn, const Mtx33 *pMtx, VecFx32 *pOut);
extern int Ov107_FindNearestObject(int obj, int kind);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov260_WalkTick(void);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov260_ApproachTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 rot;
    VecFx32 d;

    {
        int idx = ANG2IDX(state[0x19]) * 2;

        MTX_RotY33_(&rot, data_0203d210[idx], data_0203d210[idx + 1]);
    }
    MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x428) + 0x2c), &rot, (VecFx32 *)(state + 8));
    *(int *)(*state + 0x420) = Ov107_FindNearestObject(*state, 0);
    if (*(int *)(*state + 0x420) != 0) {
        VEC_Subtract((VecFx32 *)(*(int *)(*state + 0x420) + 0x190), (VecFx32 *)state[4], &d);
        if (VEC_Normalize(&d, &d) > 0x5800) {
            state[0x14] = *(int *)(*state + 0x420) + 0x190;
        } else {
            ScaleVec3Fx12(0x8000, &d, &d);
            VEC_Subtract((VecFx32 *)(*(int *)(*state + 0x420) + 0x190), &d, (VecFx32 *)(state + 0x11));
            state[0x14] = (int)(state + 0x11);
        }
    } else {
        state[0x14] = 0;
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 2, 0);
    Ov107_StartAnim(*(int *)(*state + 0x428), 1, 0);
    state[0x1c] = 0;
    *((u8 *)state + 0x7b) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_WalkTick);
}

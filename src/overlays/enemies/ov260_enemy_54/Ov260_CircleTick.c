/* Circle tick of the ov260 actor: the +0x20 velocity is its +0x428 part's +0x2c vector turned by the
 * +0x64 heading and the body sweeps for hits (020cd2a0 kind 3). Once the partner holds no queued
 * move the circle count (+0x74) grows and a new side heading (+0x6c) is rolled 35-70 degrees left or
 * right of +0x64; the landing point (+0x14) is the target's +0x190 point stepped back 4.0 along it
 * (2.0 from the fifth circle), pushed off the scene's walls within the body radius, the recoil entry
 * is armed (+0xc = 020cebe4) and the node moves on to 020cf484. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;

extern void MTX_RotY33_(Mtx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pIn, const Mtx33 *pMtx, VecFx32 *pOut);
extern void Ov260_AttackSweep(int *state, int kind, void *sphere, void *cyl, void *seg);
extern int RandNextScaled(int n);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern int Collision_CastSphere(int walls, VecFx32 *at, VecFx32 *dir, int radius);
extern void ScaleVec3Fixed27(int plane, VecFx32 *in, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov260_AiEndCircle(void);
extern void Ov260_BurstEntry(void);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov260_CircleTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 rot;
    VecFx32 off;
    int scene;
    int hit;

    {
        int idx = ANG2IDX(state[0x19]) * 2;

        MTX_RotY33_(&rot, data_0203d210[idx], data_0203d210[idx + 1]);
    }
    MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x428) + 0x2c), &rot, (VecFx32 *)(state + 8));
    Ov260_AttackSweep(state, 3, 0, 0, 0);
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    scene = *(int *)(*state + 4);
    state[0x1d]++;
    if (RandNextScaled(2) != 0) {
        int r = RandNextScaled(0x1923) + (hit - hit);

        state[0x1b] = state[0x19] - 0x3244 + r;
    } else {
        int r = RandNextScaled(0x1923) - 0x1922;

        state[0x1b] = state[0x19] + 0x3244 + r;
    }
    {
        int idx = ANG2IDX(state[0x1b]) * 2;

        off.x = data_0203d210[idx];
        off.y = 0;
        off.z = data_0203d210[idx + 1];
    }
    ScaleVec3Fx12(state[0x1d] < 5 ? -0x4000 : -0x2000, &off, &off);
    hit = Collision_CastSphere(*(int *)(scene + 0x7c), (VecFx32 *)(*(int *)(*state + 0x420) + 0x190), &off,
                        *(int *)(*state + 0x80));
    if (hit != 0) {
        ScaleVec3Fixed27(*(int *)(hit + 0xc), &off, &off);
    }
    VEC_Add((VecFx32 *)(*(int *)(*state + 0x420) + 0x190), &off, (VecFx32 *)(state + 5));
    state[3] = (int)Ov260_AiEndCircle;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_BurstEntry);
}

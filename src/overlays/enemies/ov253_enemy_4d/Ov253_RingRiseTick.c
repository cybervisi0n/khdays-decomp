/* Rising-ring tick of the ov253 actor: the +0x1c climb speed grows by 30/128 of the frame rate (at most
 * 1/16) and lowers the +0xc height; the +0x18 radius eases 1/20 of the way toward 8.0 each tick. The
 * +4 rig's particles (+0x90 array of 0x38-byte entries, +0x8c count) are spread evenly on the ring
 * around the +8 point at that height, each scaled by 8.0 / radius. When the owner's +0x50 mode is 1 an
 * upright cylinder of the ring's radius at the +8 point sweeps the +0x388 body's hits, and every hit
 * the body accepts (020cceb0) gets a kind-3 hit of strength 0x10. Once the height falls below the
 * body's +0xb4 floor plus 1.0 the +0x14 clock resets and brain slot +0x20 runs 020d372c. */

#include "nitro/fx_types.h"

typedef struct { VecFx32 pos; VecFx32 axis[3]; int radius; int flag; } Cylinder;

extern int FX_Div(int a, int b);
extern int func_02020400(int a, int b);
extern int Ov107_CollectEntitiesTouchingDisc(int actor, void *box, int *out);
extern int Ov253_IdIsFree(int item, int hit);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, const VecFx32 *push, int z);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov253_RingSpreadTick(void);
extern const short data_0203d210[];
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042258;
extern const VecFx32 data_02042264;
extern const VecFx32 data_02041dc8;

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

static inline int FX_Mul(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}

static inline unsigned short FX_RadToIdx(int rad)
{
    return (unsigned short)((0x28BE60DB9391LL * rad + 0x80000000000LL) >> 44);
}

void Ov253_RingRiseTick(int *node)
{
    int *state = (int *)node[1];
    int inv;
    unsigned short idx;
    int i;
    char *entry;
    int n;

    state[7] += FX_MUL(*(int *)(node[0] + 0x2c) * 0x1e, 0x20);
    if (state[7] > 0x100) {
        state[7] = 0x100;
    }
    state[3] -= state[7];
    state[6] += (0x8000 - state[6]) / 20;
    inv = FX_Div(state[6], 0x8000);
    for (i = 0; i < *(int *)(state[1] + 0x8c); i++) {
        entry = *(char **)(state[1] + 0x90) + i * 0x38;
        idx = FX_RadToIdx(func_02020400(i * 0x6488, *(int *)(state[1] + 0x8c)));
        *(int *)(entry + 0x2c) = state[2] + FX_Mul(data_0203d210[(idx >> 4) << 1], state[6]);
        *(int *)(entry + 0x30) = state[3];
        idx = FX_RadToIdx(func_02020400(i * 0x6488, *(int *)(state[1] + 0x8c)));
        *(int *)(entry + 0x34) = state[4] + FX_Mul(data_0203d210[((idx >> 4) << 1) + 1], state[6]);
        *(int *)entry = inv;
    }
    if (*(int *)(*state + 0x50) == 1) {
        int hits[4];
        Cylinder cyl = {0};
        VecFx32 d;

        cyl.pos = *(VecFx32 *)(state + 2);
        cyl.axis[0] = data_02042270;
        cyl.axis[1] = data_02042258;
        cyl.axis[2] = data_02042264;
        cyl.radius = state[6];
        cyl.flag = 0;
        n = Ov107_CollectEntitiesTouchingDisc(*(int *)(*state + 0x388), &cyl, hits);
        for (i = 0; i < n; i++) {
            if (Ov253_IdIsFree(*(int *)(*state + 0x388), hits[i]) != 0) {
                VEC_Subtract((VecFx32 *)(hits[i] + 0x74), &cyl.pos, &d);
                d.y = 0;
                VEC_Normalize(&d, &d);
                ScaleVec3Fx12(0x1000, &d, &d);
                Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x388), 3, &data_02041dc8, 0x10);
            }
        }
    }
    if (state[3] >= *(int *)(*(int *)(*state + 0x388) + 0xb4) + 0x1000) {
        return;
    }
    state[5] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov253_RingSpreadTick);
}

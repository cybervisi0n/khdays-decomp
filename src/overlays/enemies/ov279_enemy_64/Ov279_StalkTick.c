/* Stalk tick of the ov279 enemy. Gives up (sub-state 2) once the +8 target
 * leaves the owner's scene, loses its +0x40 bit 1 or has bit 0 of its +0x60 low byte clear.
 * Otherwise the flattened unit direction from the +0x4c point to the target's +0x74, scaled by 1/8,
 * is the +0x30 step, the +0x5c bob phase advances (half sine, lifted 2.0, into +0x60, with the
 * +0x34 climb following the owner's +0x13c height) and the +0x1c facing aims at the target. An
 * attack chosen by Ov279_DecideAttackByDistanceRoll ends the tick; within 7.0 of the target (gap between the
 * collision radii) the tick hands over to Ov279_CircleTick, and beyond the owner's +0x2d8 leash
 * sub-state 2 is requested. */

#include "nitro/fx_types.h"

struct Bits40 { int b0 : 1, b1 : 1; };
struct hw60 { unsigned short lo : 8, hi : 8; };

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)
#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void Mtx33_LookAt(int *dst, const VecFx32 *a, const VecFx32 *b, const void *c);
extern void Quat_FromMtx33(void *dst, const int *src);
extern int Ov279_DecideAttackByDistanceRoll(int *node);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern const VecFx32 data_02042264;
extern void Ov279_CircleTick(int *node);

void Ov279_StalkTick(int *node)
{
    int target;
    int *state = (int *)node[1];
    int owner;
    VecFx32 d;
    int mtx[9];
    int t;
    int o;
    int gap;
    int height;
    int bob;

    owner = state[0];
    target = state[2];
    height = *(int *)(owner + 0x13c);
    if (*(int *)(target + 4) != *(int *)(owner + 4) || ((struct Bits40 *)(target + 0x40))->b1 == 0
        || (((struct hw60 *)(target + 0x60))->lo & 1) == 0) {
        *(unsigned char *)(owner + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    VEC_Subtract((void *)(target + 0x74), (void *)state[0x13], &d);
    t = state[2];
    o = state[0];
    gap = VEC_Normalize(&d, &d) - *(int *)(t + 0x80) - *(int *)(o + 0x80);
    ScaleVec3Fx12(0x200, &d, (VecFx32 *)(state + 0xc));
    state[0x17] += *(int *)(*node + 0x2c);
    if (state[0x17] > 0x2000) {
        state[0x17] -= 0x4000;
    }
    bob = data_0203d210[ANG2IDX(FX_MUL(state[0x17], 0x3244) / 2) * 2] / 2 + 0x4000;
    state[0x18] = bob;
    state[0xd] = height > bob ? -0x80 : 0x80;
    Mtx33_LookAt(mtx, (VecFx32 *)(target + 0x74), (VecFx32 *)state[0x13], &data_02042264);
    Quat_FromMtx33(state + 7, mtx);
    if (Ov279_DecideAttackByDistanceRoll(node) != 0) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    if (gap <= 0x7000) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov279_CircleTick);
        return;
    }
    if (gap <= *(int *)(*state + 0x2d8)) {
        return;
    }
    *(unsigned char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}

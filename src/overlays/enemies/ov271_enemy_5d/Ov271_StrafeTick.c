/* Strafe move of the ov271 enemy, twin of ov200 020cf508 (the handler Ov271_AiChooseMoveSteering dispatches on its d201). Without a
 * target sub-state 2 is requested. Otherwise the owner keeps aiming at the target (+0x94 matrix)
 * and moves sideways around it: the flattened gap between the two collision radii (floored at 0)
 * gives the approach factor FX_Inv(0x4000 - gap, 0x4000) clamped to +/-1.0; the +0xc velocity is
 * the forward vector scaled by -factor plus the side vector (the +0xa4 strafe sign crossed with the
 * direction to the target) scaled by 1 - |factor|, both at 0x280, and the owner sinks at 0x180.
 * When the +0x80 timer has run out a d101 is rolled, the timer is re-rolled in [+0x224, +0x228]
 * and a sub-state is requested: 6 when the roll is below 35 and all three +0x390 parts are clear, 7
 * when it is below 80 and the last two are, else 5 (2 instead of 6/7 while +0x1c4 bit 2 is set). Otherwise the chooser takes over again once
 * the +0x13c attack window drops below 2.0. */

#include "nitro/fx_types.h"

extern int Ov107_FindNearestObject(int obj, int kind);
extern void SetIndexedSlot(int self, int slot, void (*cb)(void));
extern void Mtx33_LookAt(int *dst, const VecFx32 *a, const VecFx32 *b, const void *c);
extern void Quat_FromMtx33(int *dst, const int *src);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *dst);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *unit);
extern int FX_Div(int num, int den);
extern void Vec3TransformViaTempMtx(VecFx32 *dst, const int *a, const void *b);
extern void VEC_CrossProduct(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *src, VecFx32 *dst);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, int *out);
extern int RandNextScaled();
extern int Ov271_IsField38NibbleZero(int slot);
extern void Ov271_AiChooseMoveSteering(void);
extern char data_02042264[];
extern char data_02042258[];

static inline void VEC_Set(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void Ov271_StrafeTick(int self)
{
    int target;
    int *owner;
    int *ctx;
    int tgt;
    int *own2;
    int gap;
    int fac;
    int base;
    int span;
    int roll;
    int lo = 0;
    int mtx[9];
    VecFx32 toTarget;
    VecFx32 up;
    VecFx32 side;
    VecFx32 dir;
    VecFx32 fwd;

    ctx = *(int **)(self + 4);
    owner = (int *)ctx[0];
    ctx[2] = Ov107_FindNearestObject((int)owner, 0);
    target = ctx[2];
    if (target == 0) {
        *(signed char *)(ctx[0] + 0x1c7) = 2;
        SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
        return;
    }

    Mtx33_LookAt(mtx, (const VecFx32 *)(target + 0x74), (const VecFx32 *)ctx[0x13], data_02042264);
    Quat_FromMtx33(&ctx[0x25], mtx);
    VEC_Subtract((const VecFx32 *)ctx[0x13], (const VecFx32 *)(target + 0x74), &toTarget);
    toTarget.y = 0;
    tgt = ctx[2];
    own2 = (int *)ctx[0];
    gap = VEC_Normalize(&toTarget, &toTarget);
    gap = (gap - *(int *)(tgt + 0x80)) - own2[0x20];
    if (gap < 0) {
        gap = 0;
    }
    fac = FX_Div(0x4000 - gap, 0x4000);
    if (fac < -0x1000) {
        fac = -0x1000;
    }
    if (fac > 0x1000) {
        fac = 0x1000;
    }
    Vec3TransformViaTempMtx(&fwd, &ctx[0x25], data_02042258);
    VEC_Set(&up, 0, ctx[0x29] << 12, 0);
    VEC_CrossProduct(&up, &toTarget, &side);
    ScaleVec3Fx12(0x1000 - (fac < 0 ? -fac : fac), &side, &side);
    ScaleVec3Fx12(-fac, &fwd, &dir);
    ScaleVec3Fx12(0x280, &side, &side);
    ScaleVec3Fx12(0x280, &dir, &dir);
    VEC_Add(&side, &dir, &ctx[3]);
    ctx[4] = -0x180;

    if (ctx[0x20] <= 0) {
        roll = RandNextScaled(0x65) + lo;
        base = *(int *)(ctx[0] + 0x224);
        span = *(int *)(ctx[0] + 0x228) - base;
        if (span < 0) {
            span = -span;
        }
        ctx[0x20] = base + RandNextScaled(span + 1);

        if (roll < 0x23
            && Ov271_IsField38NibbleZero(*(int *)(ctx[0] + 0x390))
            && Ov271_IsField38NibbleZero(*(int *)(ctx[0] + 0x394))
            && Ov271_IsField38NibbleZero(*(int *)(ctx[0] + 0x398))) {
            *(signed char *)(ctx[0] + 0x1c7) = (*(unsigned char *)(ctx[0] + 0x1c4) & 4) ? 2 : 6;
            SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
            return;
        }
        if (roll < 0x50
            && Ov271_IsField38NibbleZero(*(int *)(ctx[0] + 0x394))
            && Ov271_IsField38NibbleZero(*(int *)(ctx[0] + 0x398))) {
            *(signed char *)(ctx[0] + 0x1c7) = (*(unsigned char *)(ctx[0] + 0x1c4) & 4) ? 2 : 7;
            SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
            return;
        }
        *(signed char *)(ctx[0] + 0x1c7) = 5;
        SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
        return;
    }

    if (owner[0x4f] >= 0x2000) {
        return;
    }
    SetIndexedSlot(self, *(signed char *)(self + 0x20), Ov271_AiChooseMoveSteering);
}

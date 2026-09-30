/* Ov210_TickSwoop -- swoop tick of the ov210 enemy (x3 with ov211/ov282). The +0x40 parameter accumulates the
 * owner's rate; the target is re-acquired into +0x44 -- none hands the tick over to
 * Ov211_AiEnterIdle. The end point sits on the circle of radius +0x3c around the target's
 * +0x190 at the +0x48 angle, lifted to +0x24; the curve point is the cubic Hermite blend of the
 * +0x20 start, that end and the +8/+0x14 tangents at t = +0x40. The horizontal step from the
 * +0x2c point towards it, capped at 0.5, goes to the owner's +0xf0 and is taken off the +0x3c
 * radius (floored at 0). At t = 1.0 the step clears and the tick hands over to
 * Ov211_AiReturnToIdleWhenClear. */

#include "nitro/fx_types.h"

static inline int FX_MUL(int a, int b)
{
    return (int)(((long long)a * b + 0x800) >> 12);
}
#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

extern int Ov107_FindNearestObject(int owner, int *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Mag(const VecFx32 *v);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern const short data_0203d210[];
extern const VecFx32 data_02041dc8;
extern void Ov211_AiEnterIdle(int *node);
extern void Ov211_AiReturnToIdleWhenClear(int *node);

void Ov211_TickSwoop(int *node)
{
    int *state = (int *)node[1];
    int len;

    state[0x10] += *(int *)(*node + 0x2c);
    state[0x11] = Ov107_FindNearestObject(*state, &len);
    if (state[0x11] == 0) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov211_AiEnterIdle);
        return;
    }
    {
        VecFx32 end;
        VecFx32 acc;
        VecFx32 tmp;
        VecFx32 step;
        int t, t2, t3, t2x3;

        end.x = FX_MUL(data_0203d210[ANG2IDX(state[0x12]) * 2], state[0xf]);
        end.y = state[9];
        end.z = FX_MUL(data_0203d210[ANG2IDX(state[0x12]) * 2 + 1], state[0xf]);
        VEC_Add(&end, (VecFx32 *)(state[0x11] + 0x190), &end);
        t = state[0x10];
        t2 = FX_MUL(t, t);
        t3 = FX_MUL(t2, t);
        t2x3 = 3 * t2;
        ScaleVec3Fx12(2 * t3 - t2x3 + 0x1000, (VecFx32 *)(state + 8), &acc);
        {
            int h01 = -2 * t3;
            h01 += t2x3;
            ScaleVec3Fx12(h01, &end, &tmp);
        }
        VEC_Add(&acc, &tmp, &acc);
        ScaleVec3Fx12(t + (t3 - 2 * t2), (VecFx32 *)(state + 2), &tmp);
        VEC_Add(&acc, &tmp, &acc);
        ScaleVec3Fx12(t3 - t2, (VecFx32 *)(state + 5), &tmp);
        VEC_Add(&acc, &tmp, &acc);
        VEC_Subtract(&acc, (VecFx32 *)state[0xb], &step);
        step.y = 0;
        len = VEC_Mag(&step);
        if (len > 0x800) {
            VEC_Normalize(&step, &step);
            ScaleVec3Fx12(0x800, &step, &step);
            len = 0x800;
        }
        state[0xf] -= len;
        if (state[0xf] < 0) {
            state[0xf] = 0;
        }
        *(VecFx32 *)(*state + 0xf0) = step;
    }
    if (state[0x10] < 0x1000) {
        return;
    }
    *(VecFx32 *)(*state + 0xf0) = data_02041dc8;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov211_AiReturnToIdleWhenClear);
}

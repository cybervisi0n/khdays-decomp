/* Hover-bob tick of the ov175 enemy (and its byte-identical twins), the Ov178_HoverBobTick shape:
 * acquires a target (+0xc; none requests sub-state 2), steps 0x300 along the direction from
 * 020cca08 into +0x20 and zeroes the +0x24 climb; the +0x50 bob phase advances by 30 x dt and
 * wraps against the +0x54 period (re-rolled as 0x78 + 0..1). The wanted height is the target's
 * +0x78 plus 2.0 (minus the excess of the +0x44 target height over 2.0) or the fixed +0x18
 * height when +0x88 is set, plus half a sine of the phase scaled by 2.0; a tenth of the gap to
 * the +8 position's y, times 0.5, becomes the climb. Closer than 2.0 the actor requests
 * sub-state 2 and releases the slot. The bob index is the SDK macro verbatim (FX_RAD_TO_IDX)
 * and FX_SinIdx as table[(idx >> 4) << 1]. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern int Ov107_FindNearestObject(int obj, int b);
extern void SetIndexedSlot(int node, int slot, void *cb);
extern int Ov175_FaceTargetGetClearance(int node, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int func_02020400(int a, int b);
extern short data_0203d210[];

void Ov175_HoverBobTick(int node)
{
    int *state = *(int **)(node + 4);
    VecFx32 dir;
    int dist;
    int h;
    int t;
    int idx;
    int s;
    int diff;

    state[3] = Ov107_FindNearestObject(*state, 0);
    if (state[3] == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)(node + 0x20), 0);
        return;
    }
    dist = Ov175_FaceTargetGetClearance(node, &dir);
    ScaleVec3Fx12(0x240, &dir, (VecFx32 *)(state + 8));
    state[9] = 0;
    state[0x14] += *(int *)(*(int *)node + 0x2c) * 30;
    if (state[0x14] >= state[0x15] << 12) {
        state[0x14] = 0;
        state[0x15] = RandNextScaled(1) + 0x78;
    }
    if (state[0x22] == 0) {
        h = *(int *)(state[3] + 0x78) + 0x2000;
        if (state[0x11] > 0x2000) {
            h -= state[0x11] - 0x2000;
        }
    } else {
        h = state[6];
    }
    t = func_02020400((int)(((long long)state[0x14] * 0x6488 + 0x800) >> 12), state[0x15]);
    idx = (unsigned short)((0x28BE60DB9391LL * t + 0x80000000000LL) >> 44);          /* FX_RAD_TO_IDX */
    s = data_0203d210[(idx >> 4) << 1] / 2;                                            /* FX_SinIdx / 2 */
    diff = h + (int)(((long long)s * 0x1000 + 0x800) >> 12) - *(int *)(state[2] + 4);
    state[9] += (int)(((long long)(diff / 10) * 0x1800 + 0x800) >> 12);
    if (dist >= 0x2000) {
        return;
    }
    *(unsigned char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)(node + 0x20), 0);
}

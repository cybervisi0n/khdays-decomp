/* Bounce tick of the ov163 enemy (x3: ov163/164/165): when the actor's +0x3cc bit 0 is set the
 * state ends with sub-state 8; otherwise the +0x30 timer runs and, while the +0x17a bit-1 flag
 * is set and the timer has passed two frames, the +0x3c heading is reflected against the
 * actor's +0x114 surface normal (v - 2 (v.n) n on the reversed heading), re-normalised and the
 * +0x40/+0x30 counters cleared; the +0x18 velocity is the heading scaled by the +0x38 speed,
 * which then decays to 0x7c6/2000 of itself. */

#include "nitro/fx_types.h"

struct Bit0 { int b0 : 1; };
struct Flags17a { unsigned char b0 : 1, b1 : 1; };

extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);

void Ov164_BounceTick(int *node)
{
    int actor;
    int *state = (int *)node[1];
    VecFx32 back;
    VecFx32 reflected;

    if (((struct Bit0 *)(*state + 0x3cc))->b0) {
        *(unsigned char *)(*state + 0x1c7) = 8;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    state[0xc] += *(int *)(*node + 0x2c);
    actor = *state;
    if (((struct Flags17a *)(actor + 0x17a))->b1 && state[0xc] >= *(int *)(*node + 0x2c) * 2) {
        ScaleVec3Fx12(-0x1000, (VecFx32 *)(state + 0xf), &back);
        ScaleVec3Fx12(VEC_DotProduct(&back, (VecFx32 *)(actor + 0x114)) << 1, (VecFx32 *)(actor + 0x114), &reflected);
        VEC_Subtract(&reflected, &back, &reflected);
        VEC_Normalize(&reflected, (VecFx32 *)(state + 0xf));
        state[0x10] = 0;
        state[0xc] = 0;
    }
    ScaleVec3Fx12(state[0xe], (VecFx32 *)(state + 0xf), (VecFx32 *)(state + 6));
    state[0xe] = state[0xe] * 0x7c6 / 2000;
}

/* Volley tick of the ov283 actor: the +0x48 clock runs up at the frame rate and the +0x10 velocity
 * swings along the +0x38 heading turned a quarter back (0.3125 flat, then lifted 0.375); with both
 * volleys done (+0x68 >= 2) the actor recovers (020ce8c8). Each volley fires once the clock passes
 * its step (2 then 5 x 0x88): a launch (020cc9e0) that finds no free helper ends in 020ce918,
 * otherwise the volley counts; past 0x440 the clock resets and the actor recovers. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int v[2]; } Steps;
struct Ov283VolleyTmpl { u8 pairs[4]; Steps steps; };

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int Ov283_LaunchHelper(int *node);
extern void Ov283_AiLandWithItem(void);
extern void Ov283_AiLandBounce(void);
extern const short data_0203d210[];
extern const struct Ov283VolleyTmpl data_ov283_020cfb64;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov283_VolleyTick(int *node)
{
    int *state = (int *)node[1];
    Steps steps;
    VecFx32 lift;
    long long angle;    /* kept 64-bit: the index is derived from it twice (sin and cos) */

    angle = state[0xe] + 0x3244;
    steps = data_ov283_020cfb64.steps;
    state[0x12] += *(int *)(node[0] + 0x2c);
    state[4] = data_0203d210[ANG2IDX(angle) * 2];
    state[5] = 0;
    state[6] = data_0203d210[ANG2IDX(angle) * 2 + 1];
    ScaleVec3Fx12(0x500, (VecFx32 *)(state + 4), (VecFx32 *)(state + 4));
    lift.x = 0;
    lift.y = 0x600;
    lift.z = 0;
    VEC_Add((VecFx32 *)(state + 4), &lift, (VecFx32 *)(state + 4));
    if ((unsigned int)state[0x1a] >= 2) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov283_AiLandWithItem);
        return;
    }
    if (state[0x12] > steps.v[state[0x1a]] * 0x88) {
        if (Ov283_LaunchHelper(node) == 0) {
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov283_AiLandBounce);
            return;
        }
        state[0x1a]++;
    }
    if (state[0x12] <= 0x440) {
        return;
    }
    state[0x12] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov283_AiLandWithItem);
}

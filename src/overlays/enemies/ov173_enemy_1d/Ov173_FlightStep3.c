/* Flight step 3 of the ov173 enemy (x2: ov173/174): the +0x20 velocity takes the +0x2c aim
 * scaled by 0xf00 (the aim itself is rescaled in place); without a +0x88 override the +0x50
 * phase advances 30 per frame (wrapping at 0x28000) and the vertical speed follows a sine of
 * phase * 0x6488 / 40, halved and scaled by 0x200. Once the +0x58 counter is not positive
 * animation 9 plays and the state advances to Ov173_AiStep_QueueAction5OnAnimEnd. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern void Ov173_AiStep_QueueAction5OnAnimEnd(void);

void Ov173_FlightStep3(int *node)
{
    int *state = (int *)node[1];
    int t;
    int idx;
    int s;

    *(VecFx32 *)(state + 8) = *(VecFx32 *)(state + 0xb);
    ScaleVec3Fx12(0xf00, (VecFx32 *)(state + 0xb), (VecFx32 *)(state + 0xb));
    if (state[0x22] == 0) {
        state[0x14] += *(int *)(*node + 0x2c) * 30;
        if (state[0x14] >= 0x28000) {
            state[0x14] = 0;
        }
        t = (int)(((long long)state[0x14] * 0x6488 + 0x800) >> 12) / 40;
        idx = (unsigned short)((0x28BE60DB9391LL * t + 0x80000000000LL) >> 44);   /* FX_RAD_TO_IDX */
        s = data_0203d210[(idx >> 4) << 1] / 2;                                       /* FX_SinIdx / 2 */
        state[9] = (int)(((long long)s * 0x200 + 0x800) >> 12);
    }
    if (state[0x16] <= 0) {
        Ov107_PostTagUpdate((Actor *)(*state), 9, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov173_AiStep_QueueAction5OnAnimEnd);
    }
}

/* Walk tick of the ov256 actor: it re-picks its target (020ccd54), the +0x44 heading turns by 35
 * degrees in the +0x70 direction, the +0x10 velocity is the +0x450 owner's +0x2c vector turned by its
 * heading (020cd054) scaled by 1 + boost/8 (+0x45c). Each time the partner holds no queued move (or on
 * the second +0x17a flag, or in retreat mode 4) the +0x4c step clock grows. Once the step count +0x54
 * reaches 5-7, or on those conditions, the walk winds down by stance (+0x69): 0 ends with the +0x74
 * mode + 2, 1 turns into stance 2 with pose 4 / motion 2, 2 ends once the partner is idle. Otherwise
 * every two steps the count grows and the stance flips; the stance picks pose 1 / motion 0 or pose 3 /
 * motion 2. Codegen: the stance flip is `(u8)(++stance) % 2`; `(u8)(stance + 1) % 2` adds in place
 * instead of into the ROM's fresh r3. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

struct Flag17a { u8 b0 : 1; u8 b1 : 1; };

extern int Ov256_PickTarget(int *node);
extern void Ov256_RotateByActorHeading(int *out, int param_2, int *vec);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov256_WalkTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    Ov256_PickTarget(node);
    state[0x11] += *((signed char *)state + 0x70) * 0x1922;
    Ov256_RotateByActorHeading((int *)&v, (int)node, (int *)(*(int *)(*state + 0x450) + 0x2c));
    {
        VecFx32 *vel = (VecFx32 *)(state + 4);

        *vel = v;
        ScaleVec3Fx12((*(int *)(*state + 0x45c) << 9) + 0x1000, vel, vel);
    }
    if (*(u8 *)(state[1] + 0xad) != 0 && !((struct Flag17a *)(*state + 0x17a))->b1 &&
        *((u8 *)state + 0x6b) != 4) {
        return;
    }
    state[0x13]++;
    if ((unsigned int)state[0x15] >= (unsigned int)(RandNextScaled(3) + 5) ||
        ((struct Flag17a *)(*state + 0x17a))->b1 || *((u8 *)state + 0x6b) == 4) {
        if (*((u8 *)state + 0x69) != 0) {
            if (*((u8 *)state + 0x69) == 1) {
                *((u8 *)state + 0x69) = 2;
                Ov107_PostTagUpdate((Actor *)(*state), 4, 0);
                Ov107_StartAnim(*(int *)(*state + 0x450), 2, 0);
            } else if (*((u8 *)state + 0x69) == 2 && *(u8 *)(state[1] + 0xad) == 0) {
                *(signed char *)(*state + 0x1c7) = state[0x1d] + 2;
                SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
            }
        } else {
            *(signed char *)(*state + 0x1c7) = state[0x1d] + 2;
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        }
        return;
    }
    if (state[0x13] >= 2) {
        state[0x13] = 0;
        state[0x15]++;
        *((u8 *)state + 0x69) = (u8)(++*((u8 *)state + 0x69)) % 2;
        if (*((u8 *)state + 0x69) == 0) {
            Ov107_PostTagUpdate((Actor *)(*state), 1, 0);
            Ov107_StartAnim(*(int *)(*state + 0x450), 0, 0);
        } else {
            Ov107_PostTagUpdate((Actor *)(*state), 3, 0);
            Ov107_StartAnim(*(int *)(*state + 0x450), 2, 0);
        }
        return;
    }
    if (*((u8 *)state + 0x69) == 0) {
        Ov107_PostTagUpdate((Actor *)(*state), 1, 0);
        Ov107_StartAnim(*(int *)(*state + 0x450), 0, 0);
    } else {
        Ov107_PostTagUpdate((Actor *)(*state), 3, 0);
        Ov107_StartAnim(*(int *)(*state + 0x450), 2, 0);
    }
}

/* Ov245_GlideTick -- glide tick: on entry (+0x30 latch) clears bit 7 of the actor's +0x60
 * high byte, fires reaction 0x15a/0xe at the +8 anchor and seeds the +0xc velocity (y = the
 * +0x2c sink rate + 0.03125). While sinking with the actor's +0x17a bit 0 set: below a 0.03125
 * sink rate (and no +0xad scene flag) pose 1 is restored, the velocity zeroed, +0x24 cleared
 * and the node moved to 020d1b90; otherwise the +0x28 speed and +0x2c sink rate decay once per
 * 0x88 of the frame step and the sink rate becomes the y velocity. Otherwise gravity
 * (-0x80 * step / 0x88) accumulates in y. The horizontal velocity is the +0x18 direction
 * scaled by the speed. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Bit0 { unsigned char b0 : 1; };

extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int FX_Div(int num, int den);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern const VecFx32 data_02041dc8;
extern void Ov245_Thrown_AiTimeoutTick(void);

void Ov245_GlideTick(int *node) {
    int *state = (int *)node[1];
    VecFx32 v;
    int rest;
    int vy;

    if (*((unsigned char *)state + 0x30) == 0) {
        *((unsigned char *)state + 0x30) = 1;
        {
            u16 hw = *(u16 *)(*state + 0x60);
            *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
                (((unsigned int)(unsigned short)((((unsigned int)hw << 0x10) >> 0x18) & ~0x80) << 0x18) >> 0x10);
        }
        Ov107_BuildAndSendUpdate(*state, 0x15a, 0xe, (void *)state[2]);
        vy = state[0xb] + 0x80;
        state[3] = 0;
        state[4] = vy;
        state[5] = 0;
    }
    if (state[4] < 0 && ((struct Bit0 *)(*state + 0x17a))->b0) {
        if (state[0xb] < 0x80 && *(unsigned char *)(state[1] + 0xad) == 0) {
            Ov107_PostTagUpdate((Actor *)(*state), 1, 1);
            *(VecFx32 *)(state + 3) = data_02041dc8;
            state[9] = 0;
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov245_Thrown_AiTimeoutTick);
            return;
        }
        rest = *(int *)(node[0] + 0x2c);
        while (rest > 0) {
            int ratio = FX_Div(rest <= 0x88 ? rest : 0x88, 0x88);
            int t = (int)(((long long)ratio * 0x400 + 0x800) >> 12);
            state[10] = (int)(((long long)state[10] * (0x1000 - t) + 0x800) >> 12);
            ratio = FX_Div(rest <= 0x88 ? rest : 0x88, 0x88);
            t = (int)(((long long)ratio * 0x400 + 0x800) >> 12);
            state[0xb] = (int)(((long long)state[0xb] * (0x1000 - t) + 0x800) >> 12);
            rest -= 0x88;
        }
        state[4] = state[0xb];
    } else {
        state[4] += *(int *)(node[0] + 0x2c) * -0x80 / 0x88;
    }
    ScaleVec3Fx12(state[10], (VecFx32 *)(state + 6), &v);
    state[3] = v.x;
    state[5] = v.z;
}

/* Glide tick: the +0x44 timer accumulates the frame rate; the +0xc velocity is the +0x430
 * partner's +0x2c vector turned by the +0x30 yaw, and the +0x10 climb follows a quarter sine of
 * the +0x54 drop over the timer (clamped to 0..0x1540), relative to the +8 track's height above
 * the +0x50 start. At 0x440 (once, +0x70 bit 0) reaction 0x16d/0xf fires at the track and, in move
 * 5, the actor is knocked back at its feet (020cdbbc, side 1). Once the +4 item's +0xad byte
 * clears pose 5 / partner motion 4 (move 5, which also restarts the +0x460 helper with 5) or
 * pose 2 / motion 1 play, the timers and flags reset, +0x38 takes the +0x30 yaw, the +0x58 /
 * +0x5c scales return to 1.0 and the node moves on to 020cff0c. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int m[9]; } Mtx33;

extern void MTX_RotY33_(Mtx33 *m, int sin, int cos);
extern void MTX_MultVec33(const VecFx32 *v, Mtx33 *m, VecFx32 *d);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *d);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void Ov254_KnockbackAtFeet(int actor, int side);
extern void Ov107_PostTagUpdate(int actor, int pose, int loop);
extern void Ov107_StartAnim(int part, int motion, int mode);
extern void Ov254_ForwardToAiTaskWhenReady(int helper, int mode);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov254_CircleTick(void);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov254_GlideTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 m;
    int t;
    unsigned int idx;

    t = state[0x11] += *(int *)(node[0] + 0x2c);
    if (t > 0x1540) {
        t = 0x1540;
    } else if (t < 0) {
        t = 0;
    }
    idx = ANG2IDX(state[0xc]);
    MTX_RotY33_(&m, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
    MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x430) + 0x2c), &m, (VecFx32 *)(state + 3));
    ScaleVec3Fx12(0x1000, (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    state[4] = FX_Mul(data_0203d210[ANG2IDX(t * 0x1922 / 0x1540) * 2], state[0x15]) -
               (*(int *)(state[2] + 4) - state[0x14]);
    if ((*((u8 *)state + 0x70) & 1) == 0 && state[0x11] >= 0x440) {
        *((u8 *)state + 0x70) |= 1;
        Ov107_BuildAndSendUpdate(*state, 0x16d, 0xf, (void *)state[2]);
        if (*(signed char *)(*state + 0x100 + 0xc6) == 5) {
            Ov254_KnockbackAtFeet(*state, 1);
        }
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate(*state, *(signed char *)(*state + 0x100 + 0xc6) == 5 ? 5 : 2, 0);
    Ov107_StartAnim(*(int *)(*state + 0x430), *(signed char *)(*state + 0x100 + 0xc6) == 5 ? 4 : 1, 0);
    if (*(signed char *)(*state + 0x100 + 0xc6) == 5) {
        Ov254_ForwardToAiTaskWhenReady(*(int *)(*state + 0x460), 5);
    }
    state[0x11] = 0;
    state[0x12] = 0;
    state[0x10] = 0;
    state[0x13] = 0;
    state[0xe] = state[0xc];
    *((u8 *)state + 0x71) = 0;
    *((u8 *)state + 0x70) = 0;
    state[0x16] = 0x1000;
    state[0x17] = 0x1000;
    state[0x21] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov254_CircleTick);
}

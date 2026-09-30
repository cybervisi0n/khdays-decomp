/* Drop tick: the +0x44 timer accumulates the frame rate; the +0x10 climb eases along a quarter
 * sine of the +0x54 drop over the timer (clamped to 0..0x1540), relative to the +8 track's height
 * above the +0x50 start. At 0xaa0 the actor is knocked back once at its feet (020cdbbc, side 1)
 * and in place (mode 0xa, +0x70 bit 0). Once the +4 item's +0xad byte clears pose 0x1b plays,
 * the +0x3e4 shape loses bit 1, the +0x44 timer, +0x40, +0x58 and the +0x70 flags clear and the
 * node moves on to 020cf684. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { unsigned f : 8; } B8;

extern void Ov254_KnockbackAtFeet(int actor, int side);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov254_BarrageTick(void);
extern const short data_0203d210[];
extern const VecFx32 data_02041dc8;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov254_DropTick(int *node)
{
    int *state = (int *)node[1];
    int t;

    state[0x11] += *(int *)(node[0] + 0x2c);
    t = state[0x11] > 0x1540 ? 0x1540 : (state[0x11] < 0 ? 0 : state[0x11]);
    if ((*((u8 *)state + 0x70) & 1) == 0 && state[0x11] >= 0xaa0) {
        *((u8 *)state + 0x70) |= 1;
        Ov254_KnockbackAtFeet(*state, 1);
        func_ov107_020c0b90(*state, 0xa, data_02041dc8, 0);
    }
    state[4] = FX_Mul(data_0203d210[ANG2IDX(t * 0x1922 / 0x1540) * 2], state[0x15]) -
               (*(int *)(state[2] + 4) - state[0x14]);
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x1b, 0);
    ((B8 *)(*(int *)(*state + 0x3e4) + 8))->f &= ~2;
    state[0x11] = 0;
    state[0x10] = 0;
    state[0x16] = 0;
    *((u8 *)state + 0x70) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov254_BarrageTick);
}

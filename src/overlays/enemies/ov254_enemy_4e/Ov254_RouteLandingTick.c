/* Landing tick: the +0x10 climb is 1/32 of the height difference to the next route point. Once
 * the +8 track dips below the actor's +0x4d4 floor + 15.4 (not aggressive, +0x70 bit 0 clear) the
 * actor is knocked back once at its feet (020cdbbc, side -1). Once the +4 item's +0xad byte clears
 * the actor plays pose 8 (aggressive) or 0x11 (looping) and is knocked back in place (mode 5,
 * flag 1), the +0x3e4 shape loses bit 1 and the +0x3e0 one gains it, the +0x44 timer clears and
 * the node moves to 020d1384. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { unsigned f : 8; } B8;

extern int Ov254_PanelYForPhase(int *state, int a);
extern void Ov254_KnockbackAtFeet(int actor, int side);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;
extern void Ov254_AiSettleTick(void);

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov254_RouteLandingTick(int *node)
{
    int *state = (int *)node[1];

    state[4] = FX_Mul(Ov254_PanelYForPhase(state, -1) - *(int *)(state[2] + 4), 0x80);
    if ((*((u8 *)state + 0x70) & 1) == 0 && state[0x1e] == 0 &&
        *(int *)(state[2] + 4) < *(int *)(*state + 0x4d4) + 0xf662) {
        *((u8 *)state + 0x70) |= 1;
        Ov254_KnockbackAtFeet(*state, -1);
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), state[0x1e] != 0 ? 8 : 0x11, 1);
    func_ov107_020c0b90(*state, 5, data_02041dc8, 1);
    ((B8 *)(*(int *)(*state + 0x3e4) + 8))->f &= ~2;
    ((B8 *)(*(int *)(*state + 0x3e0) + 8))->f |= 2;
    state[0x11] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov254_AiSettleTick);
}

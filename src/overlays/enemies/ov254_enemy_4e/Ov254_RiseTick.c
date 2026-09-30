/* Rise tick: the +0x10 climb is zero until the +0x44 timer reaches 0x1dc0, then 1/32 of the height
 * difference to route point 4. At 0x2068 the actor is knocked back once at its feet (020cdbbc,
 * side 1) and in place (mode 0xa, +0x70 bit 0). Once the +4 item's +0xad byte clears, every shape
 * of the ten +0x4ac items' +0x22c lists loses bit 1, the +0x45c partner is resumed (020d2050) and
 * the next move is 4. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { unsigned f : 8; } B8;
struct Items4ac { char pad[0x4ac]; int item[10]; };

extern int Ov254_PanelYForPhase(int *state, int a);
extern void Ov254_KnockbackAtFeet(int actor, int side);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern int List_First(void *list);
extern void Ov254_ForwardToAiIfReady(int partner);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov254_RiseTick(int *node)
{
    int *state = (int *)node[1];
    int i;
    int shape;

    state[4] = 0;
    state[0x11] += *(int *)(node[0] + 0x2c);
    if (state[0x11] >= 0x1dc0) {
        state[4] = FX_Mul(Ov254_PanelYForPhase(state, 4) - *(int *)(state[2] + 4), 0x80);
    }
    if ((*((u8 *)state + 0x70) & 1) == 0 && state[0x11] >= 0x2068) {
        *((u8 *)state + 0x70) |= 1;
        Ov254_KnockbackAtFeet(*state, 1);
        func_ov107_020c0b90(*state, 0xa, data_02041dc8, 0);
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    for (i = 0; i < 10; i++) {
        for (shape = List_First((void *)(((struct Items4ac *)*state)->item[i] + 0x22c)); shape != 0;
             shape = List_Next((void *)(((struct Items4ac *)*state)->item[i] + 0x22c))) {
            ((B8 *)(shape + 8))->f &= ~2;
        }
    }
    Ov254_ForwardToAiIfReady(*(int *)(*state + 0x45c));
    *(signed char *)(*state + 0x1c7) = 4;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}

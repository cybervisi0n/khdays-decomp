/* Stop entry: the +0xc velocity resets, the actor plays pose 7 (aggressive, +0x78) or 0x10 and is
 * knocked back in place (mode 5). When not aggressive, every shape of the ten +0x4ac items' +0x22c
 * lists gains bit 1 and the +0x45c partner is released (020d206c). The +0x70 flag clears and the
 * node moves to 020d121c. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

typedef struct { unsigned f : 8; } B8;
struct Items4ac { char pad[0x4ac]; int item[10]; };

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern int List_First(void *list);
extern void Ov254_ForwardToAiIfReady_2(int partner);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;
extern void Ov254_RouteLandingTick(void);

void Ov254_StopEntry(int *node)
{
    int *state = (int *)node[1];
    VecFx32 zero = data_02041dc8;
    int i;
    int shape;

    *(VecFx32 *)(state + 3) = data_02041dc8;
    Ov107_PostTagUpdate((Actor *)(*state), state[0x1e] != 0 ? 7 : 0x10, 0);
    func_ov107_020c0b90(*state, 5, zero, 0);
    if (state[0x1e] == 0) {
        for (i = 0; i < 10; i++) {
            for (shape = List_First((void *)(((struct Items4ac *)*state)->item[i] + 0x22c)); shape != 0;
                 shape = List_Next((void *)(((struct Items4ac *)*state)->item[i] + 0x22c))) {
                ((B8 *)(shape + 8))->f |= 2;
            }
        }
        Ov254_ForwardToAiIfReady_2(*(int *)(*state + 0x45c));
    }
    *((u8 *)state + 0x70) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov254_RouteLandingTick);
}

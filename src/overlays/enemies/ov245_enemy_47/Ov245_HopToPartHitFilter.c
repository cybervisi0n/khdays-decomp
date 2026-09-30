/* Ov245_HopToPartHitFilter -- hit filter of the +0x214 sub-state: a hit with low bits 0 and 4, while
 * neither the +0x44 latch nor the +0x4c flag is set, picks a random one of the owner's three
 * +0x420 parts (walking on to the next while the part's +0x60 low bit is clear), plans a hop
 * (020d2bc4) to the part's +0xb0 position raised by its +0x80 height, scales the +0x18/+0x20
 * direction by the 64-bit +0x30 length into +0xc/+0x14, sets the +0x10 speed to 1.0, latches
 * +0x44 and clears +0x24. Returns 1 when handled.
 * Codegen: the part is read again from the owner's table inside the taken branch (a single `part`
 * read before the test swaps its register with the &v address). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

struct Ov245Owner { char pad[0x420]; int parts[3]; };
struct hw60 { unsigned short lo : 8, hi : 8; };

extern void Ov245_PlanHop(int *state, VecFx32 *target, int step);

int Ov245_HopToPartHitFilter(int self, int a, unsigned int *hit) {
    int *state = *(int **)(self + 0x214);
    VecFx32 v;
    int i;
    int j;

    if (((unsigned short)*hit & 1) != 0 && ((unsigned short)*hit & 0x10) != 0) {
        if (state[0x11] != 0 || state[0x13] != 0) {
            return 0;
        }
        i = RandNextScaled(3);
        for (j = 0; j < 3; j++) {
            if ((((struct hw60 *)(((struct Ov245Owner *)*(int *)(*state + 0x390))->parts[i] + 0x60))->lo & 1) != 0) {
                int part = ((struct Ov245Owner *)*(int *)(*state + 0x390))->parts[i];

                v = *(VecFx32 *)(part + 0xb0);
                v.y += *(int *)(part + 0x80);
                Ov245_PlanHop(state, &v, 0x1000);
                state[3] = (int)((*(s64 *)(state + 0xc) * state[6] + 0x80000000LL) >> 32);
                state[5] = (int)((*(s64 *)(state + 0xc) * state[8] + 0x80000000LL) >> 32);
                state[4] = 0x1000;
                break;
            }
            i = (i + 1) % 3;
        }
        state[0x11] = 1;
        state[9] = 0;
        return 1;
    }
    return 0;
}

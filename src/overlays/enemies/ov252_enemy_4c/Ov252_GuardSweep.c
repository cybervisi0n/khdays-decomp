/* Guard sweep of the ov252 actor: while its +0x60 guard flag and the +0x98 guard are both up, the
 * guard sphere (its +0x530 model's +0x14 point, radius 8.0) is tested twice against kind-6 targets
 * (020ce0a8); each sweep that has hit anything so far plays sound 0/0x51 at the +8 point, and the
 * +0x85 hit mask keeps only the targets that were hit. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 center; int nRadius; } Sphere;
typedef struct { u16 lo : 8; u16 hi : 8; } flags16;

extern u8 Ov252_ReboundHitTest(int *state, int kind, Sphere *sphere, void *cyl, void *box);
extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, void *at);

void Ov252_GuardSweep(int *node)
{
    int *state = (int *)node[1];
    u8 i;
    u8 hit;
    Sphere guard;

    if ((((flags16 *)(*state + 0x60))->lo & 1) == 0 || state[0x26] == 0) {
        return;
    }
    hit = 0;
    for (i = 0; i < 2; i++) {
        guard.center = *(VecFx32 *)(*(int *)(*state + 0x530) + 0x14);
        guard.nRadius = 0x8000;
        if ((hit |= Ov252_ReboundHitTest(state, 6, &guard, 0, 0)) != 0) {
            Ov107_BuildAndSendUpdate(*state, 0, 0x51, (void *)state[2]);
        }
    }
    *((u8 *)state + 0x85) &= hit;
}

/* Guarded drift tick of the ov252 actor: the guard sweep runs (020ce370) and it faces the target
 * (020cdfe8 0, 1); while the +0xac guard is up and the partner's +0xaf flag is clear poses 0x31 and
 * 0x35 play. The +0xc velocity follows the +0x574 part's +0x2c vector turned by the +0x54 heading;
 * once the partner holds no queued move the next move is 5 with the guard up, else 2. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov252_GuardSweep(int *node);
extern int Ov252_CheckTarget(int *node, VecFx32 *delta, int face);
extern VecFx32 Ov252_TurnVecY(int angle, VecFx32 *vec);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov252_GuardedDriftTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    Ov252_GuardSweep(node);
    Ov252_CheckTarget(node, 0, 1);
    if (state[0x2b] != 0 && *(unsigned char *)(state[1] + 0xaf) == 0) {
        Ov107_PostTagUpdate((Actor *)(*state), 0x31, 0);
        Ov107_PostTagUpdate((Actor *)(*state), 0x35, 0);
    }
    v = Ov252_TurnVecY(state[0x15], (VecFx32 *)(*(int *)(*state + 0x574) + 0x2c));
    *(VecFx32 *)(state + 3) = v;
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    if (state[0x2b] == 0) {
        *(unsigned char *)(*state + 0x1c7) = 2;
    } else {
        *(unsigned char *)(*state + 0x1c7) = 5;
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}

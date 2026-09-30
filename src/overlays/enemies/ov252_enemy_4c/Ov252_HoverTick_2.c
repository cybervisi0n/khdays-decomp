/* Hover tick of the ov252 actor: the guard sweep runs and the +0xc velocity follows the +0x574 part's
 * +0x2c vector turned by the +0x54 heading. On a +0x88 lift cue (3) it faces the target and picks the
 * +0x92 attack (0x10 when the target is below 10.0, else 2 with a 30 % roll, else 0), +0x88 = 1, +0x64
 * clears and the heading advances. With the +0xac guard up there is no attack, and a guard cue (8)
 * with the partner's +0xaf flag clear plays poses 0x33/0x37 and sets a 28.0 lift. Once the partner
 * holds no queued move: unguarded, +0x7c is three frames and with no lift height the next move is 7;
 * otherwise (guarded: +0x64 clears, +0x7c half a frame) +0xbc/+0x70/+0xb8 clear, pose 2 plays and the
 * node moves on to 020d1c7c. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern void Ov252_GuardSweep(int *node);
extern VecFx32 Ov252_TurnVecY(int angle, VecFx32 *vec);
extern int Ov252_CheckTarget(int *node, VecFx32 *delta, int face);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_CruiseTick(void);

void Ov252_HoverTick_2(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    Ov252_GuardSweep(node);
    v = Ov252_TurnVecY(state[0x15], (VecFx32 *)(*(int *)(*state + 0x574) + 0x2c));
    *(VecFx32 *)(state + 3) = v;
    if (*((unsigned char *)state + 0x88) == 3) {
        Ov252_CheckTarget(node, 0, 1);
        *((unsigned char *)state + 0x92) = *(int *)(*(int *)(*state + 0x4e4) + 0x194) < 0xa000 ? 0x10 :
            (RandNextScaled(0x64) < 0x1e ? 2 : 0);
        *((unsigned char *)state + 0x88) = 1;
        state[0x19] = 0;
        state[0x15] += 0x3244;
    }
    if (state[0x2b] != 0) {
        *((unsigned char *)state + 0x92) = 0;
        if (*(unsigned char *)(state[1] + 0xaf) == 0 && *((unsigned char *)state + 0x88) == 8) {
            *((unsigned char *)state + 0x88) = 0;
            Ov107_PostTagUpdate((Actor *)(*state), 0x33, 1);
            Ov107_PostTagUpdate((Actor *)(*state), 0x37, 1);
            state[0x1e] = 0x1c000;
        }
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    if (state[0x2b] == 0) {
        state[0x1f] = *(int *)(node[0] + 0x2c) * 3;
        if (state[0x1e] == 0) {
            *(unsigned char *)(*state + 0x1c7) = 7;
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
            return;
        }
    }
    if (state[0x2b] != 0) {
        state[0x19] = 0;
        state[0x1f] = *(int *)(node[0] + 0x2c) / 2;
    }
    state[0x2f] = 0;
    state[0x1c] = 0;
    state[0x2e] = 0;
    Ov107_PostTagUpdate((Actor *)(*state), 2, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_CruiseTick);
}

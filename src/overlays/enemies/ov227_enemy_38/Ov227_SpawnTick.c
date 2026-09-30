/* Spawn tick of the ov227 enemy: every 0x2a8 of the +0x5c timer the first of the owner's ten +0x3ec
 * members without a +0x388 item is placed 1.0 above the +0x3ac body's +0x20 point (counting the
 * spawns at +0x60). While the +4 item is busy (+0xad) nothing else happens; otherwise animation
 * 0x19 plays, or after ten spawns animation 0x1a and the tick hands over to Ov227_AiStep_QueueAction2OnAnimEnd. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Ov227Family { char pad[0x3ec]; char *aMembers[10]; };

extern void Ov227_PlaceAt(char *obj, VecFx32 pos);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov227_AiStep_QueueAction2OnAnimEnd(int *node);

void Ov227_SpawnTick(int *node)
{
    int *state = (int *)node[1];

    state[0x17] += *(int *)(*node + 0x2c);
    if (state[0x17] >= 0x2a8) {
        int i;
        struct Ov227Family *owner = (struct Ov227Family *)*state;
        VecFx32 pos = *(VecFx32 *)(**(int **)((char *)owner + 0x3ac) + 0x20);

        pos.y += 0x1000;
        for (i = 0; i < 0xa; i++) {
            if (owner->aMembers[i] != 0 && *(int *)(owner->aMembers[i] + 0x388) == 0) {
                Ov227_PlaceAt(owner->aMembers[i], pos);
                break;
            }
        }
        state[0x17] = 0;
        state[0x18]++;
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    if (state[0x18] < 0xa) {
        Ov107_PostTagUpdate((Actor *)(*state), 0x19, 0);
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x1a, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov227_AiStep_QueueAction2OnAnimEnd);
}

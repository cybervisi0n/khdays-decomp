/* Shed tick of the ov252 actor: the +0xc velocity follows the +0x574 part's +0x2c vector turned by the
 * +0x54 heading; once the partner holds no queued move the +0x93 armour piece's shape (+0x4f4) shows,
 * the next move is 5 and the node ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { unsigned f : 8; } B8;
struct Ov252Armour { char pad[0x4e8]; int shapes[16]; };

extern VecFx32 Ov252_TurnVecY(int angle, VecFx32 *vec);
extern void SetIndexedSlot(int *node, int slot, void *cb);

void Ov252_ShedTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    v = Ov252_TurnVecY(state[0x15], (VecFx32 *)(*(int *)(*state + 0x574) + 0x2c));
    *(VecFx32 *)(state + 3) = v;
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    ((B8 *)(((struct Ov252Armour *)*state)->shapes[*((u8 *)state + 0x93) + 3] + 8))->f |= 1;
    *(u8 *)(*state + 0x1c7) = 5;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}

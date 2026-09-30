/* Release of the ov237 actor's grab: the actor's +0x4c4 point takes the +0x38 point, +0x58 and the
 * actor's +0x494 clear, pose 4 plays, bit 1 of the +0x488 rig's +8 flags is set and bit 6 of the
 * +0x60 high byte cleared; the release sound (0x12d variant 0xf) plays at the +0x38 point and the
 * brain waits on 020d039c. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { unsigned f : 8; } B8;

extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, int at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov237_AiLoop5OnAnimEnd(void);

void Ov237_ReleaseGrab(int *node)
{
    int *state = (int *)node[1];

    *(VecFx32 *)(*state + 0x4c4) = *(VecFx32 *)state[0xe];
    state[0x16] = 0;
    *(int *)(*state + 0x494) = 0;
    Ov107_PostTagUpdate((Actor *)(*state), 4, 0);
    ((B8 *)(*(int *)(*state + 0x488) + 8))->f |= 2;
    {
        u16 hw = *(u16 *)(state + 0x18);

        *(u16 *)(state + 0x18) = (hw & ~0xff00) |
            (((unsigned int)(unsigned short)((((unsigned int)hw << 0x10) >> 0x18) & ~0x40) << 0x18) >> 0x10);
    }
    Ov107_BuildAndSendUpdate(*state, 0x12d, 0xf, state[0xe]);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov237_AiLoop5OnAnimEnd);
}

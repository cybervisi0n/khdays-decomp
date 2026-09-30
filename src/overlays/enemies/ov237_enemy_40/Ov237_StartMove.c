/* Start move of the ov237 actor: the actor's +0x494 and +0x5c clear. Alone (no +0x4ac partner) it
 * appears at the +0x38 point offset by data_ov237_020d1bb8 turned toward the partner rig (020cdb50),
 * marks +0x4b0; either way bit 0 of the +0x60 high byte is set and bits 1, 2, 6 and 7 cleared, bit 0 of
 * +0x1ae cleared and bit 0 of the +0x488 rig's +8 flags set (a linked partner takes the actor's
 * health). +0x58 / +0x60 / +0x64 are set, the +0x28 timer rolls between the +0x224 and +0x228
 * bounds and the brain waits on 020ce584. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

typedef struct { unsigned f : 8; } B8;

extern VecFx32 Ov237_RotateByActorHeading(int *node, VecFx32 *target);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov237_AiShareHpAndQueue2(void);
extern const VecFx32 data_ov237_020d1bb8;

void Ov237_StartMove(int *node)
{
    int *state = (int *)node[1];
    VecFx32 pos;
    VecFx32 off;

    *(int *)(*state + 0x494) = 0;
    state[0x17] = 0;
    if (*(int *)(*state + 0x4ac) == 0) {
        pos = *(VecFx32 *)state[0xe];
        off = data_ov237_020d1bb8;
        off = Ov237_RotateByActorHeading(node, &off);
        VEC_Add(&pos, &off, &pos);
        *(int *)(*state + 0x4b0) = 1;
        Ov107_MoveNodeAndRelayout((Actor *)(*state), &pos);
        {
            u16 hw = *(u16 *)(*state + 0x60);

            *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
                ((((((unsigned int)hw << 0x10) >> 0x18) | 1) << 0x18) >> 0x10);
        }
        {
            u16 hw = *(u16 *)(*state + 0x60);

            *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
                (((unsigned int)(unsigned short)((((unsigned int)hw << 0x10) >> 0x18) & ~0xc6) << 0x18) >> 0x10);
        }
        *(u16 *)(*state + 0x1ae) &= ~1;
        ((B8 *)(*(int *)(*state + 0x488) + 8))->f |= 1;
    } else {
        {
            u16 hw = *(u16 *)(*state + 0x60);

            *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
                ((((((unsigned int)hw << 0x10) >> 0x18) | 1) << 0x18) >> 0x10);
        }
        {
            u16 hw = *(u16 *)(*state + 0x60);

            *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
                (((unsigned int)(unsigned short)((((unsigned int)hw << 0x10) >> 0x18) & ~0xc6) << 0x18) >> 0x10);
        }
        *(u16 *)(*state + 0x1ae) &= ~1;
        ((B8 *)(*(int *)(*state + 0x488) + 8))->f |= 1;
        *(short *)(*(int *)(*state + 0x4a4) + 0x21a) = *(short *)(*state + 0x21a);
    }
    state[0x18] = 1;
    state[0x19] = 1;
    state[0x16] = 1;
    {
        int lo = *(int *)(*state + 0x224);
        int span = *(int *)(*state + 0x228) - lo;

        if (span < 0) {
            span = -span;
        }
        state[0xa] = lo + RandNextScaled(span + 1);
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov237_AiShareHpAndQueue2);
}

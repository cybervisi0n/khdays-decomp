/* Fall tick of an ov256 part: the +0x10 velocity is the +0x450 owner's +0x2c vector turned by its
 * heading (020cd054); once the partner holds no queued move and it has landed (+0x17a bit 0) +0x4c
 * clears, pose 0x1b plays, it is knocked back at the +0xc point (mode 9), the +0x450 part takes motion
 * 0xc and the node moves on to 020d0144. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Flag17a { u8 b0 : 1; };

extern void Ov256_RotateByActorHeading(int *out, int param_2, int *vec);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_PartDriftTick(void);

void Ov256_TickFall(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    Ov256_RotateByActorHeading((int *)&v, (int)node, (int *)(*(int *)(*state + 0x450) + 0x2c));
    *(VecFx32 *)(state + 4) = v;
    if (*(u8 *)(state[1] + 0xad) == 0) {
        if (!((struct Flag17a *)(*state + 0x17a))->b0) {
            return;
        }
        state[0x13] = 0;
        Ov107_PostTagUpdate((Actor *)(*state), 0x1b, 0);
        func_ov107_020c0b90(*state, 9, *(VecFx32 *)state[3], 0);
        Ov107_StartAnim(*(int *)(*state + 0x450), 0xc, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_PartDriftTick);
        return;
    }
}

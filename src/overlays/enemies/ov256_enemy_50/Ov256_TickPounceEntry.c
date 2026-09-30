/* Pounce entry tick of the ov256 actor: without a target (020ccd54) the node ends at once; otherwise
 * the +0x10 velocity is the +0x450 owner's +0x2c vector turned by its heading (020cd054) and, once the
 * partner holds no queued move, +0x4c clears, pose 0x13 plays, the +0x450 part takes motion 5, it is
 * knocked back at the +0xc point (mode 0xe) and the node moves on to 020cf238. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov256_PickTarget(int *node);
extern void Ov256_RotateByActorHeading(int *out, int param_2, int *vec);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_ChaseTick(void);

void Ov256_TickPounceEntry(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    if (Ov256_PickTarget(node) == 0) {
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    Ov256_RotateByActorHeading((int *)&v, (int)node, (int *)(*(int *)(*state + 0x450) + 0x2c));
    *(VecFx32 *)(state + 4) = v;
    if (*(unsigned char *)(state[1] + 0xad) == 0) {
        state[0x13] = 0;
        Ov107_PostTagUpdate((Actor *)(*state), 0x13, 0);
        Ov107_StartAnim(*(int *)(*state + 0x450), 5, 0);
        func_ov107_020c0b90(*state, 0xe, *(VecFx32 *)state[3], 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_ChaseTick);
        return;
    }
}

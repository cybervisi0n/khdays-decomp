/* Brace tick of the ov256 actor: the +0x10 velocity is the +0x450 owner's +0x2c vector turned by its
 * heading (020cd054); once the partner holds no queued move pose 0x15 plays, the +0x450 part takes
 * motion 7, the actor is knocked back at the origin (mode 0xf), +0x4c clears and the node moves on to
 * 020cf474. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Ov256_RotateByActorHeading(int *out, int param_2, int *vec);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov256_LungeTick(void);
extern const VecFx32 data_02041dc8;

void Ov256_TickBrace(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    Ov256_RotateByActorHeading((int *)&v, (int)node, (int *)(*(int *)(*state + 0x450) + 0x2c));
    *(VecFx32 *)(state + 4) = v;
    if (*(unsigned char *)(state[1] + 0xad) == 0) {
        Ov107_PostTagUpdate((Actor *)(*state), 0x15, 0);
        Ov107_StartAnim(*(int *)(*state + 0x450), 7, 0);
        func_ov107_020c0b90(*state, 0xf, data_02041dc8, 0);
        state[0x13] = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_LungeTick);
        return;
    }
}

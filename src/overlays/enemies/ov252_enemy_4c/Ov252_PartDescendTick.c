/* Descend tick of an ov252 part: it faces the target (020cdfe8 0, 1) and +0x6c accumulates the frame
 * rate; a pending +0x88 start plays pose 0x20, at 1.43 the +0x89 cue at 5 steps down and sound 0x148/0xf
 * plays at the +8 point; once the partner holds no queued move the owner plays effect 0x10 at the
 * origin, +0x64, +0x86 and +0x88 clear and the node moves on to 020d2944. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int Ov252_CheckTarget(int *node, VecFx32 *delta, int face);
extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, void *at);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_PartSlamTick(void);
extern const VecFx32 data_02041dc8;

void Ov252_PartDescendTick(int *node)
{
    int *state = (int *)node[1];

    Ov252_CheckTarget(node, 0, 1);
    state[0x1b] += *(int *)(node[0] + 0x2c);
    if (*((unsigned char *)state + 0x88) != 0) {
        *((unsigned char *)state + 0x88) -= 1;
        Ov107_PostTagUpdate((Actor *)(*state), 0x20, 0);
    }
    if (state[0x1b] >= 0x16d8 && *((unsigned char *)state + 0x89) == 5) {
        *((unsigned char *)state + 0x89) -= 1;
        Ov107_BuildAndSendUpdate(*state, 0x148, 0xf, (void *)state[2]);
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    func_ov107_020c0b90(*state, 0x10, data_02041dc8, 0);
    state[0x19] = 0;
    *((unsigned char *)state + 0x86) = 0;
    *((unsigned char *)state + 0x88) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_PartSlamTick);
}

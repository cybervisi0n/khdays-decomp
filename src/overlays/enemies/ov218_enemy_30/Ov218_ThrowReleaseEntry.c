/* Throw release entry of the ov218 actor: the +0x28 velocity takes the +0x34 drift, which decays to
 * 0.69; once the partner holds no queued move pose 6 loops, effect 1 fires at the origin, the +0x14
 * timer starts at 75.0 and is scaled by 1.5 per throw out (+0x24), +0x40 clears and the node moves on
 * to 020cda78. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov218_ThrowTick(void);
extern const VecFx32 data_02041dc8;

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

void Ov218_ThrowReleaseEntry(int *node)
{
    int *state = (int *)node[1];
    int i;

    *(VecFx32 *)(state + 0xa) = *(VecFx32 *)(state + 0xd);
    ScaleVec3Fx12(0xb00, (VecFx32 *)(state + 0xd), (VecFx32 *)(state + 0xd));
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 6, 1);
    func_ov107_020c0b90(*state, 1, data_02041dc8, 0);
    state[5] = 0x4b000;
    for (i = 0; i < state[9]; i++) {
        state[5] = FX_MUL(state[5], 0x1800);
    }
    *((unsigned char *)state + 0x40) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov218_ThrowTick);
}

/* Shed drift tick of the ov252 actor: the +0xc velocity follows the +0x574 part's +0x2c vector turned
 * by the +0x54 heading and scaled by +0x70 + 0.5, with the part's +0x30 height; once the partner holds
 * no queued move pose 0x14 plays, the part takes motion 0xd, the owner plays effect 4 at the origin,
 * +0x88, +0x8c and +0x64 clear and the node moves on to 020d0630. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern VecFx32 Ov252_TurnVecY(int angle, VecFx32 *vec);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_ShedDriftTick(void);
extern const VecFx32 data_02041dc8;

void Ov252_TickShedDrift(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    v = Ov252_TurnVecY(state[0x15], (VecFx32 *)(*(int *)(*state + 0x574) + 0x2c));
    *(VecFx32 *)(state + 3) = v;
    ScaleVec3Fx12(state[0x1c] + 0x800, (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    state[4] = *(int *)(*(int *)(*state + 0x574) + 0x30);
    if (*(unsigned char *)(state[1] + 0xad) == 0) {
        Ov107_PostTagUpdate((Actor *)(*state), 0x14, 0);
        Ov107_StartAnim(*(int *)(*state + 0x574), 0xd, 0);
        func_ov107_020c0b90(*state, 4, data_02041dc8, 1);
        *((unsigned char *)state + 0x88) = 0;
        *((unsigned char *)state + 0x8c) = 0;
        state[0x19] = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_ShedDriftTick);
        return;
    }
}

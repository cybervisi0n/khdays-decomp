/* Drift tick of the ov252 actor: the +0xc velocity follows the +0x574 part's +0x2c vector turned by the
 * +0x54 heading; once the partner holds no queued move, when the landing spot is taken (020cdc78) pose
 * 0xf plays, the part takes motion 0x14 and the node moves on to 020d0b44, else pose 0xe and motion 0x13
 * restart. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern VecFx32 Ov252_TurnVecY(int angle, VecFx32 *vec);
extern int Ov252_GroundCheck(int *node);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_TickLand(void);

void Ov252_DriftLandTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    v = Ov252_TurnVecY(state[0x15], (VecFx32 *)(*(int *)(*state + 0x574) + 0x2c));
    *(VecFx32 *)(state + 3) = v;
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    if (Ov252_GroundCheck(node) != 0) {
        Ov107_PostTagUpdate((Actor *)(*state), 0xf, 0);
        Ov107_StartAnim(*(int *)(*state + 0x574), 0x14, 0);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_TickLand);
    } else {
        Ov107_PostTagUpdate((Actor *)(*state), 0xe, 0);
        Ov107_StartAnim(*(int *)(*state + 0x574), 0x13, 0);
    }
}

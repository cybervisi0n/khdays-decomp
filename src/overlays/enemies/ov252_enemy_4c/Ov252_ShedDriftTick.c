/* Shed drift tick of the ov252 actor: the +0xc velocity follows the +0x574 part's +0x2c vector turned
 * by the +0x54 heading and scaled by +0x70 + 0.5 (height from the part's +0x30); +0x64 accumulates the
 * frame rate and after 0.83 the drop cue (+0x88) re-arms once (+0x8c). With the cue armed, a 50 % roll
 * drops a reward (020d056c): a drop counts in +0x60 and disarms the cue, no pieces left ends the drift.
 * Once the partner holds no queued move the drift ends after 12 drops, beyond 25.0 from the origin or
 * with a +0xa0 reward pending (pose 0x15, motion 0xe, effect 4 mode 2, node to 020d0854); otherwise
 * pose 0x14, motion 0xd and effect 4 mode 1 restart it. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern VecFx32 Ov252_TurnVecY(int angle, VecFx32 *vec);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern int Ov252_DropReward(int *node, int param);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov252_HoverTickB(void);
extern const VecFx32 data_02041dc8;

static inline void VecSet(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void Ov252_ShedDriftTick(int *node)
{
    int *state = (int *)node[1];
    VecFx32 d;
    VecFx32 v;
    int dist;

    v = Ov252_TurnVecY(state[0x15], (VecFx32 *)(*(int *)(*state + 0x574) + 0x2c));
    *(VecFx32 *)(state + 3) = v;
    ScaleVec3Fx12(state[0x1c] + 0x800, (VecFx32 *)(state + 3), (VecFx32 *)(state + 3));
    state[4] = *(int *)(*(int *)(*state + 0x574) + 0x30);
    state[0x19] += *(int *)(node[0] + 0x2c);
    if (state[0x19] >= 0xd48 && *((unsigned char *)state + 0x8c) == 0) {
        *((unsigned char *)state + 0x88) = 0;
        *((unsigned char *)state + 0x8c) = 1;
    }
    if (*((unsigned char *)state + 0x88) == 0 && RandNextScaled(0x64) < 0x32) {
        switch (Ov252_DropReward(node, 2)) {
        case 0:
            break;
        case 1:
            state[0x18]++;
            *((unsigned char *)state + 0x88) = 1;
            break;
        case 2:
            goto finish;
        }
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    VecSet(&d, -((VecFx32 *)state[2])->x, 0, -((VecFx32 *)state[2])->z);
    dist = VEC_Normalize(&d, &d);
    if (state[0x18] >= 0xc || dist > 0x19000 || state[0x28] != 0) {
finish:
        Ov107_PostTagUpdate((Actor *)(*state), 0x15, 0);
        Ov107_StartAnim(*(int *)(*state + 0x574), 0xe, 0);
        func_ov107_020c0b90(*state, 4, data_02041dc8, 2);
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov252_HoverTickB);
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x14, 0);
    Ov107_StartAnim(*(int *)(*state + 0x574), 0xd, 0);
    func_ov107_020c0b90(*state, 4, data_02041dc8, 1);
    state[0x19] = 0;
    *((unsigned char *)state + 0x8c) = 0;
}

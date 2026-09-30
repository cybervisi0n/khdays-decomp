/* Swipe entry of the ov260 actor: with a +8 target in front of it (the +0x64 heading against the
 * direction to the target, dot product >= 0) pose 4 plays, otherwise pose 5; effect 0x1b starts at
 * the +0x10 point and the node moves on to 020d06ec. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *out);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern void Ov260_PlaySound(int owner, int mode, int arg);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov260_HurtTick(void);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov260_SwipeEntry(int *node)
{
    int *state = (int *)node[1];
    int front = 0;
    VecFx32 facing;
    VecFx32 d;

    if (state[2] != 0) {
        int idx = ANG2IDX(state[0x19]) * 2;

        facing.x = data_0203d210[idx];
        facing.y = 0;
        facing.z = data_0203d210[idx + 1];
        VEC_Subtract((VecFx32 *)(state[2] + 0x74), (VecFx32 *)(*state + 0x74), &d);
        VEC_Normalize(&d, &d);
        if (VEC_DotProduct(&facing, &d) >= 0) {
            front = 1;
        }
    }
    Ov107_PostTagUpdate((Actor *)(*state), front ? 4 : 5, 0);
    Ov260_PlaySound(*state, 0x1b, state[4]);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_HurtTick);
}

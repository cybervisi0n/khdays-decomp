/* Slam landing of the ov225 enemy. The point 3.04 above the owner (turned by its +0xa0 pose
 * from the +8 point) is taken and the +0x14 leap clears. With a +0x10 target still held the
 * +0x5c timer clears and the aim tick (ov225 2908) runs again; otherwise the point is sent to
 * the owner as mode 5, reaction 0x14b mode 0x12 fires there, sub-state 2 is requested and the
 * action ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *c);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern const VecFx32 data_02041dc8;
extern void Ov225_SlamAimTick(int *node);

void Ov225_SlamLanding(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;

    v.x = 0;
    v.y = 0x30a4;
    v.z = 0;
    Vec3TransformViaTempMtx(&v, (const void *)(*state + 0xa0), &v);
    VEC_Add(&v, (VecFx32 *)state[2], &v);
    *(VecFx32 *)(state + 5) = data_02041dc8;
    if (state[4] != 0) {
        state[0x17] = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov225_SlamAimTick);
        return;
    }
    func_ov107_020c0b90(*state, 5, v, 0);
    Ov107_BuildAndSendUpdate(*state, 0x14b, 0x12, &v);
    *(u8 *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}

/* Slam entry of the ov225 enemy. The point 3.04 above the owner (turned by its +0xa0 pose
 * from the +8 point) is sent to the owner as mode 5 and reaction 0x14b mode 0x12 fires there;
 * bits 1 and 7 of the +0x60 high byte and bit 0 of +0x1ae are raised, the +0x5c timer clears
 * and the tick hands over to Ov225_SlamAimTick. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *c);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov225_SlamAimTick(int *node);

void Ov225_EnterSlam(int *node)
{
    int *state = (int *)node[1];
    VecFx32 v;
    u16 flags;

    v.x = 0;
    v.y = 0x30a4;
    v.z = 0;
    Vec3TransformViaTempMtx(&v, (const void *)(*state + 0xa0), &v);
    VEC_Add(&v, (VecFx32 *)state[2], &v);
    func_ov107_020c0b90(*state, 5, v, 0);
    Ov107_BuildAndSendUpdate(*state, 0x14b, 0x12, &v);
    flags = *(u16 *)(*state + 0x60);
    *(u16 *)(*state + 0x60) = (u16)((flags & ~0xff00) | (((((unsigned int)flags << 0x10) >> 0x18 | 2) << 0x18) >> 0x10));
    *(u16 *)(*state + 0x1ae) |= 1;
    flags = *(u16 *)(*state + 0x60);
    *(u16 *)(*state + 0x60) = (u16)((flags & ~0xff00) | (((((unsigned int)flags << 0x10) >> 0x18 | 0x80) << 0x18) >> 0x10));
    state[0x17] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov225_SlamAimTick);
}

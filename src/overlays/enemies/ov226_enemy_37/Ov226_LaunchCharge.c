/* Charge launch of the ov226 enemy: clears bit 7 and raises bit 0 of the owner's +0x60 high
 * byte, raises bit 0 of the +0x388 sub-item's +8 low byte, copies the +8 point to +0xc, clears
 * +0x30 and rolls the +0x38 (0..0xffff), +0x34 (0x200..0x300), +0x3c (0x10000..0x20000) and
 * +0x40 (0x200..0x700) spans. A 1.0 cast from the +8 point towards the +0x390 pool's +0xb0
 * point that hits sends the point raised 1.49 to the owner as mode 0, requests sub-state 0 and
 * ends the action; a clear cast hands the tick over to Ov226_FlightTick. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

struct w8 { unsigned int lo : 8, rest : 24; };
struct hw60 { unsigned short lo : 8, hi : 8; };

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Collision_CastSphere(void *collision, void *origin, VecFx32 *dir, int radius);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 v, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov226_FlightTick(int *node);

void Ov226_LaunchCharge(int *node)
{
    int *state = (int *)node[1];
    VecFx32 dir;
    VecFx32 at;
    u16 flags;
    char *coll;

    ((struct hw60 *)(*state + 0x60))->hi &= ~0x80;
    flags = *(u16 *)(*state + 0x60);
    *(u16 *)(*state + 0x60) = (u16)((flags & ~0xff00) | (((((unsigned int)flags << 0x10) >> 0x18 | 1) << 0x18) >> 0x10));
    ((struct w8 *)(*(int *)(*state + 0x388) + 8))->lo |= 1;
    *(VecFx32 *)(state + 3) = *(VecFx32 *)state[2];
    state[0xc] = 0;
    state[0xe] = RandNextScaled(0x10000);
    state[0xd] = RandNextScaled(0x101) + 0x200;
    state[0xf] = RandNextScaled(0x10001) + 0x10000;
    state[0x10] = RandNextScaled(0x501) + 0x200;
    coll = *(char **)(*state + 4);
    VEC_Subtract((VecFx32 *)state[2], (VecFx32 *)(*(int *)(*state + 0x390) + 0xb0), &dir);
    if (Collision_CastSphere(*(void **)(coll + 0x7c), (void *)(*(int *)(*state + 0x390) + 0xb0), &dir, 0x1000) != 0) {
        at = *(VecFx32 *)state[2];
        at.y += 0x17cc;
        func_ov107_020c0b90(*state, 0, at, 0);
        *(u8 *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
        return;
    }
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov226_FlightTick);
}

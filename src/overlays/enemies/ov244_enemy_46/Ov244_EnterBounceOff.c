/* Bounce-off entry: clears the actor's +0x390 word and sets bit 0 of +0x1ae; the +0x34 velocity
 * is -32.0 along the +4 pose's data_02042270 axis, the +0x40 one is 32.0 along data_02042240,
 * the +0x1c anchor's position is kept at +0x4c, the +0x30 range is 1/8 of the distance from the
 * anchor to the +0x384 item's +0x3bc target's +0x14 point, +0x2c clears and the node moves to
 * 020ce09c. */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Mag(const VecFx32 *v);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042240;
extern void Ov244_LeapToCoreTick(void);

void Ov244_EnterBounceOff(int *node) {
    int *state = (int *)node[1];
    VecFx32 d;

    *(int *)(*state + 0x390) = 0;
    *(unsigned short *)(*state + 0x100 + 0xae) |= 1;
    Vec3TransformViaTempMtx((VecFx32 *)(state + 0xd), state + 1, &data_02042270);
    ScaleVec3Fx12(-0x20000, (VecFx32 *)(state + 0xd), (VecFx32 *)(state + 0xd));
    *(VecFx32 *)(state + 0x10) = data_02042240;
    ScaleVec3Fx12(0x20000, (VecFx32 *)(state + 0x10), (VecFx32 *)(state + 0x10));
    *(VecFx32 *)(state + 0x13) = *(VecFx32 *)state[7];
    VEC_Subtract((VecFx32 *)(*(int *)(*(int *)(*state + 0x384) + 0x3bc) + 0x14), (VecFx32 *)state[7], &d);
    state[0xc] = VEC_Mag(&d) / 8;
    state[0xb] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov244_LeapToCoreTick);
}

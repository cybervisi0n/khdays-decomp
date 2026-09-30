/* Ov160_EnterDash -- c634 handler: arm the owner for a dash/lunge and hand off to
 * Ov160_DashTick.
 *
 * Zeroes the object's +0x28 distance, sets bit 0 of the owner's +0x60 config high byte and
 * then clears bits 0x8c of it, sets bit 0 of the byte at *(owner+0x388)+8, sets obj[9] =
 * 0x1000 as the length, builds the +0x14 rotation from data_02042258 to the owner's +0x394
 * basis (ed60), turns data_02042258 by it into obj[2..4] (f384), scales that by the length
 * and dispatches the next state.
 *
 * The +0x60 half-word is a bitfield (unsigned short lo:8, hi:8); the |= 1 edit is spelled out
 * because the bitfield form adds a truncation the ROM does not have here. */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct hw60 { unsigned short lo:8, hi:8; };
struct b8 { unsigned int b:8; };
struct quat { int q[4]; };
extern void Quat_FromTwoVectors(struct quat *out, const VecFx32 *from, void *basis);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int self, int index, void *cb);
extern void Ov160_DashTick(void);
extern const VecFx32 data_02042258;
void Ov160_EnterDash(int self) {
    int *obj = *(int **)(self + 4);
    obj[10] = 0;
    {
        unsigned short v = *(unsigned short *)(*obj + 0x60);
        *(unsigned short *)(*obj + 0x60) =
            (unsigned short)((v & ~0xff00) | (((((unsigned int)v << 0x10) >> 0x18 | 1) << 0x18) >> 0x10));
    }
    ((struct hw60 *)(*obj + 0x60))->hi &= ~0x8c;
    ((struct b8 *)(*(int *)(*obj + 0x388) + 8))->b |= 1;
    obj[9] = 0x1000;
    Quat_FromTwoVectors((struct quat *)(obj + 5), &data_02042258, (void *)(*obj + 0x394));
    Vec3TransformViaTempMtx((VecFx32 *)(obj + 2), (VecFx32 *)(obj + 5), (void *)&data_02042258);
    ScaleVec3Fx12(obj[9], (VecFx32 *)(obj + 2), (VecFx32 *)(obj + 2));
    SetIndexedSlot(self, *(signed char *)(self + 0x20), &Ov160_DashTick);
}

/* Ov137_ArmDashAndDispatch -- c634 handler: arm the owner for a dash/lunge and hand off to Ov137_SweepStrikeTick.
 *
 * Zeroes the object's two counters (obj[9], obj[10]), sets bit 0 of the owner's +0x60
 * config high byte and then clears bits 0x8c of it, sets bit 0 of the byte at
 * *(owner+0x388)+8, copies the owner's facing vector (owner+0x394) into obj[5..7] with
 * obj[8] = 0x1000 as the scale, runs ScaleVec3Fx12 to scale that vector into obj[2..4],
 * and dispatches the next state.
 *
 * The +0x60 half-word is a bitfield (unsigned short lo:8, hi:8) -- see
 * Ov181_BeginSteerSweep for the full read-modify-write shape. The |= 1 edit is spelled out
 * because the bitfield form adds a truncation the ROM does not have here (mwcc can prove
 * `hi | 1` still fits 8 bits, but not `hi & ~0x8c`); both spellings are byte-identical
 * where they apply. */

#include "nitro/fx_types.h"

struct hw60 { unsigned short lo:8, hi:8; };
struct b8 { unsigned int b:8; };
extern void ScaleVec3Fx12(int scale, int *v, unsigned int *out);
extern void SetIndexedSlot(int self, int index, void *cb);
extern void Ov137_SweepStrikeTick(void);
void Ov137_ArmDashAndDispatch(int self) {
    int *obj = *(int **)(self + 4);
    int *dst = obj + 5;
    obj[9] = 0;
    obj[10] = 0;
    {
        unsigned short v = *(unsigned short *)(*obj + 0x60);
        *(unsigned short *)(*obj + 0x60) =
            (unsigned short)((v & ~0xff00) | (((((unsigned int)v << 0x10) >> 0x18 | 1) << 0x18) >> 0x10));
    }
    ((struct hw60 *)(*obj + 0x60))->hi &= ~0x8c;
    ((struct b8 *)(*(int *)(*obj + 0x388) + 8))->b |= 1;
    *(VecFx32 *)(obj + 5) = *(VecFx32 *)(*obj + 0x394);
    ScaleVec3Fx12(obj[8] = 0x1000, dst, (unsigned int *)(obj + 2));
    SetIndexedSlot(self, *(signed char *)(self + 0x20), &Ov137_SweepStrikeTick);
}

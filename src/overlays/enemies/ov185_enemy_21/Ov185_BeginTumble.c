/* Kick off the tumble: pick a random axis, derive a spin from it, and arm the timer.
 *
 * Sets bit 1 and 2 of the owner's hw60 high byte, arms the countdown at ctx[0xa] from the
 * template's +0x2c, then rolls three independent components in [-0x1000, +0x1000], keeps
 * them as a vector at ctx+0x78 and normalises it.  Two cross products against the constant
 * at data_02042264 turn that axis into a perpendicular spin at ctx+0x84, which is then
 * scaled by a random magnitude in [0x500, 0xa00].
 *
 * THREE CODEGEN POINTS, and the first two are a matched pair worth remembering together:
 *  - `hi |= 6` on the hw60 halfword needs the EXPLICIT extract/reassemble spelling; the
 *    `struct hw60` bitfield adds an lsl#0x10/lsr#0x10 truncation pair and costs 8 bytes.
 *    `hi &= ~x` is the opposite -- the bitfield is what that one wants.  Both forms appear
 *    within a few functions of each other in this overlay, so neither is "the" spelling.
 *  - the three rolls are held in LOCALS and stored afterwards.  Assigning each directly
 *    into the vector emits the stores interleaved with the calls; the ROM keeps all three
 *    live and stores them together, which only happens with intermediates.
 *  - those locals must be declared with `z` before `y` (the assignment order), or the two
 *    callee-saved registers come out swapped.
 */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct hw60 { unsigned short lo : 8, hi : 8; };

extern int VEC_Normalize(const VecFx32 *v, VecFx32 *unit);
extern void VEC_CrossProduct(const VecFx32 *a, const VecFx32 *b, VecFx32 *dst);
extern void ScaleVec3Fx12(int t, const VecFx32 *a, VecFx32 *b);
extern void SetIndexedSlot(int *self, int action, void *cb);
extern VecFx32 data_02042264;
extern void Ov185_Orbit_Step(void);

void Ov185_BeginTumble(int *self) {
    int *ctx = (int *)self[1];
    VecFx32 *axis = (VecFx32 *)((char *)ctx + 0x78);
    VecFx32 *spin = (VecFx32 *)((char *)ctx + 0x84);
    int z, y, x;

    {
        unsigned short hw60 = *(unsigned short *)(*ctx + 0x60);
        *(unsigned short *)(*ctx + 0x60) =
            (hw60 & ~0xff00) | (((((unsigned int)hw60 << 0x10) >> 0x18 | 6) << 0x18) >> 0x10);
    }
    ctx[0xa] = *(int *)(self[0] + 0x2c) * 0x1e;

    z = RandNextScaled(0x2001) - 0x1000;
    y = RandNextScaled(0x2001) - 0x1000;
    x = RandNextScaled(0x2001) - 0x1000;
    axis->x = x;
    axis->y = y;
    axis->z = z;
    VEC_Normalize(axis, axis);

    VEC_CrossProduct(axis, &data_02042264, spin);
    VEC_CrossProduct(axis, spin, spin);
    ScaleVec3Fx12(RandNextScaled(0x501) + 0x500, spin, spin);

    ctx[0x18] = 0;
    ctx[0x24] = *(int *)(ctx[0x1d] + 0x18c);
    ctx[0xe] = 0;
    SetIndexedSlot(self, *(signed char *)((char *)self + 0x20), &Ov185_Orbit_Step);
}

/* Ov232_Item_AiEnterCharge -- acquire a target and arm the state, or give up if there is none.
 *
 * Ov107_FindNearestObject(ctx[0], 0) picks the target. With none, ctx[0]+0x1c7 is cleared and the
 * object re-enters with no callback. With one, the squared distance from ctx[0]+0xb0 to
 * target+0x190 is cached at +0x14, the two counters at +0x1c/+0x20 are zeroed, the flags are
 * updated and effect 5 fires before handing off to Ov232_Item_AiChargeTick.
 *
 * ★ The two hw60 writes here are the catalog's discriminator sitting back to back in one
 * function, and they need OPPOSITE C forms (see codegen-cracks.md):
 *   - `hi |= 1` has NO `lsl#0x10 ; lsr#0x10` trunc pair in the ROM -> explicit extract/reassemble,
 *     with `v & ~0xff00` (bic) for the lo-byte keep;
 *   - `hi &= ~0x8c` DOES have the trunc pair -> the bitfield form.
 * Reading the pair off the disassembly is the whole trick; do not guess from the operator. */

#include "nitro/fx_types.h"

typedef struct {
    unsigned short lo : 8;
    unsigned short hi : 8;
} Hw60;

extern int Ov107_FindNearestObject(int owner, int a);
extern void SetIndexedSlot(int self, int action, void *cb);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *a, const VecFx32 *b);
extern void Ov107_BuildAndSendUpdate(int owner, int a, int b, int ptr);
extern void Ov232_Item_AiChargeTick(void);

void Ov232_Item_AiEnterCharge(int self) {
    int *ctx;
    VecFx32 d;
    int target;
    unsigned short v;

    ctx = *(int **)(self + 4);
    target = Ov107_FindNearestObject(ctx[0], 0);
    if (target == 0) {
        *(signed char *)(ctx[0] + 0x1c7) = 0;
        SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
        return;
    }

    VEC_Subtract((const VecFx32 *)(target + 0x190), (const VecFx32 *)(ctx[0] + 0xb0), &d);
    ctx[5] = VEC_Normalize(&d, &d);
    ctx[8] = 0;
    ctx[7] = 0;

    v = *(unsigned short *)(ctx[0] + 0x60);
    *(unsigned short *)(ctx[0] + 0x60) =
        (unsigned short)((v & ~0xff00)
                         | (((((unsigned int)v << 0x10) >> 0x18 | 1) << 0x18) >> 0x10));

    ((Hw60 *)(ctx[0] + 0x60))->hi &= ~0x8c;

    *(unsigned char *)((char *)ctx + 0x26) = 0;
    Ov107_BuildAndSendUpdate(ctx[0], *(short *)((char *)ctx + 0x24), 5, ctx[1]);
    SetIndexedSlot(self, *(signed char *)(self + 0x20), Ov232_Item_AiChargeTick);
}

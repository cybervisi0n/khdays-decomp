/* Ov212_Stop -- stop: kick anim 1, zero the emitter vector, queue move 0 and dispatch.
 *
 * One of a 3-member shape family; the twins live in ov266/ov267 and are byte-identical modulo
 * relocs (matched here, fanned out with dedupprop).
 *
 * The constant data_02041dc8 is passed BY VALUE, which is what produces the ROM's
 * `sub r3, sp, #8 ; ldm ; stm ; ldm r3, {r2,r3}` shuffle: a 12-byte struct as the 3rd argument goes
 * half in r2/r3 and half in the outgoing stack slot, and mwcc materialises it straddling sp so the
 * last word lands exactly where the ABI wants it. Storing it into a temp first instead would change
 * the schedule. See codegen-cracks.md. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void func_ov107_020c0b90(int obj, int kind, VecFx32 v, int flag);
extern void SetIndexedSlot(int self, int slot, void *cb);
extern VecFx32 data_02041dc8;

void Ov212_Stop(int self) {
    int *ctx;

    ctx = *(int **)(self + 4);
    Ov107_PostTagUpdate((Actor *)ctx[0], 1, 0);
    func_ov107_020c0b90(ctx[0], 0xc, data_02041dc8, 0);
    *(signed char *)(ctx[0] + 0x1c7) = 0;
    SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
}

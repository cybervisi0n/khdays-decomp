/* Ov225_Projectile_Fire -- retune, notify, and (in mode 1) start the homing move.
 * VARIADIC -- the stmdb {r0,r1,r2,r3} prologue is the tell. Its own varargs are three VecFx32 by
 * value: the first is handed to Ov107_MoveNodeAndRelayout as a va_list, and the second and third are
 * forwarded BY VALUE to Ov225_Projectile_SetupFlight (itself variadic).
 *
 * Bit 1 of +0x40 is a SIGNED bitfield (the ROM extracts it with asr, not lsr) and gates the
 * owner's own notify hook (+0xc), which is loaded only if that bit is set. The rig at +0x384 is
 * reset when there is one, +0x38c is raised, and unless +0x50 is exactly 1 that is where it stops.
 *
 * The two forwarded VecFx32 explain the tail. 020d43a0 takes (ctx, mode, VecFx32, VecFx32): the first VecFx32
 * goes in r2/r3 plus the outgoing slot at sp+0, so the SECOND lands at sp+4..0xc -- which is why
 * the frame is 0x10 and why the second is stored before the first.
 *
 * ★ THE VA_START LEVER (this is what unparked it, and it generalises).
 * The last residue was `mov r4,r1` vs `ldr r4,[sp,#0x24]`: taking `&mode` for the va_list marks
 * the parameter ADDRESS-TAKEN, so mwcc treats the variadic prologue's spilled copy as canonical
 * and reads the mode back from memory instead of keeping the incoming register. The fix is not to
 * fight the read -- it is to take the address of a parameter nobody uses. Declaring the FIRST
 * VARARG as a named parameter (`int rest`) and basing the va_list on `&rest` gives exactly the
 * same address (the block start + 8, i.e. the ROM's `add r1,sp,#0x28`), costs nothing because
 * `rest` is never read, and leaves `mode` in r1. Its sibling Ov225_Projectile_SetupFlight has the same
 * residue from the same cause.
 *
 * Three more knobs are load-bearing and must be kept:
 * 1. The apparent dead local at sp+4 is 020d43a0's SECOND by-value VecFx32. Reading it as a local
 *    costs 28 B and mwcc drops the store as dead.
 * 2. Bit 1 of +0x40 is a SIGNED bitfield -- `int b:1`, giving asrs. As unsigned you get lsrs.
 * 3. The hook must be captured INSIDE the condition (`(notify = *(...)) != 0`): as two separate
 *    expressions mwcc loads +0xc twice, once predicated for the test and again for the call.
 */

#include "nitro/fx_types.h"

#define va_start(ap, last) ((ap) = (char *)(&(last) + 1))
/* Reach a vararg by its offset without keeping a live va_list. Using a held `ap` for these two
 * makes mwcc park it in a callee-saved register (add r4,sp,#0x28 ; add r0,r4,#0xc) and spill the
 * mode; folding each one to its own frame offset gives the ROM's `add r0,sp,#0x34`. */
#define va_at(last, off) (*(VecFx32 *)((char *)&(last) + (off)))

extern void Ov107_MoveNodeAndRelayout(int self, char *ap);
extern void RefreshObjectCallbacks(int rig, int a);
extern void Ov225_Projectile_SetupFlight(int ctx, int mode, VecFx32 a, VecFx32 b);

typedef struct {
    char pad[0x40];
    int b40_0 : 1;
    int b40_1 : 1;
    int b40_rest : 30;
} Ov221_Self;

void Ov225_Projectile_Fire(int self, int mode, int rest, ...) {
    int m;
    void (*notify)(int, int);

    m = mode;
    Ov107_MoveNodeAndRelayout(self, (char *)&rest);

    /* The hook is captured INSIDE the condition on purpose: as two separate expressions mwcc
     * loads +0xc twice (once predicated for the test, once again for the call). */
    if (((Ov221_Self *)self)->b40_1 && (notify = *(void (**)(int, int))(self + 0xc)) != 0) {
        notify(self, 0);
    }
    if (*(int *)(self + 0x384) != 0) {
        RefreshObjectCallbacks(*(int *)(self + 0x384), 0);
    }
    *(int *)(self + 0x38c) = 1;
    if (*(int *)(self + 0x50) != 1) {
        return;
    }

    Ov225_Projectile_SetupFlight(*(int *)(self + 0x214), m, va_at(rest, 0xc), va_at(rest, 0x18));
}

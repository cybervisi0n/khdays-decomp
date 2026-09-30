/* Ov280_AiIdleTick -- the hover tick: track the reference point, nudge it by speed, and pick an
 * exit.
 *
 * The tracked point (Ov280_rotateVecByOwnerYaw against *(ctx[0]+0x388)+0x2c) lands at ctx+0x30, then a
 * pure-Z offset is added on top, keyed off the speed at +0x20:
 *   under 0x800 -> -0x100 (pull in) · over 0xc000 -> +0x200 (push out) · between -> 0.
 * That offset goes through Ov280_rotateVecByOwnerYaw as well (in-place: out and ref are the same
 * vector) before VEC_Add folds it onto ctx+0x30.
 *
 * The gate at *ctx[4] holds the state. Once it clears there are three ways out, in order:
 *   - a 1-in-5 roll while the speed is under 0x5000: park 2 at +0x49, clear +0x24 and play move
 *     0xb;
 *   - with both +0x24 and +0x58 idle, Ov280_ChooseAttack gets a say and can end it outright;
 *   - otherwise move 4 -- unless +0x58 is set, in which case it is cleared and modes 1 fire on
 *     both the owner and *(ctx[0]+0x388).
 *
 * The vector at sp+0xc is zeroed in full before its Z is overwritten; that is what the ROM does
 * (the address escapes into Ov280_rotateVecByOwnerYaw, so the stores are not dead). */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern void Ov280_AcquireTarget(int self);
extern void Ov280_rotateVecByOwnerYaw(VecFx32 *out, int self, const VecFx32 *ref);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void SetIndexedSlot(int self, int action, void *cb);
extern int Ov280_ChooseAttack(int self);

void Ov280_AiIdleTick(int self) {
    /* ctx is initialised at its declaration, ahead of offset's, because the ROM computes it
     * before the vector is zeroed -- an assignment after the declarations puts the zeroing first.
     *
     * offset/pt are declared in reverse of their stack order: pt at sp+0, offset at sp+0xc. And
     * the aggregate initialiser is what makes mwcc materialise offset's address into a register
     * and store through it (`add r2,sp,#0xc ; str r1,[r2] ; str r1,[r2,#4] ; str r1,[r2,#8]`);
     * three plain field assignments store against sp and come out one instruction short. */
    int *ctx = *(int **)(self + 4);
    VecFx32 offset = {0, 0, 0};
    VecFx32 pt;
    int z;

    Ov280_AcquireTarget(self);
    Ov280_rotateVecByOwnerYaw(&pt, self, (const VecFx32 *)(*(int *)(ctx[0] + 0x388) + 0x2c));
    *(VecFx32 *)((char *)ctx + 0x30) = pt;

    if (ctx[8] < 0x800) {
        z = -0x100;
    } else if (ctx[8] > 0xc000) {
        z = 0x200;
    } else {
        z = 0;
    }
    offset.z = z;
    Ov280_rotateVecByOwnerYaw(&offset, self, &offset);
    VEC_Add((const VecFx32 *)((char *)ctx + 0x30), &offset, (VecFx32 *)((char *)ctx + 0x30));

    if (*(unsigned char *)ctx[4] != 0) {
        return;
    }

    if (RandNextScaled(5) == 0 && ctx[8] < 0x5000) {
        *(unsigned char *)((char *)ctx + 0x49) = 2;
        ctx[9] = 0;
        *(signed char *)(ctx[0] + 0x1c7) = 0xb;
        SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
        return;
    }

    if (ctx[9] == 0 && ctx[0x16] == 0) {
        if (Ov280_ChooseAttack(self) != 0) {
            SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
            return;
        }
    }

    if (ctx[0x16] == 0) {
        *(signed char *)(ctx[0] + 0x1c7) = 4;
        SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
        return;
    }

    ctx[0x16] = 0;
    Ov107_PostTagUpdate((Actor *)ctx[0], 1, 0);
    Ov107_StartAnim(*(int *)(ctx[0] + 0x388), 1, 0);
}

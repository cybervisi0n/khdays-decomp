/* Ov201_AiChooseMoveSteering -- ov201's move CHOOSER, the steering variant.
 *
 * Byte-identical to Ov200_AiChooseMoveSteering modulo this overlay's own symbols; that is the rep and
 * carries the analysis. See Ov208_AiChooseAttack for the plain chooser shape.
 *
 * A dispatcher reads the queued move at ctx[0]+0x1c7 and runs it; a chooser decides what to queue.
 * This one also STEERS every frame before deciding: it aims a matrix at the target, turns the
 * distance into a Q12 approach factor, and writes a velocity into ctx[3..5].
 *
 * Flow:
 *   no target                    -> queue move 2
 *   gap  = |target - self| minus both collision radii (+0x80 each), floored at 0
 *   fac  = FX_Inv(0x4000 - gap, 0x4000) clamped to [-0x1000, 0x1000]   (1.0 = 0x1000, so this is
 *          "how much of full speed", ramping up as the gap closes below 0x4000)
 *   velocity = the aimed forward vector scaled by -fac, then by 0x280
 *   gap > *(ctx[0]+0x2d8)        -> out of leash: queue move 2
 *   ctx[0]+0x13c == 0x7fffffff   -> no attack window: just drift ctx[4] by +/-0x200 toward the
 *                                   target's height and return
 *   ctx[4] = 0x180
 *   ctx[0]+0x13c <= 0x3800       -> return (window too small)
 *   ctx[0x29] = a coin flip: 1 or -1  (the strafe direction)
 *   ctx[0x20] > 0                -> a d201: 0 queues move 5, anything else dispatches
 *                                   Ov201_StrafeMove as the handler instead of queueing
 *   otherwise                    -> burn one d100 roll, then pick a new ctx[0x20] in
 *                                   [+0x224, +0x224 + |+0x228 - +0x224|], then check three slots
 *                                   via Ov201_IsField38NibbleZero: all three clear -> move 6; else if the
 *                                   last two are clear -> move 7; else nothing.
 *
 * The discarded `RandNextScaled(0x65)` is real -- the ROM calls it and drops the result, advancing
 * the RNG.
 *
 * `+ (v - v)` on the RNG results is NOT a typo and must not be "simplified": RandNextScaled returns
 * long long, and that unfoldable zero is what makes mwcc emit the ROM's `add/adds r0, r0, #0`.
 * See deferred-ties.md.
 *
 * Three spellings here look gratuitous and are load-bearing -- do not tidy them:
 *   - `ctx[2] = f(); target = ctx[2];` rather than the other way round, so the store reads the
 *     call's own return register.
 *   - the `!= 0 ? -1 : 1` ternary rather than `== 0 ? 1 : -1`, which fixes the mvnne/moveq order.
 *   - the guards on +0x13c and ctx[0x20] are inverted so their bodies land at the END of the
 *     function, matching the ROM's out-of-line block layout.
 * The two extra locals read from ctx[2]/ctx[0] before the VEC_Normalize call are also deliberate:
 * the ROM keeps those two base pointers in callee-saved registers across it.
 *
 * FX_Inv is the reloc's own name for 01ff8a04, but it takes two arguments and divides -- it is
 * FX_Div in NitroSDK terms. The name is kept because the symbol table says so.
 */

#include "nitro/fx_types.h"

extern int Ov107_FindNearestObject(int obj, int kind);
extern void SetIndexedSlot(int self, int slot, void (*cb)(void));

extern void Mtx33_LookAt(int *dst, const VecFx32 *a, const VecFx32 *b, const void *c);
extern void Quat_FromMtx33(int *dst, const int *src);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *dst);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *unit);
extern int FX_Div(int num, int den);
extern void Vec3TransformViaTempMtx(VecFx32 *dst, const int *a, const void *b);
extern void ScaleVec3Fx12(int scale, const VecFx32 *src, int *dst);
extern int RandNextScaled();
extern int Ov201_IsField38NibbleZero(int slot);
extern void Ov201_StrafeMove(void);
extern char data_02042264[];
extern char data_02042258[];

void Ov201_AiChooseMoveSteering(int self) {
    int target;
    int *owner;
    int *ctx;
    int tgt;
    int *own2;
    int gap;
    int fac;
    int base;
    int span;
    int mtx[9];
    VecFx32 toTarget;
    VecFx32 dir;

    ctx = *(int **)(self + 4);
    owner = (int *)ctx[0];
    ctx[2] = Ov107_FindNearestObject((int)owner, 0);
    target = ctx[2];
    if (target == 0) {
        *(signed char *)(ctx[0] + 0x1c7) = 2;
        SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
        return;
    }

    Mtx33_LookAt(mtx, (const VecFx32 *)(target + 0x74), (const VecFx32 *)ctx[0x13], data_02042264);
    Quat_FromMtx33(&ctx[0x25], mtx);
    VEC_Subtract((const VecFx32 *)ctx[0x13], (const VecFx32 *)(target + 0x74), &toTarget);
    tgt = ctx[2];
    own2 = (int *)ctx[0];
    gap = VEC_Normalize(&toTarget, &toTarget);
    gap = (gap - *(int *)(tgt + 0x80)) - own2[0x20];
    if (gap < 0) {
        gap = 0;
    }
    fac = FX_Div(0x4000 - gap, 0x4000);
    if (fac < -0x1000) {
        fac = -0x1000;
    }
    if (fac > 0x1000) {
        fac = 0x1000;
    }
    Vec3TransformViaTempMtx(&dir, &ctx[0x25], data_02042258);
    ScaleVec3Fx12(-fac, &dir, &ctx[3]);
    ScaleVec3Fx12(0x280, (const VecFx32 *)&ctx[3], &ctx[3]);

    if (gap > *(int *)(ctx[0] + 0x2d8)) {
        *(signed char *)(ctx[0] + 0x1c7) = 2;
        SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
        return;
    }

    if (owner[0x4f] != 0x7fffffff) {
        ctx[4] = 0x180;
        if (owner[0x4f] <= 0x3800) {
            return;
        }

        ctx[0x29] = (RandNextScaled(2) + (gap - gap) != 0) ? -1 : 1;

        if (ctx[0x20] <= 0) {
            RandNextScaled(0x65);
            base = *(int *)(ctx[0] + 0x224);
            span = *(int *)(ctx[0] + 0x228) - base;
            if (span < 0) {
                span = -span;
            }
            ctx[0x20] = base + RandNextScaled(span + 1);

            if (Ov201_IsField38NibbleZero(*(int *)(ctx[0] + 0x390))
                && Ov201_IsField38NibbleZero(*(int *)(ctx[0] + 0x394))
                && Ov201_IsField38NibbleZero(*(int *)(ctx[0] + 0x398))) {
                *(signed char *)(ctx[0] + 0x1c7) = 6;
                SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
                return;
            }
            if (!Ov201_IsField38NibbleZero(*(int *)(ctx[0] + 0x394))) {
                return;
            }
            if (!Ov201_IsField38NibbleZero(*(int *)(ctx[0] + 0x398))) {
                return;
            }
            *(signed char *)(ctx[0] + 0x1c7) = 7;
            SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
            return;
        }

        if (RandNextScaled(0xc9) + (gap - gap) == 0) {
            *(signed char *)(ctx[0] + 0x1c7) = 5;
            SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
            return;
        }
        SetIndexedSlot(self, *(signed char *)(self + 0x20), Ov201_StrafeMove);
        return;
    }

    if (*(int *)(target + 0x78) < *(int *)(ctx[0x13] + 4)) {
        ctx[4] -= 0x200;
    } else {
        ctx[4] += 0x200;
    }
}

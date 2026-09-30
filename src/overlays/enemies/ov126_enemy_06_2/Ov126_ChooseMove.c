/* Ov126_ChooseMove -- ov126's move CHOOSER, the steering variant (see Ov200_AiChooseMoveSteering for
 * the other one, and Ov208_AiChooseAttack for the plain shape).
 *
 * The ROM loads self[0] and self[1] with one `ldm`: scene and ctx come from a block where the
 * owner (ctx[0]) is read BEFORE the ctx[0xf] rate store, which pulls the ctx load up next to the
 * scene load; `ctx`, `own2`, `target`, `t2` and `owner` are declared in that order.
 *
 * A dispatcher reads the queued move at ctx[0]+0x1c7 and runs it; a chooser decides what to queue.
 * This one steers first: it aims a matrix at the target, turns the distance into a Q12 approach
 * factor, and writes a velocity into ctx[2..4].
 *
 * Ov107_FindNearestObject hands back the target AND writes the SQUARED distance through its out-param,
 * which is why FX_Sqrt follows; `d` is that one slot reused, squared distance then real gap.
 *
 * Flow:
 *   no target                    -> queue move 2
 *   d    = sqrt(distSq) minus both collision radii (+0x80 each)
 *   fac  = FX_Inv(0x4000 - d, 0x4000) clamped to [-0x1000, 0x1000]   (1.0 = 0x1000)
 *   velocity = the aimed forward vector scaled by -fac, then by 0x280
 *   d > *(ctx[0]+0x2d8)          -> out of leash: re-roll the ctx[0x15] timer, queue move 2
 *   ctx[0]+0x13c == 0x7fffffff   -> no attack window: drift ctx[3] by +/-0x200 and return
 *   ctx[3] = 0x180
 *   ctx[0]+0x13c <= 0x3800       -> return
 *   ctx[0x1e] = a coin flip: 1 or -1 (the strafe direction)
 *   ctx[0x15] > 0                -> a d201: 0 queues move 5, else dispatch Ov126_DiveTick as
 *                                   the handler instead of queueing
 *   otherwise                    -> roll a d100, re-roll the ctx[0x15] timer, then:
 *                                     roll < 20 and slot +0x390 free -> move 6
 *                                     roll > 79                      -> move 5
 *                                     otherwise                      -> move 7
 *
 * ctx[0xf] takes `*(int *)(*(int *)self + 0x2c) * 30 / 10` -- the ROM's magic 0x66666667 with a
 * >>34 is a divide by 10, so this is the frame value times three, written as a rate conversion.
 *
 * `+ (d - d)` on two of the RNG results is NOT a typo and must not be "simplified": RandNextScaled
 * returns long long, and that unfoldable zero is what makes mwcc emit the ROM's `adds r0, r0, #0`.
 * The other three rolls feed a real add, so they need nothing. See deferred-ties.md.
 *
 * FX_Inv is the reloc's own name for 01ff8a04, but it takes two arguments and divides -- FX_Div in
 * NitroSDK terms. The name is kept because the symbol table says so.
 *
 * The odd spellings are load-bearing, as in Ov200_AiChooseMoveSteering: store-then-reload for the target,
 * the `!= 0 ? -1 : 1` ternary, and inverted guards so their bodies land out of line at the end.
 */

/* The ROM loads both of these with one `ldm` -- they are adjacent fields, not two
 * independent dereferences of `self`. */

#include "nitro/fx_types.h"

typedef struct {
    int *scene;
    int *ctx;
} Self;

extern int Ov107_FindNearestObject(int obj, int *outDistSq);
extern void SetIndexedSlot(int self, int slot, void (*cb)(void));
extern int FX_Sqrt(int x);
extern void Mtx33_LookAt(int *dst, const VecFx32 *a, const VecFx32 *b, const void *c);
extern void Quat_FromMtx33(int *dst, const int *src);
extern int FX_Div(int num, int den);
extern void Vec3TransformViaTempMtx(VecFx32 *dst, const int *a, const void *b);
extern void ScaleVec3Fx12(int scale, const VecFx32 *src, int *dst);
extern int RandNextScaled();
extern int Ov126_IsField34Nibble1(int slot);
extern void Ov126_DiveTick(void);
extern char data_02042264[];
extern char data_02042258[];

void Ov126_ChooseMove(int self) {
    Self *s;
    int *ctx;
    int *own2;
    int target;
    int t2;
    int *owner;
    int d;
    int fac;
    int base;
    int span;
    int roll;
    int mtx[9];
    VecFx32 dir;

    s = (Self *)self;
    {
        int *scene = s->scene;
        int *c = s->ctx;
        owner = (int *)c[0];
        c[0xf] = scene[0xb] * 30 / 10;
        ctx = c;
    }
    ctx[1] = Ov107_FindNearestObject(ctx[0], &d);
    target = ctx[1];
    if (target == 0) {
        *(signed char *)(ctx[0] + 0x1c7) = 2;
        SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
        return;
    }

    own2 = (int *)ctx[0];
    d = (FX_Sqrt(d) - *(int *)(target + 0x80)) - own2[0x20];

    t2 = ctx[1];
    Mtx33_LookAt(mtx, (const VecFx32 *)(t2 + 0x74), (const VecFx32 *)ctx[9], data_02042264);
    Quat_FromMtx33(&ctx[0x1a], mtx);
    fac = FX_Div(0x4000 - d, 0x4000);
    if (fac < -0x1000) {
        fac = -0x1000;
    }
    if (fac > 0x1000) {
        fac = 0x1000;
    }
    Vec3TransformViaTempMtx(&dir, &ctx[0x1a], data_02042258);
    ScaleVec3Fx12(-fac, &dir, &ctx[2]);
    ScaleVec3Fx12(0x280, (const VecFx32 *)&ctx[2], &ctx[2]);

    if (d > *(int *)(ctx[0] + 0x2d8)) {
        base = *(int *)(ctx[0] + 0x224);
        span = *(int *)(ctx[0] + 0x228) - base;
        if (span < 0) {
            span = -span;
        }
        ctx[0x15] = base + RandNextScaled(span + 1);
        *(signed char *)(ctx[0] + 0x1c7) = 2;
        SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
        return;
    }

    if (owner[0x4f] != 0x7fffffff) {
        ctx[3] = 0x180;
        if (owner[0x4f] <= 0x3800) {
            return;
        }

        ctx[0x1e] = (RandNextScaled(2) + (d - d) != 0) ? -1 : 1;

        if (ctx[0x15] <= 0) {
            roll = RandNextScaled(0x65) + (d - d);
            base = *(int *)(ctx[0] + 0x224);
            span = *(int *)(ctx[0] + 0x228) - base;
            if (span < 0) {
                span = -span;
            }
            ctx[0x15] = base + RandNextScaled(span + 1);

            if (roll < 0x14 && !Ov126_IsField34Nibble1(*(int *)(ctx[0] + 0x390))) {
                *(signed char *)(ctx[0] + 0x1c7) = 6;
                SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
                return;
            }
            if (roll < 0x50) {
                *(signed char *)(ctx[0] + 0x1c7) = 7;
                SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
                return;
            }
            *(signed char *)(ctx[0] + 0x1c7) = 5;
            SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
            return;
        }

        if (RandNextScaled(0xc9) + (d - d) == 0) {
            *(signed char *)(ctx[0] + 0x1c7) = 5;
            SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
            return;
        }
        SetIndexedSlot(self, *(signed char *)(self + 0x20), Ov126_DiveTick);
        return;
    }

    if (*(int *)(t2 + 0x78) < *(int *)(ctx[9] + 4)) {
        ctx[3] -= 0x200;
    } else {
        ctx[3] += 0x200;
    }
}

/* Ov229_SlamTick -- the slam tick (x2 with ov229): the impulse at ctx+0x1c is copied out to
 * ctx+0x10 and then grown, walking the elapsed time in steps of 0x88 and scaling it by
 * `0x1000 + FX_Mul(FX_Inv(min(step, 0x88), 0x88), 0xab)` each step (see Ov228_AiHomingDriftTick for
 * the decaying twin). The owner's +0x494 contact sphere, moved by the copied impulse, is swept
 * with kind 2 (Ov229_ContactSweep). Once the owner lands (+0x17a bit 0) the owner's +0x180 point
 * is kept at ctx+0x34 with its height lowered to the owner's +0x80 floor less 0x200, effect 6
 * spawns there, animation 0x19 plays, reaction 0x12b mode 0xc fires at it, the timer at +0x4c
 * restarts and the tick hands over to Ov229_AiExpandingSweepTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct {
    VecFx32 pos;
    int radius;
} Sphere;

struct Bits17a { unsigned char b0 : 1; };

extern int FX_Div(int a, int b);
extern void ScaleVec3Fx12(int scale, VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Ov229_ContactSweep(int *ctx, int kind, Sphere *sphere, void *box);
extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int self, int action, void *cb);
extern void Ov229_AiExpandingSweepTick(void);

void Ov229_SlamTick(int self) {
    int *ctx;
    Sphere sphere;
    int step;

    ctx = *(int **)(self + 4);
    *(VecFx32 *)((char *)ctx + 0x10) = *(VecFx32 *)((char *)ctx + 0x1c);

    step = *(int *)(*(int *)self + 0x2c);
    while (step > 0) {
        ScaleVec3Fx12((int)(((long long)FX_Div(step <= 0x88 ? step : 0x88, 0x88) * 0xabLL
                             + 0x800LL)
                            >> 12)
                          + 0x1000,
                      (VecFx32 *)((char *)ctx + 0x1c), (VecFx32 *)((char *)ctx + 0x1c));
        step -= 0x88;
    }

    sphere = *(Sphere *)(ctx[0] + 0x494);
    VEC_Add(&sphere.pos, (VecFx32 *)((char *)ctx + 0x10), &sphere.pos);
    Ov229_ContactSweep(ctx, 2, &sphere, 0);
    if (((struct Bits17a *)(ctx[0] + 0x17a))->b0 == 0) {
        return;
    }
    *(VecFx32 *)((char *)ctx + 0x34) = *(VecFx32 *)(ctx[0] + 0x180);
    ctx[0xe] -= *(int *)(ctx[0] + 0x80) - 0x200;
    func_ov107_020c0b90(ctx[0], 6, *(VecFx32 *)((char *)ctx + 0x34), 0);
    Ov107_PostTagUpdate((Actor *)ctx[0], 0x19, 0);
    Ov107_BuildAndSendUpdate(ctx[0], 0x12b, 0xc, (char *)ctx + 0x34);
    ctx[0x13] = 0;
    SetIndexedSlot(self, *(signed char *)(self + 0x20), Ov229_AiExpandingSweepTick);
}

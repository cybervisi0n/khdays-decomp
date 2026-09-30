
#include "nitro/fx_types.h"
#include "game/engine.h"

extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *c);
extern void func_ov107_020c0b90(int obj, int cmd, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int a, int b, int c, void *d);
extern void SetIndexedSlot(void *self, int idx, void *cb);
extern void Ov266_AiWaitThenAnim3(void);

/* Wind-up wait, then fire the effect (and its byte-identical twins). Accumulate dt into the
 * phase timer (ctx1+0x40) and bail until it reaches 3400. Then take the fixed
 * local offset {0, 10650, 6327}, rotate it by the owner's orientation matrix
 * (owner+0xa0) and add the anchor point (ctx1[2]) to get a world position; spawn
 * the effect there, kick the 0x48 animation, reset the timer and advance state. */
void Ov266_WaitThenSpawnEffect(void *self) {
    int *c0 = ((int **)self)[0];
    int *c1 = ((int **)self)[1];
    VecFx32 v;
    int t = c1[0x10] + c0[0xb];

    c1[0x10] = t;
    if (t < 3400) {
        return;
    }
    v.x = 0;
    v.y = 10650;
    v.z = 6327;
    Vec3TransformViaTempMtx(&v, (const void *)(c1[0] + 0xa0), &v);
    VEC_Add(&v, (VecFx32 *)c1[2], &v);
    func_ov107_020c0b90(c1[0], 0, v, 0);
    Ov107_BuildAndSendUpdate(c1[0], 0, 0x48, (void *)(c1[0] + 0x74));
    c1[0x10] = 0;
    SetIndexedSlot(self, *(signed char *)((char *)self + 0x20), Ov266_AiWaitThenAnim3);
}

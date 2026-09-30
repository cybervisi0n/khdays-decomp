/* Sweep turn tick: re-acquires the lock-on target into +0x60 (none: pose request 0, dispatch
 * null), keeps its +0x74 position at +0x70, runs the +0x18 timer and maps it to a 0..1 blend t.
 * The +0x30 rotation faces the target from the +4 anchor, the +0x20 pose slerps towards it by
 * 30/5 of the frame step, its forward (data_02042258) scaled by the +0x1c speed and by t is
 * blended with (1 - t) of the +0x64 base velocity into +0xc, and the pose is copied to +0x40.
 * Once t reaches 1.0 the timer and +0x7c clear, effect 0x122 (kind 6) spawns at the anchor from
 * the +0x384 model and the node moves to 020d0d8c; otherwise the common 020d1364 step runs. */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct m4 { int w[4]; };

extern int  Ov107_FindNearestObject(int obj, int flag);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern int FX_Div(int num, int den);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void Quat_FromTwoVectors(void *rotation, const VecFx32 *from, const VecFx32 *to);
extern void Quat_Slerp(void *a, int s, void *b, void *m);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Ov107_BuildAndSendUpdate(int actor, int id, int kind, void *anchor);
extern void Ov213_SweepPassHitTest(int *state);
extern const VecFx32 data_02042258;
extern void Ov213_SweepPassTick(void);

void Ov213_SweepTurnTick(int *node) {
    int *state = (int *)node[1];
    VecFx32 dir;
    VecFx32 fwd;
    int t;
    int target;

    target = state[0x18] = Ov107_FindNearestObject(*state, 0);
    if (target == 0) {
        *(unsigned char *)(*state + 0x1c7) = 0;
        SetIndexedSlot(node, *(signed char *)(node + 8), 0);
        return;
    }
    *(VecFx32 *)(state + 0x1c) = *(VecFx32 *)(target + 0x74);
    state[6] += *(int *)(node[0] + 0x2c);
    t = FX_Div(state[6], 0x1000);
    if (t > 0x1000) t = 0x1000;
    VEC_Subtract((VecFx32 *)(state + 0x1c), (VecFx32 *)state[1], &dir);
    VEC_Normalize(&dir, &dir);
    Quat_FromTwoVectors((void *)(state + 0xc), &data_02042258, &dir);
    Quat_Slerp(state + 8, *(int *)(node[0] + 0x2c) * 30 / 5, state + 8, state + 0xc);
    Vec3TransformViaTempMtx(&fwd, (void *)(state + 8), &data_02042258);
    ScaleVec3Fx12(state[7], &fwd, &fwd);
    ScaleVec3Fx12(t, &fwd, &fwd);
    ScaleVec3Fx12(0x1000 - t, (VecFx32 *)(state + 0x19), (VecFx32 *)(state + 3));
    VEC_Add((VecFx32 *)(state + 3), &fwd, (VecFx32 *)(state + 3));
    *(struct m4 *)(state + 0x10) = *(struct m4 *)(state + 8);
    if (t >= 0x1000) {
        state[6] = 0;
        state[0x1f] = 0;
        Ov107_BuildAndSendUpdate(*(int *)(state[0] + 0x384), 0x122, 6, (void *)state[1]);
        SetIndexedSlot(node, *(signed char *)(node + 8), Ov213_SweepPassTick);
        return;
    }
    Ov213_SweepPassHitTest(state);
}

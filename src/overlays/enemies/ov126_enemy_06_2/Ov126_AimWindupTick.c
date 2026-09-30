/*
 * Ov126_AimWindupTick -- aim wind-up tick. Recompute the aim point (anchor midpoint pulled 0x100
 * towards the player's +0x7c) and the +0x30 timer accumulates the owner's rate; the state[4]
 * sub-node is placed at the aim point and grows with the timer (2t/0x2000 + 1 on x/y, 1 on z).
 * Once the timer reaches 0x2000 the release fires: state[4] gets bit 1, state[2] loses it, is
 * placed at the aim point and scaled 4/4/1; the +0x18 direction is the owner's +0xa0 basis turned
 * by data_02042258, zero-scaled and offset from the aim point, copied to +0x24; state[1] loses bit 1,
 * is scaled 2/0/2, given the direction's pose (ed60 by data_02042240 + normalise) and placed at
 * the aim point; the timer is cleared and the 020ceb74 state registered.
 */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void VEC_Add(void *a, void *b, void *out);
extern void ScaleVec3Fx12(int scale, void *in, void *out);
extern int *Ov107_GetActorManager(void);
extern int FX_Div(int num, int den);
extern void Srt_SetTranslation(void *p, void *v);
extern void Srt_SetScaleXYZ(void *placement, int x, int y, int z);
extern void Quat_FromTwoVectors(void *dst, void *src, void *m);
extern void Srt_SetRotationQuat(void *pose, void *q);
extern void SetIndexedSlot(int self, int idx, int cb);
extern int data_02042258;
extern int data_02042240;
extern void Ov126_BeamTick(void);

void Ov126_AimWindupTick(int *self) {
    int *state = (int *)self[1];
    VecFx32 v;
    VecFx32 dir;
    VecFx32 w;
    VecFx32 pose;
    int q[4];
    int t;

    VEC_Add((void *)(*(int *)(*state + 0x394) + 0x14), (void *)(*(int *)(*state + 0x398) + 0x14), &v);
    ScaleVec3Fx12(0x800, &v, &v);
    ScaleVec3Fx12(-0x100, (void *)(*Ov107_GetActorManager() + 0x7c), &w);
    VEC_Add(&v, &w, &dir);
    state[0xc] += *(int *)(self[0] + 0x2c);
    t = FX_Div(state[0xc], 0x2000);
    Srt_SetTranslation((void *)(state[4] + 4), &dir);
    Srt_SetScaleXYZ((void *)(state[4] + 4), t * 2 + 0x1000, t * 2 + 0x1000, 0x1000);
    if (state[0xc] < 0x2000) {
        return;
    }
    *(int *)(state[4] + 0x5c) |= 2;
    *(int *)(state[2] + 0x5c) &= ~2;
    Srt_SetTranslation((void *)(state[2] + 4), &dir);
    Srt_SetScaleXYZ((void *)(state[2] + 4), 0x4000, 0x4000, 0x1000);
    Vec3TransformViaTempMtx(&pose, (char *)*state + 0xa0, &data_02042258);
    ScaleVec3Fx12(0, &pose, state + 6);
    VEC_Add(&v, state + 6, state + 6);
    *(VecFx32 *)(state + 9) = *(VecFx32 *)(state + 6);
    *(int *)(state[1] + 0x5c) &= ~2;
    Srt_SetScaleXYZ((void *)(state[1] + 4), 0x2000, 0, 0x2000);
    Quat_FromTwoVectors(q, &data_02042240, &pose);
    Vec4_Normalize(q, q);
    Srt_SetRotationQuat((void *)(state[1] + 4), q);
    Srt_SetTranslation((void *)(state[1] + 4), &v);
    state[0xc] = 0;
    SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), (int)&Ov126_BeamTick);
}

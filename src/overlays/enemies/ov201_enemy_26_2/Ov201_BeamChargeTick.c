/* Beam charge tick of the ov200 enemy (x3: ov200/ov201/ov271). The aim point is the midpoint
 * of the +0x48 and +0x4c anchors; the +0xc node is placed there while the +0x34 timer
 * accumulates the owner's rate and scales the node by 2 x (timer / 2.0) + 0x1200 (z 1.0). The
 * charge lasts 0x1a90 without a +0x14 target, 2.0 with one. Then the +0xc and +8 nodes are unflagged,
 * the +8 node placed at the aim point with scale (1.0, 2.0, 0.5), the +0x18 point becomes the aim
 * point plus 0 x the owner's +0xa0 pose applied to data_02042258, copied to +0x24, the +4
 * node is unflagged and scaled 3 x (1.5 without a target, 1.0 with) on x/z, posed to face the
 * forward from data_02042240 (ed60 + f4a4) at the aim point, the timer clears and, in owner
 * mode 1 with a target kind of 0 or 2, the overlay's 4-byte command (data_ov201_020d5480, byte
 * 3 = 5 for kind 0 / 6 otherwise) goes to the owner's +0x24 hook. +0x50 is set and the tick
 * hands over to Ov201_BeamFireTick. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int q[4]; } Quat;
typedef struct { u16 lo; u16 hi; } Cmd4;

extern void VEC_Add(void *a, void *b, void *out);
extern void ScaleVec3Fx12(int scale, void *in, void *out);
extern int FX_Div(int num, int den);
extern void Srt_SetTranslation(void *p, void *v);
extern void Srt_SetScaleXYZ(void *placement, int x, int y, int z);
extern void Quat_FromTwoVectors(Quat *dst, void *src, VecFx32 *m);
extern void Srt_SetRotationQuat(void *pose, Quat *q);
extern void SetIndexedSlot(int self, int idx, void *cb);
extern int data_02042258;
extern int data_02042240;
extern const Cmd4 data_ov201_020d5480;
extern void Ov201_BeamFireTick(void);

void Ov201_BeamChargeTick(int *self) {
    int *state = (int *)self[1];
    VecFx32 aim;
    VecFx32 fwd;
    Quat q;
    Cmd4 cmd;
    int t;
    int nLimit;
    int nScale;

    nLimit = state[5] == 0 ? 0x1a90 : 0x2000;
    VEC_Add((void *)state[0x12], (void *)state[0x13], &aim);
    ScaleVec3Fx12(0x800, &aim, &aim);
    Srt_SetTranslation((void *)(state[3] + 4), &aim);
    state[0xd] += *(int *)(self[0] + 0x2c);
    t = FX_Div(state[0xd], 0x2000);
    Srt_SetTranslation((void *)(state[3] + 4), &aim);
    Srt_SetScaleXYZ((void *)(state[3] + 4), t * 2 + 0x1200, t * 2 + 0x1200, 0x1000);
    if (state[0xd] < nLimit) {
        return;
    }
    nScale = state[5] == 0 ? 0x1800 : 0x1000;
    *(int *)(state[3] + 0x5c) &= ~2;
    *(int *)(state[2] + 0x5c) &= ~2;
    Srt_SetTranslation((void *)(state[2] + 4), &aim);
    Srt_SetScaleXYZ((void *)(state[2] + 4), 0x1000, 0x2000, 0x800);
    Vec3TransformViaTempMtx(&fwd, (char *)*state + 0xa0, &data_02042258);
    ScaleVec3Fx12(0, &fwd, (VecFx32 *)(state + 6));
    VEC_Add(&aim, (VecFx32 *)(state + 6), (VecFx32 *)(state + 6));
    *(VecFx32 *)(state + 9) = *(VecFx32 *)(state + 6);
    *(int *)(state[1] + 0x5c) &= ~2;
    Srt_SetScaleXYZ((void *)(state[1] + 4), nScale * 3, 0, nScale * 3);
    Quat_FromTwoVectors(&q, &data_02042240, &fwd);
    Vec4_Normalize(&q, &q);
    Srt_SetRotationQuat((void *)(state[1] + 4), &q);
    Srt_SetTranslation((void *)(state[1] + 4), &aim);
    state[0xd] = 0;
    if (*(int *)(*state + 0x50) == 1 && (state[5] == 0 || state[5] == 2)) {
        cmd = data_ov201_020d5480;
        ((u8 *)&cmd)[3] = state[5] == 0 ? 5 : 6;
        if (*(void (**)(int, Cmd4 *, int))(*state + 0x24) != 0) {
            (*(void (**)(int, Cmd4 *, int))(*state + 0x24))(*state, &cmd, 4);
        }
    }
    state[0x14] = 1;
    SetIndexedSlot((int)self, *(signed char *)((int)self + 0x20), (void *)Ov201_BeamFireTick);
}

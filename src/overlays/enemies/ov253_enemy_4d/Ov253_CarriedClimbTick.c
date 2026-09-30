/* Ov253_CarriedClimbTick -- carried item climb: the +0x384 owner's chain picked by the +0x388
 * count plus three minus its +0x450 side, modulo four, supplies the +8 / +0x14 endpoints at
 * the +0x24 joint index and the next one; the +0x20 timer runs up and its ratio over 0.25
 * (clamped to 1.0) places the +4 item along the segment at scale 2.0, turned to face it about
 * data_02042264, with the item's +0x70 grey level rising with the index (bit 1 of +0x5c
 * cleared); once complete the item sits at the far end, the timer restarts and the index
 * advances -- at four the node is released (0203c640). */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct Ov253Joints { int cur[1]; int next[3]; };

extern int FX_Div(int num, int den);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Quat_FromTwoVectors(void *rotation, const VecFx32 *from, const VecFx32 *to);
extern void Srt_SetTranslation(void *srt, const VecFx32 *translation);
extern void Srt_SetScaleUniform(void *srt, int scale);
extern void Srt_SetRotationQuat(void *srt, const void *rotation);
extern const VecFx32 data_02042264;

static inline int Ov253_Grey(int v) {
    return v | (v << 5) | (v << 10);
}

void Ov253_CarriedClimbTick(int *node) {
    int *state = (int *)node[1];
    int *anchors[4];
    VecFx32 pos;
    VecFx32 dir;
    int rot[4];
    int t;
    int len;
    int idx;
    int *chain;
    int owner = *(int *)(*state + 0x384);

    state[8] += *(int *)(node[0] + 0x2c);
    t = FX_Div(state[8], 0x400);
    if (t > 0x1000) {
        t = 0x1000;
    }
    *(int *)(state[1] + 0x5c) &= ~2;
    *(unsigned short *)(state[1] + 0x70) = Ov253_Grey(((t << 3) + (state[9] << 15)) >> 12);
    anchors[3] = (int *)(owner + 0x3ac);
    anchors[0] = (int *)(owner + 0x3f4);
    anchors[1] = (int *)(owner + 0x418);
    anchors[2] = (int *)(owner + 0x3d0);
    idx = (*(int *)(*state + 0x388) + (3 - *(signed char *)(owner + 0x400 + 0x50))) % 4;
    chain = anchors[idx];
    *(VecFx32 *)(state + 2) = *(VecFx32 *)(chain[state[9]] + 0x14);
    *(VecFx32 *)(state + 5) = *(VecFx32 *)(((struct Ov253Joints *)chain)->next[state[9]] + 0x14);
    VEC_Subtract((VecFx32 *)(state + 5), (VecFx32 *)(state + 2), &dir);
    len = VEC_Normalize(&dir, &dir);
    ScaleVec3Fx12((int)(((long long)len * t + 0x800) >> 12), &dir, &pos);
    VEC_Add((VecFx32 *)(state + 2), &pos, &pos);
    Quat_FromTwoVectors(rot, &data_02042264, &dir);
    Srt_SetTranslation((void *)(state[1] + 0x30), &pos);
    Srt_SetScaleUniform((void *)(state[1] + 0x30), 0x2000);
    Srt_SetRotationQuat((void *)(state[1] + 0x30), rot);
    if (t < 0x1000) {
        return;
    }
    Srt_SetTranslation((void *)(state[1] + 0x30), (VecFx32 *)(state + 5));
    state[8] = 0;
    state[9] += 1;
    if (state[9] != 4) {
        return;
    }
    Task_MarkFinished(node);
}

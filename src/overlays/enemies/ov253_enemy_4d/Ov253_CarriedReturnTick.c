/* Ov253_CarriedReturnTick -- carried item return: the +8 start follows the actor's +0x390 item
 * anchor and the +0x14 end one of the +0x384 owner's four part anchors (+0x3f4 / +0x418 /
 * +0x3d0 / +0x3ac) picked by the +0x388 count plus three minus the owner's +0x450 side, modulo
 * four; the +0x20 timer runs up and its ratio over 0.25 (clamped to 1.0) places the +4 item
 * along the segment; once complete the timer and hop count clear, the item sits at the end and
 * the node moves to 020d1660. */

#include "nitro/fx_types.h"

extern int FX_Div(int num, int den);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Srt_SetTranslation(void *srt, const VecFx32 *translation);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov253_CarriedClimbTick(void);

void Ov253_CarriedReturnTick(int *node) {
    int *state = (int *)node[1];
    int *anchors[4];
    VecFx32 dir;
    VecFx32 pos;
    int t;
    int len;
    int idx;
    int *chain;
    int owner = *(int *)(*state + 0x384);

    anchors[3] = (int *)(owner + 0x3ac);
    anchors[0] = (int *)(owner + 0x3f4);
    anchors[1] = (int *)(owner + 0x418);
    anchors[2] = (int *)(owner + 0x3d0);
    idx = (*(int *)(*state + 0x388) + (3 - *(signed char *)(owner + 0x400 + 0x50))) % 4;
    chain = anchors[idx];
    *(VecFx32 *)(state + 2) = *(VecFx32 *)(*(int *)(*state + 0x390) + 0x14);
    *(VecFx32 *)(state + 5) = *(VecFx32 *)(chain[0] + 0x14);
    state[8] += *(int *)(node[0] + 0x2c);
    t = FX_Div(state[8], 0x400);
    if (t > 0x1000) {
        t = 0x1000;
    }
    VEC_Subtract((VecFx32 *)(state + 5), (VecFx32 *)(state + 2), &dir);
    len = VEC_Normalize(&dir, &dir);
    ScaleVec3Fx12((int)(((long long)len * t + 0x800) >> 12), &dir, &pos);
    VEC_Add((VecFx32 *)(state + 2), &pos, &pos);
    Srt_SetTranslation((void *)(state[1] + 0x30), &pos);
    if (t < 0x1000) {
        return;
    }
    state[8] = 0;
    state[9] = 0;
    Srt_SetTranslation((void *)(state[1] + 0x30), (VecFx32 *)(state + 5));
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov253_CarriedClimbTick);
}

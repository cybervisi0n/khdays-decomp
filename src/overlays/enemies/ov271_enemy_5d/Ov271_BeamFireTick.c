/* Beam fire tick of the ov200 enemy (x3: ov200/ov201/ov271). The +0x34 timer accumulates the
 * owner's rate; its fraction of 2.0 scales the +0xc node (placed at the midpoint of the +0x48
 * and +0x4c anchors) to 2 x t + 0x1200 on x, 2 x t + 2.0 on y; its fraction of 0.5, capped at
 * 1.0, times 30 is the beam length, and the node is rescaled to 3 x t + 1.5 by 2 x t + 1.5.
 * The +4 node is scaled by 4 x (1.5 without a +0x14 target, 1.0 with) on x/z and 0.1 of the
 * length on y, and the beam runs through Ov271_TickBeam with the length and mode 1. */

#include "nitro/fx_types.h"

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

extern int FX_Div(int num, int den);
extern void VEC_Add(void *a, void *b, void *out);
extern void ScaleVec3Fx12(int scale, void *in, void *out);
extern void Srt_SetTranslation(void *p, void *v);
extern void Srt_SetScaleXYZ(void *placement, int x, int y, int z);
extern void Ov271_TickBeam(int *node, int nLength, int mode);

void Ov271_BeamFireTick(int *self) {
    int *state = (int *)self[1];
    VecFx32 aim;
    int t;
    int nLen;
    int nScale;
    int nGrow;

    nScale = state[5] == 0 ? 0x1800 : 0x1000;
    state[0xd] += *(int *)(self[0] + 0x2c);
    t = FX_Div(state[0xd], 0x2000);
    VEC_Add((void *)state[0x12], (void *)state[0x13], &aim);
    ScaleVec3Fx12(0x800, &aim, &aim);
    Srt_SetTranslation((void *)(state[3] + 4), &aim);
    Srt_SetScaleXYZ((void *)(state[3] + 4), t * 2 + 0x1200, t * 2 + 0x2000, 0x1000);
    nGrow = FX_Div(state[0xd], 0x800);
    if (nGrow > 0x1000) {
        nGrow = 0x1000;
    }
    nLen = nGrow * 30;
    Srt_SetTranslation((void *)(state[3] + 4), &aim);
    Srt_SetScaleXYZ((void *)(state[3] + 4), nGrow * 3 + 0x1800, nGrow * 2 + 0x1800, 0x1000);
    Srt_SetScaleXYZ((void *)(state[1] + 4), nScale * 4, FX_MUL(nLen, 0x1999), nScale * 4);
    Ov271_TickBeam(self, nLen, 1);
}

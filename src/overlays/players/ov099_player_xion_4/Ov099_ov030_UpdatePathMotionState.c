/* Per-frame update of one of the character's eight emitter objects: a state machine on +0x12c that
 * spins it up, orbits it around the owner (radius 0x1800) snapped to the ground, then launches it
 * along its heading, requesting hit spawns on the way and ending in state 5. */

#include "nitro/fx_types.h"

extern void MTX_RotY33_();
extern void ScaleVec3Fx12();
extern void MTX_MultVec33();
extern void VEC_Add();
extern void VEC_Normalize();
extern int VEC_Distance();
extern unsigned short Sequence_UpdateTracks();
extern void Ov099_beginState5();
extern void Ov099_RebindEmitterSlots();
extern void Ov099_RequestSpawnAtObject();
extern void Ov099_ProbeLandingPoint();
extern short data_0203d210[];
extern int data_02041dc8;

int Ov099_ov030_UpdatePathMotionState(char *owner, char *ent, int delta)
{
    char *cfg = *(char **)(owner + 0xdb4);
    int ret = 1;
    int vec[6];
    int mtx[9];

    *(int *)(ent + 0x130) += delta;

    switch (*(unsigned char *)(ent + 0x12c)) {
    case 0:
        ret = 0;
        break;
    case 1:
        *(unsigned short *)(ent + 0x15e) += *(unsigned short *)(owner + 0x10);
        if (*(int *)(ent + 0x130) >= *(int *)(ent + 0x134)) {
            *(unsigned char *)(ent + 0x12c) = 3;
            *(int *)(ent + 0x130) = 0;
        }
        break;
    case 3:
    {
        int a;
        vec[0] = 0;
        vec[1] = 0;
        vec[2] = 0x1000;
        *(unsigned short *)(ent + 0x15e) += *(unsigned short *)(owner + 0x10);
        a = *(unsigned short *)(ent + 0x15e) >> 4;
        MTX_RotY33_(mtx, -data_0203d210[a * 2], -data_0203d210[a * 2 + 1]);
        ScaleVec3Fx12(0x1800, vec, vec);
        MTX_MultVec33(vec, mtx, vec);
        VEC_Add(vec, cfg + 0x48c, &vec[3]);
        Ov099_ProbeLandingPoint(&vec[3], owner, ent, &vec[3], &data_02041dc8, 0);
        *(VecFx32 *)(ent + 0xa4) = *(VecFx32 *)&vec[3];
        VEC_Normalize(vec, vec);
        if (Sequence_UpdateTracks(ent, delta) != 0 && *(short *)(ent + 2) == 0) {
            if (*(int *)(cfg + 0x6bc) != 0x30)
                Ov099_beginState5(ent);
            else
                Ov099_RebindEmitterSlots(ent, 1);
        }
        if (*(int *)(ent + 0x130) >= 0xc000)
            Ov099_RequestSpawnAtObject(owner, ent, ent + 0xa4, vec);
        if (*(int *)(cfg + 0x7b0) >= 0x4b000 || *(int *)(cfg + 0x6bc) != 0x30) {
            *(VecFx32 *)(ent + 0x13c) = *(VecFx32 *)&vec[3];
            MTX_MultVec33(ent + 0x148, mtx, vec);
            VEC_Normalize(vec, ent + 0x148);
            ScaleVec3Fx12(*(int *)(ent + 0x158), ent + 0x148, ent + 0x148);
            *(unsigned char *)(ent + 0x12c) = 4;
            *(int *)(ent + 0x130) = 0;
        }
        break;
    }
    case 4:
        Ov099_RequestSpawnAtObject(owner, ent, ent + 0xa4, ent + 0x148);
        if (*(unsigned char *)(ent + 0x12c) == 4) {
            Ov099_ProbeLandingPoint(&vec[3], owner, ent, ent + 0xa4, ent + 0x148, 1);
            *(VecFx32 *)(ent + 0xa4) = *(VecFx32 *)&vec[3];
            if (Sequence_UpdateTracks(ent, delta) != 0 && *(short *)(ent + 2) == 0)
                Ov099_RebindEmitterSlots(ent, 1);
        }
        if (VEC_Distance(ent + 0x13c, &vec[3]) > *(int *)(ent + 0x154)
            || *(unsigned char *)(ent + 0x12c) != 4)
            Ov099_beginState5(ent);
        break;
    case 5:
        if (Sequence_UpdateTracks(ent, delta) != 0) {
            ret = 0;
            *(unsigned char *)(ent + 0x12c) = 0;
            *(int *)(ent + 0x130) = 0;
        }
        break;
    }
    return ret;
}

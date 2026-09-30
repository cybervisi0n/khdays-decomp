/* Per-frame update of the ov237 actor: the +0x488 and +0x3ec rigs follow the +0x444 body transform
 * (the first 0.5 lower); in move 0xc or +0x49e mode 3 and up the loop effects stop (020ccbac), and a
 * +0x4b4 hold releases the three +0x490 effect handles. The +0x3f0 part points from the +0x448 joint
 * to the +0x44c joint, the +0x48c rig copies its +0x58 pose, and both take the actor transform (+0xa0).
 * In move 10 a linked partner's +0x384 / +0x3ac rigs follow the actor's frame; outside move 9 a
 * pending +0x498 item is dropped (020cb100). Each segment of both arms (+0x3f4) points along its joint
 * chain (+0x41c, from the +0x44c joint for the first) and takes its joint's transform; the +0x45c
 * transform copies the actor's, the +0x470 clock advances by 2.5 and the base update runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int w[11]; } SrtTransform;
typedef struct { int w[8]; } Pose32;
struct Xf10 { char pad[0x10]; SrtTransform srt; };
struct Xf4 { char pad[4]; SrtTransform srt; };
struct Pose58 { char pad[0x58]; Pose32 pose; };
struct Ov237Body { char pad[0x3f4]; int arms[2][5]; int joints[2][5]; };

extern void Ov237_StopLoopEffects(char *self);
extern void TaskList_FinishByTag(int model, int handle);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int queryTableEntry(int rig, int channel);
extern void callIfTableEntrySet(int rig, int channel, int frame);
extern void Ov107_UnlinkNodeFromOwner(int item);
extern void Ov107_AiState_PostTickBase(char *self);

void Ov237_Update(char *self)
{
    VecFx32 d;
    VecFx32 e;
    int k;
    long m;
    int part;

    ((struct Xf10 *)(**(int **)(self + 0x488)))->srt = ((struct Xf4 *)*(int *)(self + 0x444))->srt;
    *(int *)(**(int **)(self + 0x488) + 0x24) -= 0x800;
    ((struct Xf10 *)(*(int *)(self + 0x3ec)))->srt = ((struct Xf4 *)*(int *)(self + 0x444))->srt;
    if (*(signed char *)(self + 0x1c6) == 0xc || *(u8 *)(self + 0x49e) >= 3) {
        Ov237_StopLoopEffects(self);
    }
    if (*(int *)(self + 0x4b4) != 0) {
        u8 i;

        for (i = 0; i < 3; i++) {
            if (*(int *)(*(int *)(self + 0x490) + i * 8 + 0x7c) != 0) {
                TaskList_FinishByTag(*(int *)(self + 0x3c), *(int *)(*(int *)(self + 0x490) + i * 8 + 0x7c));
                *(int *)(*(int *)(self + 0x490) + i * 8 + 0x7c) = 0;
            }
        }
    }
    {
        int part = *(int *)(self + 0x3f0);

        VEC_Subtract((VecFx32 *)(*(int *)(self + 0x44c) + 0x14), (VecFx32 *)(*(int *)(self + 0x448) + 0x14), &d);
        *(int *)(part + 0x70) = VEC_Normalize(&d, (VecFx32 *)(part + 0x64));
        ((struct Pose58 *)**(int **)(self + 0x48c))->pose = ((struct Pose58 *)part)->pose;
    }
    ((struct Xf10 *)(*(int *)(self + 0x3f0)))->srt = *(SrtTransform *)(self + 0xa0);
    ((struct Xf10 *)(**(int **)(self + 0x48c)))->srt = *(SrtTransform *)(self + 0xa0);
    if (*(int *)(self + 0x4ac) != 0 && *(signed char *)(self + 0x1c6) == 10) {
        int frame = queryTableEntry(*(int *)(self + 0x384), 0);

        callIfTableEntrySet(*(int *)(*(int *)(self + 0x4a4) + 0x384), 0, frame);
        callIfTableEntrySet(*(int *)(*(int *)(self + 0x4a4) + 0x3ac), 0, frame);
    }
    if (*(signed char *)(self + 0x1c6) != 9 && *(int *)(self + 0x498) != 0) {
        Ov107_UnlinkNodeFromOwner(*(int *)(self + 0x498));
        *(int *)(self + 0x498) = 0;
    }
    for (k = 0; k < 2; k++) {
        for (m = 0; m < 5; m++) {
            part = ((struct Ov237Body *)self)->arms[k][m];
            if (m == 0) {
                VEC_Subtract((VecFx32 *)(*(int *)(self + 0x44c) + 0x14),
                             (VecFx32 *)(((struct Ov237Body *)self)->joints[k][m] + 0x14), &e);
            } else {
                VEC_Subtract((VecFx32 *)(((struct Ov237Body *)self)->joints[k][m - 1] + 0x14),
                             (VecFx32 *)(((struct Ov237Body *)self)->joints[k][m] + 0x14), &e);
            }
            VEC_Normalize(&e, &e);
            *(VecFx32 *)(part + 0x64) = e;
            *(int *)(part + 0x70) = 0x1000;
            ((struct Xf10 *)(((struct Ov237Body *)self)->arms[k][m]))->srt =
                ((struct Xf4 *)((struct Ov237Body *)self)->joints[k][m])->srt;
        }
    }
    *(SrtTransform *)(self + 0x45c) = *(SrtTransform *)(self + 0xa0);
    *(int *)(self + 0x470) += 0x2800;
    Ov107_AiState_PostTickBase(self);
}

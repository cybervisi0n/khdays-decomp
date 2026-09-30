/* Per-frame update of the ov244 enemy. Outside move 6 bit 4 of +0x420 is set; the move effects
 * (+0x490 for move 4, +0x494 for 5, +0x498 for 6) are released once the move changes, and in move 3
 * the +0x40c table's slots 7 and 0xd are stopped. The ten bone-to-bone collision segments are
 * refreshed (start at the first bone's +0x14, unit direction and length towards the next one's):
 * the eight +0x3e0..+0x3fc capsules along the +0x3d0..+0x3dc, +0x3c0..+0x3cc, +0x3ac/+0x3b0 and
 * +0x3b4/+0x3b8 chains, and the +0x3a0 / +0x3a4 shapes' capsules. The +0x38c / +0x388 rigs take the
 * +0x3a8 / +0x3bc joints' poses turned by data_ov277_020d35c0 (the +0x39c shape copies +0x3bc's),
 * the +0x3b0 prop is turned in place, and the base update runs.
 *
 * Codegen: the pose copies go through typed members (src=lr / dst=ip); a raw cast swaps them. */

#include "nitro/fx_types.h"

typedef struct { int w[4]; } Quat;
typedef struct { int w[11]; } Pose;
typedef struct { VecFx32 p0; VecFx32 dir; int len; } Seg;
struct At4 { char pad[4]; Pose pose; };
struct At10 { char pad[0x10]; Pose pose; };
struct At30 { char pad[0x30]; Pose pose; };

extern void Ov107_UnlinkNodeFromOwner(int effect);
extern void TaskList_FinishByTag(int list, int node);
extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *v, VecFx32 *d);
extern void Quat_Multiply(void *out, const void *a, const Quat *b);
extern void Srt_SetRotationQuat(void *pose, const Quat *q);
extern void Ov107_AiState_PostTickBase(char *self);
extern const Quat data_ov277_020d35c0;

#define SET_SEG(seg, from, to)                                           ((Seg *)(seg))->p0 = *(VecFx32 *)((from) + 0x14);                        VEC_Subtract((to) + 0x14, &((Seg *)(seg))->p0, &((Seg *)(seg))->dir);     ((Seg *)(seg))->len = VEC_Normalize(&((Seg *)(seg))->dir, &((Seg *)(seg))->dir)

void Ov277_Update(char *self)
{
    Quat rot = data_ov277_020d35c0;
    Quat tmp;
    Seg *seg;

    if (*(signed char *)(self + 0x100 + 0xc6) != 6) {
        *(int *)(self + 0x420) |= 0x10;
    }
    if (*(signed char *)(self + 0x100 + 0xc6) != 4 && *(int *)(self + 0x490) != 0) {
        Ov107_UnlinkNodeFromOwner(*(int *)(self + 0x490));
        *(int *)(self + 0x490) = 0;
    }
    if (*(signed char *)(self + 0x100 + 0xc6) != 5 && *(int *)(self + 0x494) != 0) {
        Ov107_UnlinkNodeFromOwner(*(int *)(self + 0x494));
        *(int *)(self + 0x494) = 0;
    }
    if (*(signed char *)(self + 0x100 + 0xc6) != 6 && *(int *)(self + 0x498) != 0) {
        Ov107_UnlinkNodeFromOwner(*(int *)(self + 0x498));
        *(int *)(self + 0x498) = 0;
    }
    if (*(signed char *)(self + 0x100 + 0xc6) == 3) {
        if ((*(int **)(self + 0x40c))[7] != 0) {
            TaskList_FinishByTag(*(int *)(self + 0x3c), (*(int **)(self + 0x40c))[7]);
            (*(int **)(self + 0x40c))[7] = 0;
        }
        if ((*(int **)(self + 0x40c))[0xd] != 0) {
            TaskList_FinishByTag(*(int *)(self + 0x3c), (*(int **)(self + 0x40c))[0xd]);
            (*(int **)(self + 0x40c))[0xd] = 0;
        }
    }
    seg = (Seg *)(*(int *)(self + 0x3e0) + 0x58);
    SET_SEG(seg, *(char **)(self + 0x3d0), *(char **)(self + 0x3d4));
    seg = (Seg *)(*(int *)(self + 0x3e4) + 0x58);
    SET_SEG(seg, *(char **)(self + 0x3d4), *(char **)(self + 0x3d8));
    seg = (Seg *)(*(int *)(self + 0x3e8) + 0x58);
    SET_SEG(seg, *(char **)(self + 0x3d8), *(char **)(self + 0x3dc));
    seg = (Seg *)(*(int *)(self + 0x3ec) + 0x58);
    SET_SEG(seg, *(char **)(self + 0x3c0), *(char **)(self + 0x3c4));
    seg = (Seg *)(*(int *)(self + 0x3f0) + 0x58);
    SET_SEG(seg, *(char **)(self + 0x3c4), *(char **)(self + 0x3c8));
    seg = (Seg *)(*(int *)(self + 0x3f4) + 0x58);
    SET_SEG(seg, *(char **)(self + 0x3c8), *(char **)(self + 0x3cc));
    seg = (Seg *)(*(int *)(self + 0x3f8) + 0x58);
    SET_SEG(seg, *(char **)(self + 0x3ac), *(char **)(self + 0x3b0));
    seg = (Seg *)(*(int *)(self + 0x3fc) + 0x58);
    SET_SEG(seg, *(char **)(self + 0x3b4), *(char **)(self + 0x3b8));
    Quat_Multiply(&tmp, (void *)(*(int *)(self + 0x3a8) + 4), &rot);
    ((struct At30 *)*(int *)(self + 0x38c))->pose = ((struct At4 *)*(int *)(self + 0x3a8))->pose;
    Srt_SetRotationQuat((void *)(*(int *)(self + 0x38c) + 0x30), &tmp);
    Quat_Multiply(&tmp, (void *)(*(int *)(self + 0x3bc) + 4), &rot);
    ((struct At30 *)*(int *)(self + 0x388))->pose = ((struct At4 *)*(int *)(self + 0x3bc))->pose;
    Srt_SetRotationQuat((void *)(*(int *)(self + 0x388) + 0x30), &tmp);
    ((struct At10 *)**(int **)(self + 0x39c))->pose = ((struct At4 *)*(int *)(self + 0x3bc))->pose;
    seg = (Seg *)(**(int **)(self + 0x3a0) + 0x58);
    SET_SEG(seg, *(char **)(self + 0x3d8), *(char **)(self + 0x3dc));
    seg = (Seg *)(**(int **)(self + 0x3a4) + 0x58);
    SET_SEG(seg, *(char **)(self + 0x3c8), *(char **)(self + 0x3cc));
    Quat_Multiply((void *)(*(int *)(self + 0x3b0) + 4), (void *)(*(int *)(self + 0x3b0) + 4), &rot);
    Ov107_AiState_PostTickBase(self);
}

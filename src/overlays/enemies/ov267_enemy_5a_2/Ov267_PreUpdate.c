/* Pre-update of the ov266 enemy: outside mode 9 the +0x620 task is dropped, outside mode 0xc
 * the +0x658 one; the +0x514 offset is reset to (0, 0x1c00, 0), turned by the actor's +0xa0
 * basis and added to the +0xb0 position, then the ov107 actor base finishes the frame. */

#include "nitro/fx_types.h"

extern void TaskList_FinishByTag(int taskList, int task);
extern void Vec3TransformViaTempMtx(VecFx32 *out, void *pose, VecFx32 *in);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Ov107_AiState_PostTickBase(int self);

void Ov267_PreUpdate(int self)
{
    if (*(signed char *)(self + 0x1c6) != 9 && *(int *)(self + 0x620) != 0) {
        TaskList_FinishByTag(*(int *)(self + 0x3c), *(int *)(self + 0x620));
        *(int *)(self + 0x620) = 0;
    }
    if (*(signed char *)(self + 0x1c6) != 0xc && *(int *)(self + 0x658) != 0) {
        TaskList_FinishByTag(*(int *)(self + 0x3c), *(int *)(self + 0x658));
        *(int *)(self + 0x658) = 0;
    }
    *(int *)(self + 0x514) = 0;
    *(int *)(self + 0x518) = 0x1c00;
    *(int *)(self + 0x51c) = 0;
    Vec3TransformViaTempMtx((VecFx32 *)(self + 0x114 + 0x400), (void *)(self + 0xa0), (VecFx32 *)(self + 0x114 + 0x400));
    VEC_Add((VecFx32 *)(self + 0x114 + 0x400), (VecFx32 *)(self + 0xb0), (VecFx32 *)(self + 0x114 + 0x400));
    Ov107_AiState_PostTickBase(self);
}

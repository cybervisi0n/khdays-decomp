/* Per-frame update of the ov210 enemy (x2 with ov211). Outside mode 8 (+0x310) the +0x3d0 part's
 * +0x2c effect is released (0203c650 on the +0x3c list) and cleared; outside sub-state 0xf its
 * +0x14 effect goes the same way, while in sub-state 0xf the +0x3d8 handle is released (ov107
 * b100) and cleared and a +0x1c4 bit 1/3 enemy with no pending request asks for sub-state 2. The
 * beam segment from the +0x3bc part's +0x14 point to the +0x3c4 part's (unit direction, length,
 * radius 0.625) is copied to the +0x3b4 body's and the first +0x3b0 body's +0x58, and the base
 * update (ov107 7ca4) runs. */

#include "nitro/fx_types.h"

typedef struct { VecFx32 p0; VecFx32 dir; int nLength; int nRadius; } Segment;
struct Body { char pad[0x58]; Segment seg; };

extern void TaskList_FinishByTag(int list, int handle);
extern void Ov107_UnlinkNodeFromOwner(int handle);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void Ov107_AiState_PostTickBase(char *self);

void Ov211_Update(char *self)
{
    Segment seg;

    if (*(signed char *)(self + 0x310) != 8 && *(int *)(*(int *)(self + 0x3d0) + 0x2c) != 0) {
        TaskList_FinishByTag(*(int *)(self + 0x3c), *(int *)(*(int *)(self + 0x3d0) + 0x2c));
        *(int *)(*(int *)(self + 0x3d0) + 0x2c) = 0;
    }
    if (*(signed char *)(self + 0x1c6) != 0xf) {
        if (*(int *)(*(int *)(self + 0x3d0) + 0x14) != 0) {
            TaskList_FinishByTag(*(int *)(self + 0x3c), *(int *)(*(int *)(self + 0x3d0) + 0x14));
            *(int *)(*(int *)(self + 0x3d0) + 0x14) = 0;
        }
    } else {
        if (*(int *)(self + 0x3d8) != 0) {
            Ov107_UnlinkNodeFromOwner(*(int *)(self + 0x3d8));
            *(int *)(self + 0x3d8) = 0;
        }
        if ((*(unsigned char *)(self + 0x1c4) & 0xa) != 0 && *(signed char *)(self + 0x1c7) == -1) {
            *(signed char *)(self + 0x1c7) = 2;
        }
    }
    seg.nRadius = 0xa00;
    seg.p0 = *(VecFx32 *)(*(int *)(self + 0x3bc) + 0x14);
    VEC_Subtract((VecFx32 *)(*(int *)(self + 0x3c4) + 0x14), &seg.p0, &seg.dir);
    seg.nLength = VEC_Normalize(&seg.dir, &seg.dir);
    ((struct Body *)*(int *)(self + 0x3b4))->seg = seg;
    ((struct Body *)**(int **)(self + 0x3b0))->seg = seg;
    Ov107_AiState_PostTickBase(self);
}

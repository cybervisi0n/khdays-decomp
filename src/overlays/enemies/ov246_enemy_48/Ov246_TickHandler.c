/* Tick handler of the ov246 enemy. If flagged (+0x1c4 & 0xa) the +0x390 block's +0x24 effect is
 * released and, when idle (+0x1c7 == -1) in sub-state 2, 4, 6 or 7, sub-state 5 is forced.
 * Outside sub-states 6 and 7 the block's +4, +0xc and +0x3c effects and the +0x3a8 / +0x3a4
 * attachments are released. Then the segment from the +0x394 bone's +0x14 to the +0x398
 * bone's +0x14 (unit direction, length, radius 0x1800) is written into the +0x38c item's +0x58
 * and the base tick runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 origin; VecFx32 dir; int nLength; int nRadius; } Segment;
typedef struct { char pad[0x58]; Segment seg; } Ov246Item;

extern void TaskList_FinishByTag(int list, int node);
extern void Ov107_UnlinkNodeFromOwner(int sub);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void Ov107_AiState_PostTickBase(int self);

void Ov246_TickHandler(int self)
{
    Segment seg;

    if ((*(u8 *)(self + 0x1c4) & 0xa) != 0) {
        if (*(int *)(*(int *)(self + 0x390) + 0x24) != 0) {
            TaskList_FinishByTag(*(int *)(self + 0x3c), *(int *)(*(int *)(self + 0x390) + 0x24));
            *(int *)(*(int *)(self + 0x390) + 0x24) = 0;
        }
        if (*(s8 *)(self + 0x1c7) == -1) {
            s8 cur = *(s8 *)(self + 0x1c6);
            if (cur == 2 || cur == 4 || (u8)(s8)(cur - 6) <= 1) {
                *(u8 *)(self + 0x1c7) = 5;
            }
        }
    }
    if (*(s8 *)(self + 0x1c6) != 6 && *(s8 *)(self + 0x1c6) != 7) {
        if (*(int *)(*(int *)(self + 0x390) + 4) != 0) {
            TaskList_FinishByTag(*(int *)(self + 0x3c), *(int *)(*(int *)(self + 0x390) + 4));
            *(int *)(*(int *)(self + 0x390) + 4) = 0;
        }
        if (*(int *)(*(int *)(self + 0x390) + 0xc) != 0) {
            TaskList_FinishByTag(*(int *)(self + 0x3c), *(int *)(*(int *)(self + 0x390) + 0xc));
            *(int *)(*(int *)(self + 0x390) + 0xc) = 0;
        }
        if (*(int *)(*(int *)(self + 0x390) + 0x3c) != 0) {
            TaskList_FinishByTag(*(int *)(self + 0x3c), *(int *)(*(int *)(self + 0x390) + 0x3c));
            *(int *)(*(int *)(self + 0x390) + 0x3c) = 0;
        }
        if (*(int *)(self + 0x3a8) != 0) {
            Ov107_UnlinkNodeFromOwner(*(int *)(self + 0x3a8));
            *(int *)(self + 0x3a8) = 0;
        }
        if (*(int *)(self + 0x3a4) != 0) {
            Ov107_UnlinkNodeFromOwner(*(int *)(self + 0x3a4));
            *(int *)(self + 0x3a4) = 0;
        }
    }
    seg.nRadius = 0x1800;
    seg.origin = *(VecFx32 *)(*(int *)(self + 0x394) + 0x14);
    VEC_Subtract((VecFx32 *)(*(int *)(self + 0x398) + 0x14), &seg.origin, &seg.dir);
    seg.nLength = VEC_Normalize(&seg.dir, &seg.dir);
    (*(Ov246Item **)(self + 0x38c))->seg = seg;
    Ov107_AiState_PostTickBase(self);
}

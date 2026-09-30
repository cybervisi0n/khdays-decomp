/* Tick handler of the ov119 enemy (x3 with ov272/ov279). A flagged (+0x1c4 & 0xa), idle (+0x1c7 ==
 * -1) enemy outside sub-states 0, 1, 3, 4 and 0xb is forced into sub-state 4. Outside sub-state 7
 * the +0x3a8 block's +0x24 effect and the +0x3b0 reaction are released, outside 0xa its +0x1c
 * effect, and outside 9/0xa its +0x14 effect and the +0x3a4 effect are released and the +0x384
 * rig is shown (bit 1 of +0x5c cleared). The block's first item is shown in sub-states 5/6 and
 * hidden otherwise; a +0x3ac grab is let go (ov022 020ad8e0) outside sub-state 0xa. Then the
 * segment from the +0x39c bone's +0x14 to the +0x3a0 bone's +0x14 (unit direction, length, radius
 * 0x240) is written into the +0x38c item's and the +0x388 part's +0x58 and the base tick runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 origin; VecFx32 dir; int nLength; int nRadius; } Segment;
typedef struct { char pad[0x58]; Segment seg; } Ov119Item;

extern void TaskList_FinishByTag(int list, int node);
extern void Ov107_UnlinkNodeFromOwner(int sub);
extern void Ov022_ToggleBit13ByMode(int partner, int flag);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void Ov107_AiState_PostTickBase(int self);

void Ov119_Tick(int self)
{
    Segment seg;

    if ((*(u8 *)(self + 0x1c4) & 0xa) != 0 && *(s8 *)(self + 0x1c7) == -1) {
        s8 cur = *(s8 *)(self + 0x1c6);
        if (cur != 0 && cur != 1 && cur != 3 && cur != 4 && cur != 0xb) {
            ((signed char *)self)[0x1c7] = 4;
        }
    }
    if (*(s8 *)(self + 0x1c6) != 7) {
        if (*(int *)(*(int *)(self + 0x3a8) + 0x24) != 0) {
            TaskList_FinishByTag(*(int *)(self + 0x3c), *(int *)(*(int *)(self + 0x3a8) + 0x24));
            *(int *)(*(int *)(self + 0x3a8) + 0x24) = 0;
        }
        if (*(int *)(self + 0x3b0) != 0) {
            Ov107_UnlinkNodeFromOwner(*(int *)(self + 0x3b0));
            *(int *)(self + 0x3b0) = 0;
        }
    }
    if (*(s8 *)(self + 0x1c6) != 0xa && *(int *)(*(int *)(self + 0x3a8) + 0x1c) != 0) {
        TaskList_FinishByTag(*(int *)(self + 0x3c), *(int *)(*(int *)(self + 0x3a8) + 0x1c));
        *(int *)(*(int *)(self + 0x3a8) + 0x1c) = 0;
    }
    if (*(s8 *)(self + 0x1c6) != 9 && *(s8 *)(self + 0x1c6) != 0xa) {
        if (*(int *)(*(int *)(self + 0x3a8) + 0x14) != 0) {
            TaskList_FinishByTag(*(int *)(self + 0x3c), *(int *)(*(int *)(self + 0x3a8) + 0x14));
            *(int *)(*(int *)(self + 0x3a8) + 0x14) = 0;
        }
        if (*(int *)(self + 0x3a4) != 0) {
            TaskList_FinishByTag(*(int *)(self + 0x3c), *(int *)(self + 0x3a4));
            *(int *)(self + 0x3a4) = 0;
        }
        *(int *)(*(int *)(self + 0x384) + 0x5c) &= ~2;
    }
    if (*(s8 *)(self + 0x1c6) == 5) {
        *(int *)(**(int **)(self + 0x3a8) + 0x5c) &= ~2;
    } else if (*(s8 *)(self + 0x1c6) == 6) {
        *(int *)(**(int **)(self + 0x3a8) + 0x5c) &= ~2;
    } else {
        *(int *)(**(int **)(self + 0x3a8) + 0x5c) |= 2;
    }
    if (*(int *)(self + 0x3ac) != 0 && *(s8 *)(self + 0x1c6) != 0xa) {
        Ov022_ToggleBit13ByMode(*(int *)(self + 0x3ac), 0);
        *(int *)(self + 0x3ac) = 0;
    }
    seg.nRadius = 0x240;
    seg.origin = *(VecFx32 *)(*(int *)(self + 0x39c) + 0x14);
    VEC_Subtract((VecFx32 *)(*(int *)(self + 0x3a0) + 0x14), &seg.origin, &seg.dir);
    seg.nLength = VEC_Normalize(&seg.dir, &seg.dir);
    (*(Ov119Item **)(self + 0x38c))->seg = seg;
    (**(Ov119Item ***)(self + 0x388))->seg = seg;
    Ov107_AiState_PostTickBase(self);
}

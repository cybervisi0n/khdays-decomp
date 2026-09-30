/* Piece lure of the ov144 enemy (and its byte-identical twin): in mission mode 1, with a
 * positive charge range, a +0x3ec piece and the +0x1f4 gate open, the piece's position raised
 * by 0x800 becomes the +0x18 goal and the +0xc target (reporting 1) when its flat distance from
 * the actor's +0x74 position is within the range. */

#include "nitro/fx_types.h"

extern int Ov002_List_GetMode(void);
extern int Ov145_ChargeRange(int *state, int flag);
extern int Ov014_IsState3(void *self);
extern VecFx32 *Ov002_Element_CallHook2C(void *piece);
extern void VEC_Subtract(void *a, void *b, void *d);
extern int VEC_Normalize(void *a, void *d);

int Ov145_LureToPiece(int *state, int flag)
{
    VecFx32 dir;
    VecFx32 goal;
    int range;

    if (Ov002_List_GetMode() == 1) {
        range = Ov145_ChargeRange(state, flag);
        if (range <= 0) {
            return 0;
        }
        if (*(void **)(*state + 0x3ec) != 0 && Ov014_IsState3(*(void **)(*state + 0x3ec)) != 0) {
            goal = *Ov002_Element_CallHook2C(*(void **)(*state + 0x3ec));
            goal.y += 0x800;
            VEC_Subtract(&goal, (void *)(*state + 0x74), &dir);
            dir.y = 0;
            if (VEC_Normalize(&dir, &dir) <= range) {
                *(VecFx32 *)(state + 6) = goal;
                *(VecFx32 *)(state + 3) = *(VecFx32 *)(state + 6);
                return 1;
            }
        }
    }
    return 0;
}

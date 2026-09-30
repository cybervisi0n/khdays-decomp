/* Arms request `slot` of the node's +0x12c rows (stride 0x240): the first time a row is used
 * its 2 animations (+4, stride 0x108) get channels 0 and 2 bound to their +0xe0 tables and
 * rewound, then the row is marked live, its +0xa8 / +0x224 vectors copied from the two
 * arguments and its +0x1b0 vector computed from the slot's world position (4ef0). */

#include "nitro/fx_types.h"

extern void BindAnimTrack(void *animation, int track, void *table, short mode);   /* BindAnimTrack */
extern void Anim_SetFrameWrapped(void *animation, int track, int frame);                /* Anim_SetFrameWrapped */
extern void Ov062_ComputeSlotPosition(char *self, int slot, VecFx32 *out);

void Ov062_ArmRequestRow(char *self, char *node, int slot, VecFx32 *a, VecFx32 *b)
{
    char *row = node + 0x12c + slot * 0x240;
    int i;
    char *anim;
    VecFx32 pos;

    if (*(int *)row == 0) {
        anim = row + 4;
        for (i = 0; i < 2; i++) {
            BindAnimTrack(anim, 0, anim + 0xe0, 0);
            BindAnimTrack(anim, 2, anim + 0xe0, 0);
            Anim_SetFrameWrapped(anim, 0, 0);
            Anim_SetFrameWrapped(anim, 2, 0);
            anim += 0x108;
        }
    }
    *(int *)row = 1;
    *(VecFx32 *)(row + 0xa8) = *a;
    *(VecFx32 *)(row + 0x224) = *b;
    Ov062_ComputeSlotPosition(self, slot, &pos);
    *(VecFx32 *)(row + 0x1b0) = pos;
}

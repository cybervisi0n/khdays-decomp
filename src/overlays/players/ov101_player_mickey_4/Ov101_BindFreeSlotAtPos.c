/* Starts a free effect slot (of six) at the position: activates it, clears its timer and binds its
 * tracks, rewound. */

#include "nitro/fx_types.h"

extern void BindAnimTrack(int a, int b, int c, int d);
extern void Anim_SetFrameWrapped(int a, int b, int c);

void Ov101_BindFreeSlotAtPos(char *arr, VecFx32 *src) {
    char *slot = 0;
    int i;
    char *q = arr + 0x128;
    for (i = 0; i < 6; i++, arr += 0x120, q += 0x120) {
        if (*(int *)(arr + 0x128) == 0) slot = q;
    }
    if (slot == 0) return;
    *(int *)slot = 1;
    *(int *)(slot + 0x10c) = 0;
    BindAnimTrack((int)(slot + 4), 0, (int)(slot + 0xe4), 0);
    BindAnimTrack((int)(slot + 4), 2, (int)(slot + 0xe4), 0);
    Anim_SetFrameWrapped((int)(slot + 4), 0, 0);
    Anim_SetFrameWrapped((int)(slot + 4), 0, 0);
    *(VecFx32 *)(slot + 0xa8) = *src;
}

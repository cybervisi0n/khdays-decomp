/* In action 1 reacts to the hit message flags (bounce on bit 0 / 4). */

#include "nitro/fx_types.h"

extern int func_ov107_020c0b90(int a, int b, VecFx32 v, int c);

int Ov176_OnHitMessage(int *a, int b, short *c) {
    int *p = *(int **)((char *)a + 0x214);
    int *q = *(int **)p;
    VecFx32 *vp;
    unsigned short v;
    if (*(signed char *)((char *)q + 0x1c6) == 1) {
        v = (unsigned short)*(int *)c;
        if (v & 1) {
            if (v & 0x10) {
                vp = *(VecFx32 **)((char *)p + 4);
                func_ov107_020c0b90(*(int *)((char *)q + 0x38c), 2, *vp, 0);
                *(char *)(*(int *)p + 0x1c7) = 0;
                return 1;
            }
        }
    }
    return 0;
}

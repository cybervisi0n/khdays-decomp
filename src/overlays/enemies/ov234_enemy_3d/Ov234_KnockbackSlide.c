#include "nitro/fx_types.h"
#include "game/engine.h"

struct bitguard { unsigned char b0 : 1; };

extern void Ov107_BuildAndSendUpdate(int a, int b, int c, int d);
extern void func_ov107_020c0b90(int obj, int cmd, VecFx32 v, int flag);

/* Per-frame knockback slide: while the node is airborne (+0x17a bit0) and the
 * timer (+0x64) is live, drop the position (y -= 0x400) and push it to the render
 * hook. After 4 frames land (state 3), else on frame 3 play the landing pose;
 * advance and clear the timer each frame. */
void Ov234_KnockbackSlide(void *param_1) {
    int *piVar1 = *(int **)((char *)param_1 + 4);
    int inner;

    if (piVar1[0x19] == 0) {
        return;
    }
    inner = *piVar1;
    if (((struct bitguard *)(inner + 0x17a))->b0 == 0) {
        return;
    }
    Ov107_BuildAndSendUpdate(inner, 0x178, 4, piVar1[2]);
    {
        VecFx32 v = *(VecFx32 *)(*piVar1 + 0x74);
        v.y -= 0x400;
        func_ov107_020c0b90(*piVar1, 0, v, 0);
    }
    if (*(short *)((char *)piVar1 + 0x70) >= 4) {
        int q;
        piVar1[0x19] = 0;
        piVar1[0x12] = 0xff0;
        inner = *piVar1;
        q = QueryActiveStateOrDelegate();
        {
            VecFx32 v = *(VecFx32 *)(inner + 0x74);
            func_ov107_020c0b90(inner, 3, v, q & 0xff);
        }
        *(unsigned char *)(*piVar1 + 0x1c7) = 3;
    } else {
        if (*(short *)((char *)piVar1 + 0x70) == 3) {
            int q;
            inner = *piVar1;
            q = QueryActiveStateOrDelegate();
            {
                VecFx32 v = *(VecFx32 *)(inner + 0x74);
                func_ov107_020c0b90(inner, 6, v, q & 0xff);
            }
        }
        *(short *)((char *)piVar1 + 0x70) = *(short *)((char *)piVar1 + 0x70) + 1;
        piVar1[0x19] = 0;
        piVar1[0x12] = 0xff0;
        *(unsigned char *)(*piVar1 + 0x1c7) = 2;
    }
    piVar1[0x11] = 0;
}

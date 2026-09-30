/* AI step: keeps and scales up the velocity and, when the animation ends, rolls a timer and picks
 * approaching (far or not ready) or attacking. */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct b2 { unsigned char b0:1, b1:1; };

extern void ScaleVec3Fx12(int s, int dst, int src);
extern int Ov298_AcquireTargetGapAndAngle(void *param);
extern void SetIndexedSlot(int obj, int idx, int cb);

void Ov298_AiPickApproachOrAttack(int *this)
{
    int node = this[1];
    VecFx32 *v = (VecFx32 *)(node + 0x1c);
    *(VecFx32 *)(node + 0x10) = *v;
    ScaleVec3Fx12(0xb00, (int)v, (int)v);

    {
        int diff = Ov298_AcquireTargetGapAndAngle(this);

        if (((struct b2 *)(*(int *)node + 0x17a))->b1) {
            *(int *)(node + 0x8c) = 1;
        }
        if (*(unsigned char *)(*(int *)(node + 4) + 0xad) != 0) {
            return;
        }

        *(int *)(node + 0x28) = Rand16NextScaled(0x1922) + 0x1922;

        if (diff > 0x9000) {
            *(signed char *)(*(int *)node + 0x1c7) = 4;
        } else {
            if (*(int *)(node + 0x8c) != 0 && *(int *)(node + 0x40) <= 0) {
                *(signed char *)(*(int *)node + 0x1c7) = 5;
            } else {
                *(signed char *)(*(int *)node + 0x1c7) = 4;
            }
        }
    }

    SetIndexedSlot((int)this, *(signed char *)((int)this + 0x20), 0);
}

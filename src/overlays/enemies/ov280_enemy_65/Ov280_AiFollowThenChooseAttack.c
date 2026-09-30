/* Compute the offset vector (020cca74) into (child)+0x30 and scale it by 0x2000 or 0x4000
 * per the mode byte at (child)+0x49; unless the gate byte at *(child+0x10) is set, clear
 * bit 1 in the low byte of [+8] of the child slot at (*child)+0x3bc and, if 020ccfb8 approves,
 * dispatch. */

#include "nitro/fx_types.h"

extern void Ov280_rotateVecByOwnerYaw(void *out, int a, int b);
extern void ScaleVec3Fx12(int a, int b, int c);
extern int Ov280_ChooseAttack(int);
extern int SetIndexedSlot(int a, int b, void *handler);
struct lo8_020ce10c { unsigned f : 8; };
void Ov280_AiFollowThenChooseAttack(int param_1) {
    int child = *(int *)(param_1 + 4);
    VecFx32 out;
    Ov280_rotateVecByOwnerYaw(&out, param_1, *(int *)(*(int *)child + 0x388) + 0x2c);
    *(VecFx32 *)(child + 0x30) = out;
    if (*(unsigned char *)(child + 0x49) < 2) {
        ScaleVec3Fx12(0x2000, child + 0x30, child + 0x30);
    } else {
        ScaleVec3Fx12(0x4000, child + 0x30, child + 0x30);
    }
    if (*(unsigned char *)*(int *)(child + 0x10) != 0) return;
    {
        int c = *(int *)(*(int *)child + 0x3bc);
        ((struct lo8_020ce10c *)(c + 8))->f &= ~2;
    }
    if (Ov280_ChooseAttack(param_1) == 0) return;
    SetIndexedSlot(param_1, *(signed char *)(param_1 + 0x20), (void *)0);
}

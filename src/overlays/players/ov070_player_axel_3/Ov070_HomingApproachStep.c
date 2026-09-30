
#include "nitro/fx_types.h"

extern void Ov070_ComputeApproachVelocity(VecFx32 *out, void *a, int b, int c);
extern void Ov022_ResolveShotHit(void *a, int b, VecFx32 *c, VecFx32 *d);
extern void func_ov022_02091540(int a, int b);
extern int VEC_Distance(int a, VecFx32 *b);
extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *c);
extern void Ov022_ReleaseRigSlots(int a, int b);

/* Per-frame homing/approach step: copy the target's stored offset (+0xcc) into a
 * work vector, run the approach solver (020b4b08 + 02091b48), and advance the
 * sub-state. If still approaching (state 2) commit the new offset back; otherwise
 * latch state 4, reset the timer to 0x3000, clear the 8-slot history to -1, and
 * hand off to 020914a0. */
int Ov070_HomingApproachStep(void *param_1, int param_2, int param_3, int param_4) {
    VecFx32 s;
    VecFx32 out;
    int *iVar3 = *(int **)(param_2 + 0x138);

    s = *(VecFx32 *)(param_2 + 0xcc);
    Ov070_ComputeApproachVelocity(&out, param_1, param_2, param_3);
    Ov022_ResolveShotHit(param_1, param_2, &s, &out);
    func_ov022_02091540(param_2 + 0x28, param_3);
    if (*(char *)(param_2 + 2) != 3) {
        if (VEC_Distance(param_2 + 0x10, &s) > iVar3[5] ||
            *(int *)(param_2 + 4) >= iVar3[6]) {
            *(char *)(param_2 + 2) = 4;
        }
    }
    if (*(char *)(param_2 + 2) != 2) {
        int i;
        *(char *)(param_2 + 2) = 4;
        *(int *)(param_2 + 4) = 0x3000;
        for (i = 0; i < 8; i++) {
            ((short *)param_2)[i + 0x9e] = -1;
        }
        Ov022_ReleaseRigSlots(param_2, iVar3[0xf]);
    } else {
        VEC_Add(&s, &out, &s);
        *(VecFx32 *)(param_2 + 0xcc) = s;
    }
    return 0;
}

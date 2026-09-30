/* Approach tick of the ov178 enemy (x3: ov178/179/180): while no override height is pending
 * (+0x88), steer the hover height (+0x24) -- towards 0x2800 by a 1/30 step when the target
 * height (+0x44) is unset, otherwise 0x200 per tick towards the actor's +0x194; face the target
 * (look-at matrix from the actor's +0x74 to the target's +0x74, applied via 0202ea48), keep the
 * normalised direction from the actor's anchor (+0x390 +0x14), advance the +0x48 phase by the
 * node's +0x2c speed and, once the actor's +0xad flag is clear, run the setup (020ce710) and
 * hand over to sub-state 2 with the slot cleared. */

#include "nitro/fx_types.h"

extern void Mtx33_LookAt(void *mtx, VecFx32 *from, VecFx32 *to, void *up);
extern void Quat_FromMtx33(void *dst, void *mtx);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void Ov179_RunSetupThenSetHw60HighBit0(int rig, int target, VecFx32 *dir);
extern void SetIndexedSlot(int obj, int slot, void *cb);
extern int data_02042264;

void Ov179_ApproachTick(int node)
{
    char *actor;
    int *state = *(int **)(node + 4);
    char mtx[0x24];
    VecFx32 dir;

    actor = (char *)state[3];
    if (state[0x22] == 0) {
        if (state[0x11] != 0x7fffffff) {
            state[9] += (0x2800 - state[0x11]) / 30;
        } else if (*(int *)(actor + 0x194) < state[9]) {
            state[9] -= 0x200;
        } else {
            state[9] += 0x200;
        }
    }
    Mtx33_LookAt(mtx, (VecFx32 *)(actor + 0x74), (VecFx32 *)state[2], &data_02042264);
    Quat_FromMtx33(state + 0x1d, mtx);
    VEC_Subtract((VecFx32 *)(actor + 0x74), (VecFx32 *)(*(int *)(*state + 0x390) + 0x14), &dir);
    VEC_Normalize(&dir, &dir);
    state[0x12] += *(int *)(*(int *)node + 0x2c);
    if (*(unsigned char *)(state[1] + 0xad) == 0) {
        Ov179_RunSetupThenSetHw60HighBit0(*(int *)(*state + 0x3ac), state[2], &dir);
        *(unsigned char *)(*state + 0x1c7) = 2;
        SetIndexedSlot(node, *(signed char *)(node + 0x20), 0);
    }
}

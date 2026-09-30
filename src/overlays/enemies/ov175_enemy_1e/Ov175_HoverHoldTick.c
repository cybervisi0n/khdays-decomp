/* Hover-hold tick of the ov175 enemy (and its byte-identical twins). With +0x88 clear the +0x24 height
 * eases towards 2.5 (a thirtieth of the difference per tick) unless the +0x44 target height is
 * unset (INT_MAX), in which case it steps 0x200 towards the target's +0x194. The +0x74
 * orientation looks from the target's +0x74 at the +8 position (world Y up), the +0x48 timer
 * grows by dt, and the direction from the +0x390 joint's +0x14 to the target is kept. Once the
 * timer is non-negative (bit 0 of +0x84 not yet set) the actor sends a zero-vector position
 * message (cmd 8, flag 2) and sets the bit; past 0x3a70 (bit 1 not yet set) the first free
 * +0x3ac sub-item (bit 0 of its +0x60 clear) is launched by 020ce134 along that direction and
 * the bit is set. Losing the +4 item's +0xad byte requests sub-state 2 and releases the slot. */

#include "nitro/fx_types.h"

struct hw60 { unsigned short lo : 8, hi : 8; };

extern void Mtx33_LookAt(int *out, VecFx32 *from, VecFx32 *at, const int *up);
extern void Quat_FromMtx33(int *quat, int *mtx);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void func_ov107_020c0b90(int obj, int cmd, VecFx32 v, int flag);
extern void Ov175_RelayoutAndStoreVec(int subitem, VecFx32 *from, VecFx32 *dir);
extern void SetIndexedSlot(int node, int slot, void *cb);
extern int data_02042264;
extern VecFx32 data_02041dc8;

void Ov175_HoverHoldTick(int node)
{
    int target;
    int *state = *(int **)(node + 4);
    int mtx[9];
    VecFx32 dir;
    int i;

    target = state[3];
    if (state[0x22] == 0) {
        if (state[0x11] != 0x7fffffff) {
            state[9] += (0x2800 - state[0x11]) / 30;
        } else if (*(int *)(target + 0x194) < state[9]) {
            state[9] -= 0x200;
        } else {
            state[9] += 0x200;
        }
    }
    Mtx33_LookAt(mtx, (VecFx32 *)(target + 0x74), (VecFx32 *)state[2], &data_02042264);
    Quat_FromMtx33(state + 0x1d, mtx);
    VEC_Subtract((VecFx32 *)(target + 0x74), (VecFx32 *)(*(int *)(*state + 0x390) + 0x14), &dir);
    VEC_Normalize(&dir, &dir);
    state[0x12] += *(int *)(*(int *)node + 0x2c);
    if ((*(unsigned char *)((char *)state + 0x84) & 1) == 0 && state[0x12] >= 0x1980) {
        VecFx32 v = data_02041dc8;
        func_ov107_020c0b90(*state, 8, v, 2);
        *(unsigned char *)((char *)state + 0x84) |= 1;
    }
    if ((*(unsigned char *)((char *)state + 0x84) & 2) == 0 && state[0x12] >= 0x3a70) {
        for (i = 0; i < 1; i++) {
            int sub = ((int *)*state)[0xeb + i];
            if ((((struct hw60 *)(sub + 0x60))->lo & 1) == 0) {
                Ov175_RelayoutAndStoreVec(sub, (VecFx32 *)(*(int *)(*state + 0x390) + 0x14), &dir);
                break;
            }
        }
        *(unsigned char *)((char *)state + 0x84) |= 2;
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    *(unsigned char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)(node + 0x20), 0);
}

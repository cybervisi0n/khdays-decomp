/* Grab approach finish of the ov272 enemy (x3 with ov272/ov279). The owner is set 0.94 behind the
 * +8 target (its rig yaw turned half a circle), effect 0 spawns there and reaction 0x121 mode 5
 * fires at it; the owner then faces the target from the +0x4c point, takes the target's +0x74
 * position as its own point, the +0x50 timer restarts and the tick hands over to the grab attempt
 * (Ov272_TickHoldTarget). */

#include "nitro/fx_types.h"

typedef struct { int w[4]; } Quat;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)
#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void Mtx33_LookAt(int *dst, const VecFx32 *a, const VecFx32 *b, const void *c);
extern void Quat_FromMtx33(void *dst, const int *src);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern const VecFx32 data_02042264;
extern void Ov272_TickHoldTarget(int *node);

void Ov272_GrabApproachFinish(int *node)
{
    int *state = (int *)node[1];
    int mtx[9];
    VecFx32 pos;
    int target = state[2];
    VecFx32 *tpos = (VecFx32 *)(target + 0x74);
    int yaw = (*(unsigned short *)(*(int *)(*(int *)(target + 0x18c) + 0x20) + 0x80) - 0x8000) & 0xffff;
    int rad = (int)(((long long)yaw * 0x6487f + 0x80000) >> 20);

    pos.x = tpos->x + FX_MUL(data_0203d210[ANG2IDX(rad) * 2], 0x1680);
    pos.y = tpos->y;
    pos.z = tpos->z + FX_MUL(data_0203d210[ANG2IDX(rad) * 2 + 1], 0x1680);
    func_ov107_020c0b90(*state, 0, pos, 0);
    Ov107_BuildAndSendUpdate(*state, 0x167, 5, &pos);
    {
        Mtx33_LookAt(mtx, tpos, (VecFx32 *)state[0x13], &data_02042264);
        Quat_FromMtx33(state + 7, mtx);
        *(Quat *)(state + 3) = *(Quat *)(state + 7);
        *(VecFx32 *)(state + 0x1d) = *tpos;
    }
    state[0x14] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov272_TickHoldTarget);
}

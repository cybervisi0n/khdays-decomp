/* Pounce entry of the ov272 enemy: effect 0 spawns at the +0x4c point,
 * reaction 0x167 mode 5 fires there, bit 0 of the +0x388 part's flag byte is raised and bit 1 of
 * the +0x60 high byte cleared; the +0x1c facing aims from the point at the +8 target's +0x74 and
 * is committed to +0xc, the +0x50 timer restarts and the tick hands over to Ov279_AiTimerQueueAction7. */

#include "nitro/fx_types.h"

typedef struct { int w[4]; } Quat;
typedef struct { unsigned int lo : 8, rest : 24; } Byte8;
struct hw60 { unsigned short lo : 8, hi : 8; };

extern void func_ov107_020c0b90(int actor, int a, VecFx32 v, int b);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void Mtx33_LookAt(int *dst, const VecFx32 *a, const VecFx32 *b, const void *c);
extern void Quat_FromMtx33(void *dst, const int *src);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02042264;
extern void Ov279_AiTimerQueueAction7(int *node);

void Ov279_EnterPounce(int *node)
{
    int *state = (int *)node[1];
    int mtx[9];

    func_ov107_020c0b90(*state, 0, *(VecFx32 *)state[0x13], 0);
    Ov107_BuildAndSendUpdate(*state, 0x167, 5, (void *)state[0x13]);
    ((Byte8 *)(*(int *)(*state + 0x388) + 8))->lo |= 1;
    ((struct hw60 *)(*state + 0x60))->hi &= ~2;
    Mtx33_LookAt(mtx, (VecFx32 *)(state[2] + 0x74), (VecFx32 *)state[0x13], &data_02042264);
    Quat_FromMtx33(state + 7, mtx);
    *(Quat *)(state + 3) = *(Quat *)(state + 7);
    state[0x14] = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov279_AiTimerQueueAction7);
}

/* Dash tick of the ov260 actor: while dashing (+0x7c) the +0x70 timer accumulates the frame rate,
 * the cue fires once at 0x4c8 (020cd04c 3, +0x7b bit 0), the +0x20 velocity is its +0x428 part's
 * +0x2c vector turned by the +0x64 heading and the body sweeps for hits (020cd2a0 kind 7). Once the
 * partner holds no queued move the next move is 2 and the node ends. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int m[9]; } Mtx33;
struct Flag17a { u8 b0 : 1; };

extern void MTX_RotY33_(Mtx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pIn, const Mtx33 *pMtx, VecFx32 *pOut);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov260_PlaySound(int owner, int mode, int arg);
extern void Ov260_AttackSweep(int *state, int kind, VecFx32 *sphere, void *cyl, void *seg);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern const VecFx32 data_02041dc8;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)
extern void Ov260_MapHeldItemKindToAnim(int actor, int flag);

void Ov260_DashTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 rot;

    if (state[0x1f] != 0) {
        state[0x1c] += *(int *)(node[0] + 0x2c);
        if ((*((u8 *)state + 0x7b) & 1) == 0 && state[0x1c] >= 0x4c8) {
            *((u8 *)state + 0x7b) |= 1;
            Ov260_MapHeldItemKindToAnim(*state, 3);
        }
        {
            int idx = ANG2IDX(state[0x19]) * 2;

            MTX_RotY33_(&rot, data_0203d210[idx], data_0203d210[idx + 1]);
        }
        MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x428) + 0x2c), &rot, (VecFx32 *)(state + 8));
        Ov260_AttackSweep(state, 7, (VecFx32 *)(*state + 0x74), 0, 0);
    }
    if (*(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    *(signed char *)(*state + 0x1c7) = 2;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), 0);
}

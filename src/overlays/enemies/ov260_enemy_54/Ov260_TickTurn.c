/* Turn tick of the ov260 actor: 020cd794 steers, and the +0x428 rig's offset turned by the +0x64
 * heading is kept in +0x20. Once the +4 rig is idle pose 0xe plays, the rig takes motion 6, effect 5
 * spawns in place, move 0xa starts (020cd148 with the +0x10 argument), the +0x79 flag clears and
 * 020ce9d4 runs next. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int m[9]; } Mtx33;

extern int Ov260_PickTarget(int *node);
extern void MTX_RotY33_(Mtx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pIn, const Mtx33 *pMtx, VecFx32 *pOut);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov260_PlaySound(int owner, int mode, int arg);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov260_CircleTick(void);
extern const short data_0203d210[];
extern const VecFx32 data_02041dc8;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov260_TickTurn(int *node)
{
    int *state = (int *)node[1];
    Mtx33 rot;

    Ov260_PickTarget(node);
    {
        int idx = ANG2IDX(state[0x19]) * 2;

        MTX_RotY33_(&rot, data_0203d210[idx], data_0203d210[idx + 1]);
    }
    MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x428) + 0x2c), &rot, (VecFx32 *)(state + 8));
    if (*(unsigned char *)(state[1] + 0xad) == 0) {
        Ov107_PostTagUpdate((Actor *)(*state), 0xe, 0);
        Ov107_StartAnim(*(int *)(*state + 0x428), 6, 0);
        func_ov107_020c0b90(*state, 5, data_02041dc8, 0);
        Ov260_PlaySound(*state, 0xa, state[4]);
        *((unsigned char *)state + 0x79) = 0;
        SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_CircleTick);
        return;
    }
}

/* Recoil fall tick of the ov260 actor: +0x20 follows the +0x2c velocity, which takes the +0x428
 * part's +0x30 lift (height and fall speed) when it has one, and the body sweeps for hits around its
 * +0x74 position (020cd2a0 kind 4). On landing (+0x17a bit 0) the +0x54 impact point is its ground
 * point lowered by the +0x80 radius, bit 6 of the +0x60 high byte drops, it is knocked back there
 * (mode 2), effect 0xd starts there, pose 0x1c plays, +0x70 and the +0x79 / +0x7b flags clear and the
 * node moves on to 020cfa38. */

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
extern void Ov260_StompTick_2(void);

void Ov260_TickRecoilFall(int *node)
{
    int *state = (int *)node[1];
    int lift = *(int *)(*(int *)(*state + 0x428) + 0x30);

    *(VecFx32 *)(state + 8) = *(VecFx32 *)(state + 0xb);
    if (lift != 0) {
        state[0xc] = lift;
        state[9] = lift;
    }
    Ov260_AttackSweep(state, 4, (VecFx32 *)(*state + 0x74), 0, 0);
    if (!(!((struct Flag17a *)(*state + 0x17a))->b0)) {
        {
            VecFx32 *land = (VecFx32 *)(state + 0x15);

            *land = *(VecFx32 *)(*state + 0x180);
            state[0x16] -= *(int *)(*state + 0x80);
            {
                u16 hw = *(u16 *)(*state + 0x60);
                *(u16 *)(*state + 0x60) = (hw & ~0xff00) |
                    (((unsigned int)(u16)((((unsigned int)hw << 0x10) >> 0x18) & ~0x40) << 0x18) >> 0x10);
            }
            func_ov107_020c0b90(*state, 2, *land, 0);
            Ov260_PlaySound(*state, 0xd, (int)(state + 0x15));
            Ov107_PostTagUpdate((Actor *)(*state), 0x1c, 0);
            state[0x1c] = 0;
            *((u8 *)state + 0x79) = 0;
            *((u8 *)state + 0x7b) = 0;
            SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_StompTick_2);
        }
        return;
    }
}

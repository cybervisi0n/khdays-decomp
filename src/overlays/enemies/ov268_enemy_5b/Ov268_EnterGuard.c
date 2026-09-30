/* Guard entry of the ov268 enemy (x3 with ov209/ov268): raises bit 7 and clears bit 0 of the
 * owner's +0x60 high byte, clears bit 0 of the +0x388 sub-item's +8 low byte, sends the owner
 * commands 2 and 3 with the zero vector, fires reaction 0x15f mode 0xd at the +8 point, keeps the
 * +0x394 partner's sub-state in the +0x44 byte, clears the +0x40 timer, keeps the +8 point at
 * +0x24, clears the +0x45 byte and hands the tick over to Ov268_ItemSweepTick. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct hw60 { unsigned short lo : 8, hi : 8; };
struct w8 { unsigned int lo : 8, rest : 24; };

extern void func_ov107_020c0b90(int owner, int mode, VecFx32 v, int flag);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const VecFx32 data_02041dc8;
extern void Ov268_ItemSweepTick(int *node);

void Ov268_EnterGuard(int *node)
{
    int *state = (int *)node[1];
    VecFx32 zero = data_02041dc8;
    u16 v;

    v = *(u16 *)(*state + 0x60);
    *(u16 *)(*state + 0x60) = (u16)((v & ~0xff00) | (((((unsigned int)v << 0x10) >> 0x18 | 0x80) << 0x18) >> 0x10));
    ((struct hw60 *)(*state + 0x60))->hi &= ~1;
    ((struct w8 *)(*(int *)(*state + 0x388) + 8))->lo &= ~1;
    func_ov107_020c0b90(*state, 2, zero, 0);
    func_ov107_020c0b90(*state, 3, zero, 0);
    Ov107_BuildAndSendUpdate(*state, 0x15f, 0xd, (void *)state[2]);
    *(unsigned char *)((char *)state + 0x44) = *(signed char *)(*(int *)(*state + 0x394) + 0x1c6);
    state[0x10] = 0;
    *(VecFx32 *)(state + 9) = *(VecFx32 *)state[2];
    *(unsigned char *)((char *)state + 0x45) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), (void *)Ov268_ItemSweepTick);
}

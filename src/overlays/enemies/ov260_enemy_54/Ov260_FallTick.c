/* Fall tick of the ov260 actor: the +0x20 velocity is its +0x428 part's +0x2c vector turned by the
 * +0x64 heading, the +0x24 height takes the +0x30 fall speed which grows by 1/32 each frame. Landing
 * (+0x17a bit 0) or a partner with no queued move plays pose 0x10 and moves on to 020ced8c. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Flag17a { u8 b0 : 1; };
typedef struct { int m[9]; } Mtx33;

extern void MTX_RotY33_(Mtx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pIn, const Mtx33 *pMtx, VecFx32 *pOut);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov260_TickFall(void);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov260_FallTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 rot;

    {
        int idx = ANG2IDX(state[0x19]) * 2;

        MTX_RotY33_(&rot, data_0203d210[idx], data_0203d210[idx + 1]);
    }
    MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x428) + 0x2c), &rot, (VecFx32 *)(state + 8));
    state[9] += state[0xc];
    state[0xc] -= 0x80;
    if (!((struct Flag17a *)(*state + 0x17a))->b0 && *(u8 *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x10, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_TickFall);
}

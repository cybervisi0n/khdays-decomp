/* Glide tick of the ov260 actor: the +0x20 velocity is its +0x428 part's +0x2c vector turned by the
 * +0x64 heading; once the partner holds no queued move pose 0x13 plays, the part takes motion 9,
 * +0x70 and the +0x7b / +0x78 flags clear and the node moves on to 020ce3dc. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int m[9]; } Mtx33;

extern void MTX_RotY33_(Mtx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pIn, const Mtx33 *pMtx, VecFx32 *pOut);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov260_GlideComboTick(void);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov260_GlideTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 rot;

    {
        int idx = ANG2IDX(state[0x19]) * 2;

        MTX_RotY33_(&rot, data_0203d210[idx], data_0203d210[idx + 1]);
    }
    MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x428) + 0x2c), &rot, (VecFx32 *)(state + 8));
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0x13, 0);
    Ov107_StartAnim(*(int *)(*state + 0x428), 9, 0);
    state[0x1c] = 0;
    *((unsigned char *)state + 0x7b) = 0;
    *((unsigned char *)state + 0x78) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov260_GlideComboTick);
}

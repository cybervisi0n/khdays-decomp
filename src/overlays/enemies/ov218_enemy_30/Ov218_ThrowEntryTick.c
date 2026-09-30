/* Throw entry tick of the ov218 actor: the +0x28 velocity is its +0x3ac part's +0x2c vector turned by
 * the +0xc heading; once the partner holds no queued move pose 9 loops, +0x14 and +0x40 clear and the
 * node moves on to 020cdd44. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int m[9]; } Mtx33;

extern void MTX_RotY33_(Mtx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pIn, const Mtx33 *pMtx, VecFx32 *pOut);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov218_StaggerTick(void);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov218_ThrowEntryTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 rot;

    {
        int idx = ANG2IDX(state[3]) * 2;

        MTX_RotY33_(&rot, data_0203d210[idx], data_0203d210[idx + 1]);
    }
    MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x3ac) + 0x2c), &rot, (VecFx32 *)(state + 0xa));
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 9, 1);
    state[5] = 0;
    *((unsigned char *)state + 0x40) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov218_StaggerTick);
}

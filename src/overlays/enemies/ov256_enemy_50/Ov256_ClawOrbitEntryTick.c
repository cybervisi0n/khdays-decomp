/* Orbit entry tick of an ov256 claw: the +0x10 velocity is the +0x390 part's +0x2c vector turned by
 * the +0x54 heading; after 0x908 of the +0x60 timer it aims (+0x50 = 020d1b2c) and the +0x34 target
 * takes the +0xc anchor. The timer runs and the claw moves (020d1400 1, 3); once the part's animation
 * ends the orbit count +0x64 = 1, the timer and the +0x6c flag clear, the part takes motion 4 and the
 * node moves on to 020d1d88. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int m[9]; } Mtx33;

extern void MTX_RotY33_(Mtx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pIn, const Mtx33 *pMtx, VecFx32 *pOut);
extern int Ov256_HelperOrbitHeading(int *node);
extern void Ov256_AttackHitTestB(int *node, int a, int b);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern const short data_0203d210[];
extern void Ov256_ClawOrbitTick(void);

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov256_ClawOrbitEntryTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 rot;

    {
        int idx = ANG2IDX(state[0x15]) * 2;

        MTX_RotY33_(&rot, data_0203d210[idx], data_0203d210[idx + 1]);
    }
    MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x390) + 0x2c), &rot, (VecFx32 *)(state + 4));
    if (state[0x18] >= 0x908) {
        state[0x14] = Ov256_HelperOrbitHeading(node);
        *(VecFx32 *)(state + 0xd) = *(VecFx32 *)state[3];
    }
    state[0x18] += *(int *)(node[0] + 0x2c);
    Ov256_AttackHitTestB(node, 1, 3);
    if (*(u8 *)(*(int *)(*(int *)(*state + 0x390) + 0x3c) + 0xad) != 0) {
        return;
    }
    state[0x19] = 1;
    state[0x18] = 0;
    *((u8 *)state + 0x6c) = 0;
    Ov107_StartAnim(*(int *)(*state + 0x390), 4, 0);
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov256_ClawOrbitTick);
}

/* Swing tick of the ov230 actor: the +0x4c clock runs up at the frame rate and the +0x490 bone's
 * offset, turned by the +0x40 heading, is kept in +0x10; a hit found by 020d2d90 clears +8. Once the
 * +4 rig is idle pose 0xe plays, move 0xb starts (020d3028), the +0x62 flag clears and brain
 * slot +0x20 runs 020d4298. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int m[9]; } Mtx33;

extern void MTX_RotY33_(Mtx33 *pMtx, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *pIn, const Mtx33 *pMtx, VecFx32 *pOut);
extern int Ov230_ContactCheck(int *state);
extern void Ov230_startAnim(int owner, int anim);
extern void SetIndexedSlot(int *node, int slot, void *cb);
extern void Ov230_SlamSweepTick(void);
extern const short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov230_SwingTick(int *node)
{
    int *state = (int *)node[1];
    Mtx33 rot;

    state[0x13] += *(int *)(node[0] + 0x2c);
    {
        int idx = ANG2IDX(state[0x10]) * 2;

        MTX_RotY33_(&rot, data_0203d210[idx], data_0203d210[idx + 1]);
    }
    MTX_MultVec33((VecFx32 *)(*(int *)(*state + 0x490) + 0x2c), &rot, (VecFx32 *)(state + 4));
    if (Ov230_ContactCheck(state) != 0) {
        state[2] = 0;
    }
    if (*(unsigned char *)(state[1] + 0xad) != 0) {
        return;
    }
    Ov107_PostTagUpdate((Actor *)(*state), 0xe, 0);
    Ov230_startAnim(*state, 0xb);
    *((unsigned char *)state + 0x62) = 0;
    SetIndexedSlot(node, *(signed char *)((char *)node + 0x20), Ov230_SlamSweepTick);
}

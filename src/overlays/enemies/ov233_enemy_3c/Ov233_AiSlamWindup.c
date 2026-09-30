/* Ov233_AiSlamWindup -- the steering tick that leads into Ov233_AiSlamLeap. Structurally the
 * same as that one: age the timer at +0x4c, re-aim, and once ctx[1]+0xad clears fire the hand-off
 * modes and re-enter. Only the modes differ (0xd/0xa here, 0xe/0xb there) and this one does not
 * touch the byte at +0x62.
 *
 * The aim is the family's Y-rotation steering: turn the Q12-radians heading at +0x40 into sin/cos
 * via data_0203d210 (see codegen-cracks.md), build a MtxFx33, and rotate the owner's offset
 * vector into ctx+0x10. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct {
    int m[9];
} MtxFx33;

extern void MTX_RotY33_(MtxFx33 *mtx, int sinVal, int cosVal);
extern void MTX_MultVec33(const VecFx32 *v, const MtxFx33 *m, VecFx32 *dst);
extern void Ov233_startAnim(int owner, int a);
extern void SetIndexedSlot(int self, int action, void *cb);
extern void Ov233_AiSlamLeap(void);
extern short data_0203d210[];

void Ov233_AiSlamWindup(int self) {
    int *ctx;
    MtxFx33 mtx;
    int idx;

    ctx = *(int **)(self + 4);
    ctx[0x13] += *(int *)(*(int *)self + 0x2c);

    idx = (unsigned short)(((long long)ctx[0x10] * 0x28be60db9391LL + 0x80000000000LL) >> 44)
          >> 4;
    MTX_RotY33_(&mtx, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
    MTX_MultVec33((const VecFx32 *)(*(int *)(ctx[0] + 0x490) + 0x2c), &mtx,
                  (VecFx32 *)((char *)ctx + 0x10));

    if (*(unsigned char *)(ctx[1] + 0xad) != 0) {
        return;
    }

    Ov107_PostTagUpdate((Actor *)ctx[0], 0xd, 0);
    Ov233_startAnim(ctx[0], 0xa);
    SetIndexedSlot(self, *(signed char *)(self + 0x20), Ov233_AiSlamLeap);
}

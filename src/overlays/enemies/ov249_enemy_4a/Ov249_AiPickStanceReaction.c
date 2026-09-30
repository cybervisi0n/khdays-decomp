/* Ov249_AiPickStanceReaction -- pick the reaction for the current stance, or fall through to steering.
 *
 * Ov249_MeasureTargetGap returning negative aborts to a plain c634 re-entry.
 *
 * When the flag at ctx[1]+0xad is SET the function is just the Y-rotation steering tail shared
 * with Ov228_AiStanceMoveFinish: build a MtxFx33 from the heading at +0x40 and rotate the owner's
 * offset vector into ctx+0x10. (See codegen-cracks.md for the Q12-radians -> sin/cos conversion.)
 *
 * When it is clear, the stance at +0x5c selects a (mode, arg) pair to fire -- 0 -> (3, 2),
 * 2 -> (9, 7), 3 -> (6, 5) -- and then continues into Ov249_AiStanceMoveTick. Any other stance
 * parks 2 in ctx[0]+0x1c7 and re-enters with no callback, i.e. gives up. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct {
    int m[9];
} MtxFx33;

extern int Ov249_MeasureTargetGap(int self);
extern void SetIndexedSlot(int self, int action, void *cb);
extern void Ov249_startAnim(int owner, int a);
extern void Ov249_AiStanceMoveTick(void);
extern void MTX_RotY33_(MtxFx33 *mtx, int sinVal, int cosVal);
extern void MTX_MultVec33(const VecFx32 *v, const MtxFx33 *m, VecFx32 *dst);
extern short data_0203d210[];

void Ov249_AiPickStanceReaction(int self) {
    int *ctx;
    MtxFx33 mtx;
    int idx;

    ctx = *(int **)(self + 4);
    if (Ov249_MeasureTargetGap(self) < 0) {
        SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
        return;
    }

    if (*(unsigned char *)(ctx[1] + 0xad) == 0) {
        switch (ctx[0x17]) {
        case 0:
            Ov107_PostTagUpdate((Actor *)ctx[0], 3, 0);
            Ov249_startAnim(ctx[0], 2);
            break;
        case 2:
            Ov107_PostTagUpdate((Actor *)ctx[0], 9, 0);
            Ov249_startAnim(ctx[0], 7);
            break;
        case 3:
            Ov107_PostTagUpdate((Actor *)ctx[0], 6, 0);
            Ov249_startAnim(ctx[0], 5);
            break;
        default:
            *(unsigned char *)(ctx[0] + 0x1c7) = 2;
            SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
            return;
        }
        SetIndexedSlot(self, *(signed char *)(self + 0x20), Ov249_AiStanceMoveTick);
        return;
    }

    idx = (unsigned short)(((long long)ctx[0x10] * 0x28be60db9391LL + 0x80000000000LL) >> 44)
          >> 4;
    MTX_RotY33_(&mtx, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
    MTX_MultVec33((const VecFx32 *)(*(int *)(ctx[0] + 0x490) + 0x2c), &mtx,
                  (VecFx32 *)((char *)ctx + 0x10));
}

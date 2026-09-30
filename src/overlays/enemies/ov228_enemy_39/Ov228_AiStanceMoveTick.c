/* Ov228_AiStanceMoveTick -- the wait loop of the approach: hold the stance for a few ticks, then
 * commit. This is the other half of Ov228_AiPickStanceReaction, which enters here; from here the object
 * either loops back into 020cfeac or moves on to Ov228_AiStanceMoveFinish.
 *
 * Ov228_MeasureTargetGap returns the current range (negative aborts). Inside 0x1000 there is a 45%
 * chance per tick of breaking off early: stance 1, 4 parked in ctx[0]+0x1c7, no callback.
 *
 * Otherwise the retry counter at +0x50 ticks down; while it is still positive the object
 * re-enters 020cfeac and the loop goes round again. When it runs out, the stance at +0x5c picks a
 * parting mode -- 0 -> (4, 3), 2 -> 0xa, 3 -> 7, anything else nothing -- and the object commits
 * to Ov228_AiStanceMoveFinish.
 *
 * As everywhere in this family, a set ctx[1]+0xad flag means "not ready yet" and the whole tick
 * degenerates into the Y-rotation steering tail (see codegen-cracks.md for the Q12-radians
 * conversion). */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

typedef struct {
    int m[9];
} MtxFx33;

extern int Ov228_MeasureTargetGap(int self);
extern void SetIndexedSlot(int self, int action, void *cb);
extern void Ov228_startAnim(int owner, int a);
extern void Ov228_AiPickStanceReaction(void);
extern void Ov228_AiStanceMoveFinish(void);
extern void MTX_RotY33_(MtxFx33 *mtx, int sinVal, int cosVal);
extern void MTX_MultVec33(const VecFx32 *v, const MtxFx33 *m, VecFx32 *dst);
extern short data_0203d210[];

void Ov228_AiStanceMoveTick(int self) {
    int *ctx;
    MtxFx33 mtx;
    int range;
    int idx;

    ctx = *(int **)(self + 4);
    range = Ov228_MeasureTargetGap(self);
    if (range < 0) {
        SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
        return;
    }

    if (*(unsigned char *)(ctx[1] + 0xad) == 0) {
        if (range < 0x1000 && (unsigned int)RandNextScaled(100) < 0x2d) {
            ctx[0x17] = 1;
            *(unsigned char *)(ctx[0] + 0x1c7) = 4;
            SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
            return;
        }

        if (--ctx[0x14] > 0) {
            SetIndexedSlot(self, *(signed char *)(self + 0x20), Ov228_AiPickStanceReaction);
            return;
        }

        switch (ctx[0x17]) {
        case 0:
            Ov107_PostTagUpdate((Actor *)ctx[0], 4, 0);
            Ov228_startAnim(ctx[0], 3);
            break;
        case 2:
            Ov107_PostTagUpdate((Actor *)ctx[0], 0xa, 0);
            break;
        case 3:
            Ov107_PostTagUpdate((Actor *)ctx[0], 7, 0);
            break;
        }

        SetIndexedSlot(self, *(signed char *)(self + 0x20), Ov228_AiStanceMoveFinish);
        return;
    }

    idx = (unsigned short)(((long long)ctx[0x10] * 0x28be60db9391LL + 0x80000000000LL) >> 44)
          >> 4;
    MTX_RotY33_(&mtx, data_0203d210[idx * 2], data_0203d210[idx * 2 + 1]);
    MTX_MultVec33((const VecFx32 *)(*(int *)(ctx[0] + 0x490) + 0x2c), &mtx,
                  (VecFx32 *)((char *)ctx + 0x10));
}

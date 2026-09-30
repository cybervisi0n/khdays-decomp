/* Ov228_AiSlamLeap -- the steering tick: age the timer, re-aim, and once the owner is ready
 * hand off to Ov228_SlamSweepTick.
 *
 * The timer at +0x4c accumulates (*self)[0x2c] every tick (nothing here reads it back; the
 * consumer is elsewhere in the state machine).
 *
 * The aim is the Y-rotation steering shared with Ov228_AiStanceMoveFinish: turn the Q12-radians
 * heading at +0x40 into sin/cos via data_0203d210 (see codegen-cracks.md), build a MtxFx33, and
 * rotate the owner's offset vector into ctx+0x10.
 *
 * While the flag at ctx[1]+0xad is still set the tick ends there and repeats. Once it clears, the
 * hand-off fires modes 0xe/0xb, clears the byte at +0x62 and re-enters at Ov228_SlamSweepTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct {
    int m[9];
} MtxFx33;

extern void MTX_RotY33_(MtxFx33 *mtx, int sinVal, int cosVal);
extern void MTX_MultVec33(const VecFx32 *v, const MtxFx33 *m, VecFx32 *dst);
extern void Ov228_startAnim(int owner, int a);
extern void SetIndexedSlot(int self, int action, void *cb);
extern void Ov228_SlamSweepTick(void);
extern short data_0203d210[];

void Ov228_AiSlamLeap(int self) {
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

    Ov107_PostTagUpdate((Actor *)ctx[0], 0xe, 0);
    Ov228_startAnim(ctx[0], 0xb);
    *(unsigned char *)((char *)ctx + 0x62) = 0;
    SetIndexedSlot(self, *(signed char *)(self + 0x20), Ov228_SlamSweepTick);
}

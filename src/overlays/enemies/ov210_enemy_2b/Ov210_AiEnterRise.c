/* Ov210_AiEnterRise -- enter the state: play mode 0x15, seed the offset vector at ctx+0x54 by
 * scaling the constant axis data_02042264 by 0x1200, clear the counter at +0x2c and hand off to
 * Ov210_AiRiseTick. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void SetIndexedSlot(int self, int action, void *cb);
extern void Ov210_AiRiseTick(void);
extern VecFx32 data_02042264;

void Ov210_AiEnterRise(int self) {
    int *ctx;

    ctx = *(int **)(self + 4);
    Ov107_PostTagUpdate((Actor *)ctx[0], 0x15, 0);
    ScaleVec3Fx12(0x1200, &data_02042264, (VecFx32 *)((char *)ctx + 0x54));
    ctx[0xb] = 0;
    SetIndexedSlot(self, *(signed char *)(self + 0x20), Ov210_AiRiseTick);
}

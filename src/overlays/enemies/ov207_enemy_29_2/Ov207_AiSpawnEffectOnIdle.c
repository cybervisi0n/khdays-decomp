/* Ov207_AiSpawnEffectOnIdle -- reset the tuning, quiesce the owner and queue the effect, then hand on.
 * The constants at data_02041dc8 are copied into +0x14 unconditionally. Nothing further happens
 * while the gate byte at *(+0xc) is set.
 * Otherwise the owner is quiesced (mode 3), the progress fields (+0x24/+0x52) cleared, and a
 * 4-byte descriptor is queued through MsgQueue_Post -- built from data_ov207_020d41e8's +4/+6,
 * except the low half is then overwritten with the owner's own id (+2), so only the high half of
 * the global actually survives. Finally the caller's action (+0x20) is dispatched through
 * SetIndexedSlot with Ov207_FallTick as the continuation. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct {
    unsigned short lo;
    unsigned short hi;
} Ov206_EffectDesc;

extern void MsgQueue_Post(int a, Ov206_EffectDesc *desc, int n, int v);
extern void SetIndexedSlot(int self, int action, void (*cb)(void));
extern void Ov207_FallTick(void);
extern VecFx32 data_02041dc8;
extern unsigned short data_ov207_020d41e8[];

void Ov207_AiSpawnEffectOnIdle(int self) {
    int *ctx;
    Ov206_EffectDesc desc;
    unsigned short id;

    ctx = *(int **)(self + 4);
    *(VecFx32 *)((char *)ctx + 0x14) = data_02041dc8;
    if (**(unsigned char **)(ctx + 3) != 0) {
        return;
    }

    Ov107_PostTagUpdate((Actor *)ctx[0], 3, 0);
    ctx[9] = 0;
    *(unsigned char *)((char *)ctx + 0x52) = 0;

    desc.hi = data_ov207_020d41e8[3];
    desc.lo = data_ov207_020d41e8[2];
    id = *(unsigned short *)(ctx[0] + 2);
    desc.lo = id;
    MsgQueue_Post(1, &desc, 4, id);

    SetIndexedSlot(self, *(signed char *)(self + 0x20), Ov207_FallTick);
}

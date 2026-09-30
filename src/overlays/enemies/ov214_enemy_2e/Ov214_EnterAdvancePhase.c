/*
 * Enter the "advance" phase: kick animation mode 6, convert a fresh atan2 (RandNext) to a
 * sin/cos facing in node[5..7], copy the constant offset vector data_02041dc8 and publish it to the
 * owner via modes 2 and 4, clear the one-shot bytes and phase timers, seed the reach at +0x5c to
 * 0x5000, and chain to the next state Ov214_stActivateWhenReady.
 *
 * `const short data_0203d210[]` is load-bearing: without const mwcc must assume the node stores
 * could write the sin/cos table, which reorders a table load; const is the aliasing fact that
 * matches. One of a 5-member family (ov215/216/217/264); only the chained-state symbol differs.
 */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern const short data_0203d210[];
extern VecFx32 data_02041dc8;
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 v, int c);
extern void SetIndexedSlot(int self, int idx, void *cb);
extern void Ov214_stActivateWhenReady(void);

void Ov214_EnterAdvancePhase(int param_1) {
    int *node = *(int **)(param_1 + 4);
    int idx;
    VecFx32 v;
    Ov107_PostTagUpdate((Actor *)(*node), 6, 0);
    idx = (int)(((unsigned)(((long long)RandNext() * 0x28be60db9391LL + 0x80000000000LL) >> 0x20) << 4) >> 0x10) >> 4;
    node[5] = data_0203d210[idx * 2];
    node[6] = 0;
    node[7] = data_0203d210[idx * 2 + 1];
    v = data_02041dc8;
    func_ov107_020c0b90(*node, 2, v, 0);
    func_ov107_020c0b90(*node, 4, v, 0);
    *(char *)((char *)node + 0x70) = 0;
    *(char *)((char *)node + 0x71) = 0;
    *(char *)((char *)node + 0x73) = 0;
    *(char *)((char *)node + 0x6e) = 0;
    *(char *)((char *)node + 0x6f) = RandNextScaled(0xb) + 0x14;
    *(char *)((char *)node + 0x74) = 0;
    *(int *)((char *)node + 0x58) = 0;
    *(int *)((char *)node + 0x78) = 0;
    *(int *)((char *)node + 0x5c) = 0x5000;
    *(int *)((char *)node + 0x60) = 0;
    SetIndexedSlot(param_1, *(signed char *)((char *)param_1 + 0x20), &Ov214_stActivateWhenReady);
}

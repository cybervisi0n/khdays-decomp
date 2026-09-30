/* Settle-pose hold of the ov178 enemy (x3: ov178/179/180): plays pose 1 while the actor's +0xad
 * flag is clear; every 0x800 of the node's +0x2c speed it sweeps a 0x2000 sphere around the
 * actor's +0x74 position through the pool's collision query and applies hit 1 (push from the
 * zero vector, flags 0x10) to everything it finds. The +0xc timer runs to 0xa000 -- or ends
 * early when the pool's +0x60 low byte has bit 7 set -- and then the target position (+8) is
 * pushed to the render hook (cmd 1), pose 2 plays and the release handler (020cee40) follows. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[4]; } Vec4;
struct hw60 { unsigned short lo : 8, hi : 8; };

extern int Ov107_CollectSphereOverlaps(int owner, Vec4 *src, int *out);
extern int Ov107_InvokeHitCallback(int victim, int a, int b, int mode, void *push, int flags);
extern void func_ov107_020c0b90(int obj, int cmd, VecFx32 v, int flag);
extern void SetIndexedSlot(int obj, int slot, void *cb);
extern VecFx32 data_02041dc8;
extern void Ov178_AiStep_QueueAction0OnAnimEnd(void);

void Ov178_SettleHold(int node) {
    int *state = *(int **)(node + 4);
    Vec4 sphere;
    int hits[4];
    int i;
    int n;

    if (*(unsigned char *)(state[1] + 0xad) == 0) {
        Ov107_PostTagUpdate((Actor *)(*state), 1, 1);
    }
    state[4] += *(int *)(*(int *)node + 0x2c);
    if (state[4] >= 0x800) {
        state[4] = 0;
        sphere = *(Vec4 *)(*state + 0x74);
        sphere.w[3] = 0x2000;
        n = Ov107_CollectSphereOverlaps(*(int *)(*state + 0x388), &sphere, hits);
        for (i = 0; i < n; i++) {
            Ov107_InvokeHitCallback(hits[i], *state, *(int *)(*state + 0x388), 1, &data_02041dc8, 0x10);
        }
    }
    state[3] += *(int *)(*(int *)node + 0x2c);
    if (state[3] < 0xa000 && (((struct hw60 *)(*(int *)(*state + 0x388) + 0x60))->lo & 0x80) == 0) {
        return;
    }
    {
        VecFx32 v = *(VecFx32 *)state[2];
        func_ov107_020c0b90(*(int *)(*state + 0x388), 1, v, 0);
    }
    Ov107_PostTagUpdate((Actor *)(*state), 2, 0);
    SetIndexedSlot(node, *(signed char *)(node + 0x20), Ov178_AiStep_QueueAction0OnAnimEnd);
}

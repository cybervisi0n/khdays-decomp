/* When the effect node is in state 1, places it at the locked target's point turned with the
 * character and draws it. */

#include "nitro/fx_types.h"

extern void func_ov022_020ad44c(void *out, int self);
extern void Scene_DrawNode(int a);

typedef struct {
    char pad0[0x24];
    int kind;
    unsigned short flags;
    char pad2a[0x7a];
    unsigned short angle;
    char pada6[0x26];
    VecFx32 vec;
} Node;

typedef struct { char pad0[0x100]; Node node; } Block;

void Ov079_DrawSubNodeWithYaw(int self, Block *b) {
    VecFx32 v;
    if (b->node.kind != 1) return;
    func_ov022_020ad44c(&v, self);
    b->node.angle = (unsigned short)(*(unsigned short *)(*(int *)(self + 0x20) + 0x80) - 0x8000) + 0x8000;
    b->node.flags |= 0x20;
    b->node.vec = v;
    Scene_DrawNode((int)&b->node.flags);
}

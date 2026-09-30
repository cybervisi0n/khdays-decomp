/* When the sub-node is in its visible state, turns it with the character, places it at the
 * character's anchor and draws it. */

#include "nitro/fx_types.h"

extern void Scene_DrawNode(int a);

typedef struct {
    int kind;
    unsigned short flags;
    char pad6[0x7a];
    unsigned short angle;
    char pad82[0x24];
    VecFx32 vec;
} Node;

typedef struct { char pad[0x11c]; Node node; } Block;

void Ov037_SetYawAndDrawSubNode(int self, Block *b) {
    if (b->node.kind != 2) return;
    b->node.angle = (unsigned short)(*(unsigned short *)(*(int *)(self + 0x20) + 0x80) - 0x8000) + 0x8000;
    b->node.flags |= 0x20;
    b->node.vec = *(VecFx32 *)(self + 0x104 + 0x800);
    Scene_DrawNode((int)&b->node.flags);
}

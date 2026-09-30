/* When the effect node is in one of its visible states, places it at the character's position
 * turned with the character and draws it. */

#include "nitro/fx_types.h"

extern void Scene_DrawNode(int a);

typedef struct {
    char pad0[0x4c];
    unsigned short flags;
    char pad4e[0x7a];
    unsigned short angle;
    char padca[0x26];
    VecFx32 vec;
    char padfc[0x158 - 0xfc];
    int kind;
} Node;

typedef struct { char pad0[0x200]; Node node; } Block;

void Ov048_DrawSubNodeWithYaw3(int self, Block *b) {
    VecFx32 v;
    if (b->node.kind != 2 && b->node.kind != 3 && b->node.kind != 4) return;
    v = *(VecFx32 *)(self + 0x8c + 0x400);
    b->node.angle = (unsigned short)(*(unsigned short *)(*(int *)(self + 0x20) + 0x80) - 0x8000) + 0x8000;
    b->node.flags |= 0x20;
    b->node.vec = v;
    Scene_DrawNode((int)&b->node.flags);
}

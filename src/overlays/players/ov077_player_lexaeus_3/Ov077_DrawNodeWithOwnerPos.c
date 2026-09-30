/* When the effect node is in state 1, places it at the character's position turned with the
 * character and draws it. */

#include "nitro/fx_types.h"

extern void Scene_DrawNode(int a);

typedef struct {
    char pad0[0xc];
    int kind;
    unsigned short flags;
    char pad12[0x7a];
    unsigned short angle;
    char pad8e[0x26];
    VecFx32 vec;
} Node;

void Ov077_DrawNodeWithOwnerPos(int self, Node *node) {
    VecFx32 v;
    if (node->kind != 1) return;
    v = *(VecFx32 *)(self + 0x8c + 0x400);
    node->angle = (unsigned short)(*(unsigned short *)(*(int *)(self + 0x20) + 0x80) - 0x8000) + 0x8000;
    node->flags |= 0x20;
    node->vec = v;
    Scene_DrawNode((int)&node->flags);
}

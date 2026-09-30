/* When the effect node is in its visible state, turns it with the character, places it at the
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

void Ov080_SetYawAndDrawNode(int self, Node *node) {
    if (node->kind != 2) return;
    node->angle = (unsigned short)(*(unsigned short *)(*(int *)(self + 0x20) + 0x80) - 0x8000) + 0x8000;
    node->flags |= 0x20;
    node->vec = *(VecFx32 *)(self + 0x104 + 0x800);
    Scene_DrawNode((int)&node->flags);
}

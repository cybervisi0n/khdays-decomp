/* When the effect node is in one of its visible states, places it at the character's position
 * turned with the character and draws it. */

#include "nitro/fx_types.h"

extern void Scene_DrawNode(int a);

typedef struct {
    char pad0[4];
    unsigned short flags;
    char pad6[0x7a];
    unsigned short angle;
    char pad82[0x26];
    VecFx32 vec;
    char padb4[0x110 - 0xb4];
    int kind;
} Node;

void Ov046_DrawNodeWithOwnerPos3(int self, Node *node) {
    VecFx32 v;
    if (node->kind != 2 && node->kind != 3 && node->kind != 4) return;
    v = *(VecFx32 *)(self + 0x8c + 0x400);
    node->angle = (unsigned short)(*(unsigned short *)(*(int *)(self + 0x20) + 0x80) - 0x8000) + 0x8000;
    node->flags |= 0x20;
    node->vec = v;
    Scene_DrawNode((int)&node->flags);
}

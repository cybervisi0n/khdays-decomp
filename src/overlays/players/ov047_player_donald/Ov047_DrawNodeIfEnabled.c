/* While the character is shown and the effect node is visible, places it at the character's
 * position turned with the character and draws it. */

#include "nitro/fx_types.h"

extern void Scene_DrawNode(int a);

typedef struct { unsigned char b0 : 1; } Flags;

typedef struct {
    char pad0[0x10];
    int kind;
    unsigned short flags;
    char pad16[0x7a];
    unsigned short angle;
    char pad92[0x26];
    VecFx32 vec;
} Node;

void Ov047_DrawNodeIfEnabled(int self, Node *node) {
    VecFx32 v;
    if (!((Flags *)(self + 0x694))->b0) return;
    if (node->kind != 2) return;
    v = *(VecFx32 *)(self + 0x8c + 0x400);
    node->angle = (unsigned short)(*(unsigned short *)(*(int *)(self + 0x20) + 0x80) - 0x8000) + 0x8000;
    node->flags |= 0x20;
    node->vec = v;
    Scene_DrawNode((int)&node->flags);
}

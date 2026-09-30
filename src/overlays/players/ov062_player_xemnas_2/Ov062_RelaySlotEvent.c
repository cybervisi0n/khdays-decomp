/* Slot-event relay of the mission enemy: when the actor's +0x694 bit 0 is set and the node is
 * of kind 2, 3 or 4, samples the actor's anchor position, raises it 0x800, faces the node the
 * model's way (heading +0x80 flipped by 0x8000), flags it 0x20 and stores the position at +0xb4
 * before re-registering the node. Then the 2 requests at +0x12c (stride 0x240) are stepped
 * through Ov062_DrawNodesWhileActive. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct {
    char pad0[0x10];
    u16 flags;
    char pad12[0x7a];
    u16 angle;
    char pad8e[0x26];
    VecFx32 vec;
    char padc0[0x11c - 0xc0];
    int kind;
} Node;

struct b1 { u8 b0 : 1; };

extern void func_ov022_020ad44c(VecFx32 *out, char *self);
extern void Scene_DrawNode(void *node);
extern void Ov062_DrawNodesWhileActive(char *req);

void Ov062_RelaySlotEvent(char *self, Node *node)
{
    VecFx32 pos;
    int i;
    char *req;

    if (((struct b1 *)(self + 0x694))->b0 == 0) {
        return;
    }
    if (!(node->kind != 2 && node->kind != 3 && node->kind != 4)) {
        func_ov022_020ad44c(&pos, self);
        pos.y += 0x800;
        node->angle = (u16)(*(u16 *)(*(char **)(self + 0x20) + 0x80) - 0x8000) + 0x8000;
        node->flags |= 0x20;
        node->vec = pos;
        Scene_DrawNode(&node->flags);
    }
    req = (char *)node + 0x12c;
    for (i = 0; i < 2; i++) {
        Ov062_DrawNodesWhileActive(req);
        req += 0x240;
    }
}

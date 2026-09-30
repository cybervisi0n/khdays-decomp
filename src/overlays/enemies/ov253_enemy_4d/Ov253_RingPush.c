/* Ov253_RingPush -- queue a kind-0x10 entry in the owner's +0x90 ring at the node's +4
 * index: position from `v`, +0x18 cleared, +0x1c = arg; bit 1 of the owner's +0x5c is cleared
 * and the index advances modulo the owner's +0x8c length. */

#include "nitro/fx_types.h"

struct Ov253Ring { int kind; int pad[5]; int f18; int f1c; int pad2[3]; VecFx32 pos; };

extern int func_02020400(int num, int den);

void Ov253_RingPush(int *node, const VecFx32 *v, int arg) {
    struct Ov253Ring *entry = &((struct Ov253Ring *)*(int *)(node[0] + 0x90))[node[1]];

    *(int *)(node[0] + 0x5c) &= ~2;
    entry->kind = 0x10;
    entry->pos = *v;
    entry->f18 = 0;
    entry->f1c = arg;
    node[1] = (node[1] + 1) % *(int *)(node[0] + 0x8c);
}

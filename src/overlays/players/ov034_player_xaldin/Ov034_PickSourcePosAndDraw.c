/* While the character is shown, places the effect node at the source its kind names (the
 * character's position, raised or not, or the locked target's point) and draws it. */

#include "nitro/fx_types.h"

extern void func_ov022_020ad44c(void *out, int self);
extern void Scene_DrawNode(int a);

typedef struct { unsigned char b0 : 1; } Flags;

void Ov034_PickSourcePosAndDraw(int self, int *node) {
    VecFx32 v;
    int ok = 0;
    if (!((Flags *)(self + 0x694))->b0) return;
    switch (node[9]) {
    case 5:
        v = *(VecFx32 *)(self + 0x8c + 0x400);
        v.y += 0xf33;
        ok = 1;
        break;
    case 2:
        func_ov022_020ad44c(&v, self);
        ok = 1;
        break;
    case 4:
        v = *(VecFx32 *)(self + 0x8c + 0x400);
        ok = 1;
        break;
    }
    if (ok == 0) return;
    *(VecFx32 *)((char *)node + 0xcc) = v;
    Scene_DrawNode((int)node + 0x28);
}

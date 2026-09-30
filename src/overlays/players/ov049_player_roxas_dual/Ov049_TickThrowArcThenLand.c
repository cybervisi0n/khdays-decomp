/* Thrown arc step: advances the shot's timer, resolves hits along its path, and when its animation
 * ends marks it landed (state 3) and releases it. */

#include "nitro/fx_types.h"

extern void Ov022_ResolveShotHit(int self, char *node, void *v, void *w);
extern int func_ov022_02091540(int a, int b);
extern void func_ov022_02091d80(int self, char *node, int c);

extern VecFx32 data_02041dc8;

int Ov049_TickThrowArcThenLand(int self, char *node, int dt) {
    VecFx32 a;
    VecFx32 b;
    *(int *)(node + 4) += dt;
    a = *(VecFx32 *)(node + 0xcc);
    *(VecFx32 *)(node + 0xcc) = a;
    b = data_02041dc8;
    b.y += 0x25000;
    Ov022_ResolveShotHit(self, node, &a, &b);
    node[2] = 2;
    if (func_ov022_02091540((int)(node + 0x28), dt) != 0) {
        node[2] = 3;
    }
    if (node[2] == 3) {
        *(int *)(node + 4) = 0;
        func_ov022_02091d80(self, node, 0);
    }
    return 0;
}

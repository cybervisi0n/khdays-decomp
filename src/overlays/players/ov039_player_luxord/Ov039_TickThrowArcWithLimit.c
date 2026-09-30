/* Thrown-card arc step: advances the shot's timer, resolves hits until close to the end of its
 * animation, and marks it finished (state 3, then 4) when the animation ends, releasing it. */

#include "nitro/fx_types.h"

extern int Anim_GetLengthQ12(int a, int b);
extern void Ov022_ResolveShotHit(int self, char *node, void *v, void *w);
extern int func_ov022_02091540(int a, int b);
extern void func_ov022_02091d80(int self, char *node, int c);

extern VecFx32 data_02041dc8;

int Ov039_TickThrowArcWithLimit(int self, char *node, int dt) {
    VecFx32 a;
    VecFx32 b;
    int r = Anim_GetLengthQ12((int)(node + 0x28), 0);
    int lim = r - 0x9000;
    a = *(VecFx32 *)(node + 0xcc);
    b = data_02041dc8;
    *(int *)(node + 4) += dt;
    if (*(int *)(node + 4) <= lim) {
        Ov022_ResolveShotHit(self, node, &a, &b);
    }
    if (node[2] == 3) {
        node[2] = 4;
    }
    if (func_ov022_02091540((int)(node + 0x28), dt) != 0) {
        node[2] = 3;
    }
    if (node[2] == 3) {
        *(int *)(node + 4) = 0;
        func_ov022_02091d80(self, node, 0);
    }
    return 0;
}

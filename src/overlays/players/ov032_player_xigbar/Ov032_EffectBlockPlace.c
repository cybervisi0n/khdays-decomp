/* Places the ov032 enemy's effect block in world space (and its byte-identical twins): only
 * while the block is running, bit 0 of the actor's +0x694 flags is set and the block is in
 * state 2 or 4. Publishes the actor's +0x488 heading at +0x88 (biased), raises flag 0x20 and
 * hands in the local offset {0x148, 0x1429, 0x6e1} rotated by the heading and added to the
 * actor's +0x48c position at +0xb0. */

#include "nitro/fx_types.h"

struct b1 { unsigned char b0 : 1; };

extern void MTX_RotY33_(int *m, int s, int c);
extern void MTX_MultVec33(VecFx32 *v, int *m, VecFx32 *out);
extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *out);
extern void Scene_DrawNode(void *p);
extern const short data_0203d210[];

void Ov032_EffectBlockPlace(int self, unsigned char *block)
{
    int m[9];
    VecFx32 v;
    int heading;
    int idx;
    int s;
    int c;

    if (block[0x114] == 0) {
        return;
    }
    if (((struct b1 *)(self + 0x694))->b0 == 0) {
        return;
    }
    if (block[0x114] != 2 && block[0x114] != 4) {
        return;
    }
    heading = *(int *)(self + 0x488);
    idx = heading >> 4;
    *(unsigned short *)(block + 0x88) = heading + 0x8000;
    s = data_0203d210[idx * 2];
    c = data_0203d210[idx * 2 + 1];
    *(unsigned short *)(block + 0xc) |= 0x20;
    v.x = 0x148;
    v.y = 0x1429;
    v.z = 0x6e1;
    MTX_RotY33_(m, -s, -c);
    MTX_MultVec33(&v, m, &v);
    VEC_Add(&v, (VecFx32 *)(self + 0x48c), &v);
    *(VecFx32 *)(block + 0xb0) = v;
    Scene_DrawNode(block + 0xc);
}

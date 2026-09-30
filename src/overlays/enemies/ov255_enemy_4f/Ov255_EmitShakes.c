/* Shake emitter tick of the ov255 partner (only while its owner's kind +0x1c6 is 1): the burst
 * uses 4 entries from 5.0 on the +8 clock, else 3; once past 0.03 a pending +0xc flag hides the
 * +4 object again. The clock advances, the owner's +0x3c8 byte becomes 1 or 2 by whether +0x3cc is
 * set, and the entries from the ring cursor (+0x10, wrapping at the table's +0x8c count) are
 * spread along the owner's move since the last tick (+0x39c to +0xb0): offset 0.25 (3 entries) or
 * 0.125, full strength and the owner's +0x3bc handle. The move start is then updated. */

#include "nitro/fx_types.h"

struct Shake { int offset; int strength; char pad08[0x10]; int handle; char pad1c[0x10]; VecFx32 at; };
struct ShakeSet { char pad[0x8c]; int count; struct Shake *items; };

extern void VEC_Subtract(const void *a, const void *b, VecFx32 *out);
extern long long func_02020400(int num, int den);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const void *a, const void *b, void *out);

void Ov255_EmitShakes(int *node)
{
    int *h = (int *)node[1];
    VecFx32 move;
    VecFx32 step;
    signed char i;
    int count;

    if (*(signed char *)(h[0] + 0x1c6) != 1) {
        return;
    }
    count = h[2] >= 0x5000 ? 4 : 3;
    if (h[2] >= 0x88 && *(unsigned char *)(h + 3) == 1) {
        *(unsigned char *)(h + 3) = 0;
        *(int *)(h[1] + 0x5c) &= ~2;
    }
    h[2] += *(int *)(node[0] + 0x2c);
    *(unsigned char *)(h[0] + 0x3c8) = *(int *)(h[0] + 0x3cc) == 0 ? 1 : 2;
    VEC_Subtract((void *)(h[0] + 0x39c), (void *)(h[0] + 0xb0), &move);
    for (i = 0; i < count; i++) {
        struct Shake *e = &((struct ShakeSet *)h[1])->items[h[4]];

        ScaleVec3Fx12((int)func_02020400(i * 0x1000, count), &move, &step);
        VEC_Add((void *)(h[0] + 0xb0), &step, &e->at);
        e->offset = count == 3 ? 0x400 : 0x200;
        e->strength = 0x1000;
        e->handle = *(int *)(h[0] + 0x3bc);
        h[4] = (int)(func_02020400(h[4] + 1, ((struct ShakeSet *)h[1])->count) >> 32);
    }
    *(VecFx32 *)(h[0] + 0x39c) = *(VecFx32 *)(h[0] + 0xb0);
}

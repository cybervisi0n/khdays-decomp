/* Copies the step's stored vector into the actor's movement vector (+0xf0). */

#include "nitro/fx_types.h"

struct Inner {
    VecFx32 *dst;
    int pad[2];
    VecFx32 src;
};

struct Outer {
    int pad;
    struct Inner *inner;
};

void Ov156_AiApplyMoveVector(struct Outer *a) {
    struct Inner *p = a->inner;
    VecFx32 *dst = p->dst;
    *(VecFx32 *)((char *)dst + 0xf0) = p->src;
}

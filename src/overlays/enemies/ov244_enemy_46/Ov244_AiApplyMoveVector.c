/* Copies the step's stored vector into the actor's movement vector (+0xf0). */

#include "nitro/fx_types.h"

struct Src {
    int *base;
    int pad;
    VecFx32 data;
};

struct Outer {
    int pad0;
    struct Src *src;
};

void Ov244_AiApplyMoveVector(struct Outer *p) {
    struct Src *s = p->src;
    *(VecFx32 *)((char *)s->base + 0xf0) = s->data;
}

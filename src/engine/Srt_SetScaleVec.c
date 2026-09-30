/* Sets an SRT's scale from a vector and marks it non-identity and non-uniform. */

#include "nitro/fx_types.h"

struct Obj {
    char pad[0x1c];
    VecFx32 v;
    unsigned char flags;
};

void Srt_SetScaleVec(struct Obj *o, VecFx32 *src) {
    o->v = *src;
    o->flags &= ~1;
    o->flags &= ~2;
}

/* Ov253_ChainUpdate -- chain update: the +0x38c item's transform takes the +0x398 joint's
 * anchor and its +4 rotation while the actor is in kind 3 or 7 (otherwise a rotation of +0x44c
 * about data_02042264); then each of the four +0x3ac chains refreshes its three +0x58 segments
 * (start at joint i's anchor, +0xc direction to joint i+1, +0x18 length). */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct vec4 { int a, b, c, d; };
struct Ov253Chain { int joints[4]; int f10; int segs[3]; int f20; };
struct Ov253Chains { char pad[0x3ac]; struct Ov253Chain chain[4]; };

extern void Srt_SetTranslation(void *srt, const VecFx32 *translation);
extern void Srt_SetRotationQuat(void *srt, const struct vec4 *rotation);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern const VecFx32 data_02042264;

static inline void ChainUpdateOne(struct Ov253Chain *chain) {
    int i;
    for (i = 0; i < 3; i++) {
        char *seg = (char *)chain->segs[i] + 0x58;
        *(VecFx32 *)seg = *(VecFx32 *)(chain->joints[i] + 0x14);
        VEC_Subtract((VecFx32 *)(chain->joints[i + 1] + 0x14), (VecFx32 *)seg, (VecFx32 *)(seg + 0xc));
        *(int *)(seg + 0x18) = VEC_Normalize((VecFx32 *)(seg + 0xc), (VecFx32 *)(seg + 0xc));
    }
}

void Ov253_ChainUpdate(int a, char *self) {
    struct vec4 rot;

    if (!(*(signed char *)(self + 0x100 + 0xc6) != 7 && *(signed char *)(self + 0x100 + 0xc6) != 3)) {
        rot = *(struct vec4 *)(*(int *)(self + 0x398) + 4);
    } else {
        QuatFromAxisAngle(&rot, &data_02042264, *(int *)(self + 0x44c));
    }
    Srt_SetTranslation((void *)(*(int *)(self + 0x38c) + 0x30), (VecFx32 *)(*(int *)(self + 0x398) + 0x14));
    Srt_SetRotationQuat((void *)(*(int *)(self + 0x38c) + 0x30), &rot);
    ChainUpdateOne(&((struct Ov253Chains *)self)->chain[0]);
    ChainUpdateOne(&((struct Ov253Chains *)self)->chain[1]);
    ChainUpdateOne(&((struct Ov253Chains *)self)->chain[2]);
    ChainUpdateOne(&((struct Ov253Chains *)self)->chain[3]);
}

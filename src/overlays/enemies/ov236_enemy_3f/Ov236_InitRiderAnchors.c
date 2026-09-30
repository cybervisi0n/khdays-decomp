/* Model pose init: after the base 020c4924 step, mirror bit 1 of the +0x9c list's +0x5c into
 * the +0x390 item's, then for each rider pair: push the +0x3ac clip's pose (+4) into the +0x39c
 * item's +0x10 and the +0x3c0 handle's target, seat the +0x398 item's +0x58 anchor at the +0x3a8
 * clip's +0x14 position with its +0x64 direction from the +0x3a8 to the +0x3ac position
 * (normalised, length at +0x70); likewise +0x3b4 -> +0x3a4 / +0x3c4 and +0x3b0 -> +0x3a0. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct blk11 { int w[11]; };
struct Flags5c { int b0 : 1; int b1 : 1; };
struct Ov236Anchor { VecFx32 pos; VecFx32 dir; int len; };
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);

void Ov236_InitRiderAnchors(char *obj, int flag) {
    struct Ov236Anchor *a;

    Ov107_AiState_DispatchModelCallbacks(obj, flag);
    ((struct Flags5c *)(*(int *)(obj + 0x390) + 0x5c))->b1 = ((struct Flags5c *)(*(int *)(obj + 0x9c) + 0x5c))->b1;
    *(struct blk11 *)(*(char **)(obj + 0x39c) + 0x10) = *(struct blk11 *)(*(char **)(obj + 0x3ac) + 4);
    *(struct blk11 *)(*(char **)(*(char **)(obj + 0x3c0)) + 0x10) = *(struct blk11 *)(*(char **)(obj + 0x3ac) + 4);
    a = (struct Ov236Anchor *)(*(char **)(obj + 0x398) + 0x58);
    a->pos = *(VecFx32 *)(*(char **)(obj + 0x3a8) + 0x14);
    VEC_Subtract((VecFx32 *)(*(char **)(obj + 0x3ac) + 0x14), (VecFx32 *)(*(char **)(obj + 0x3a8) + 0x14), &a->dir);
    a->len = VEC_Normalize(&a->dir, &a->dir);
    *(struct blk11 *)(*(char **)(obj + 0x3a4) + 0x10) = *(struct blk11 *)(*(char **)(obj + 0x3b4) + 4);
    *(struct blk11 *)(*(char **)(*(char **)(obj + 0x3c4)) + 0x10) = *(struct blk11 *)(*(char **)(obj + 0x3b4) + 4);
    a = (struct Ov236Anchor *)(*(char **)(obj + 0x3a0) + 0x58);
    a->pos = *(VecFx32 *)(*(char **)(obj + 0x3b0) + 0x14);
    VEC_Subtract((VecFx32 *)(*(char **)(obj + 0x3b4) + 0x14), (VecFx32 *)(*(char **)(obj + 0x3b0) + 0x14), &a->dir);
    a->len = VEC_Normalize(&a->dir, &a->dir);
}

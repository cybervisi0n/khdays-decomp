/* Ov107_MoveNodeAndRelayout -- move a node to `v` (its translation, +0xa0) and refresh its
 * collision sphere, ov107. Returns what Ov107_UpdateCollisionSphere returns. */

#include "nitro/fx_types.h"
#include "game/actor.h"
#include "game/enemy_common.h"

extern void Srt_SetTranslation(void *sub, const void *src);
int Ov107_MoveNodeAndRelayout(Actor *node, const VecFx32 *v) {
    Srt_SetTranslation(&node->srt, v);
    return Ov107_UpdateCollisionSphere((int)node);
}

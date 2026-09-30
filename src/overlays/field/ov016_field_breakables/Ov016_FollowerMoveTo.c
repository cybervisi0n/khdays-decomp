/* Ov016_FollowerMoveTo -- Ov016_FollowerMoveTo: move the follower (the class-0x16 object that
 * trails a player) to pPos, remembering the
 * offset from its rest position (+0xd0) at +0x1a0 and pushing the new position into the
 * render node (+0x28, 0202b450). */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct Ov016Follower {
    unsigned char pad_000[0x28];
    unsigned char node[0xd0 - 0x28];   /* 0x28: render node */
    VecFx32 rest;                      /* 0xd0 */
    unsigned char pad_0dc[0x1a0 - 0xdc];
    VecFx32 offset;                    /* 0x1a0 */
} Ov016Follower;

void Ov016_FollowerMoveTo(Ov016Follower *pSelf, VecFx32 *pPos)
{
    VecFx32 rest;

    rest = pSelf->rest;
    pSelf->offset.x = pPos->x - rest.x;
    pSelf->offset.y = pPos->y - rest.y;
    pSelf->offset.z = pPos->z - rest.z;
    Actor_SetVecAndSyncChild(pSelf->node, pPos);
}

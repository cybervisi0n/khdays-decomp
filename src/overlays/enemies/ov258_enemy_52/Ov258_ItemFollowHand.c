/* Hand follow of the ov258 held item: the item's pose takes the transform of the owner's (+0x390)
 * left (+0x43c) or right (+0x448) hand by its +0x38c side flag and the item moves to that hand's
 * +0x14 point. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void Srt_SetRotationQuat(void *srt, void *from);

void Ov258_ItemFollowHand(int *node)
{
    int *state = (int *)node[1];
    int hand;

    if (*(u8 *)(*state + 0x38c) == 0) {
        hand = *(int *)(*(int *)(*state + 0x390) + 0x43c);
        Srt_SetRotationQuat((void *)(*state + 0xa0), (void *)(hand + 4));
    } else {
        hand = *(int *)(*(int *)(*state + 0x390) + 0x448);
        Srt_SetRotationQuat((void *)(*state + 0xa0), (void *)(hand + 4));
    }
    Ov107_MoveNodeAndRelayout((Actor *)(*state), (VecFx32 *)(hand + 0x14));
}

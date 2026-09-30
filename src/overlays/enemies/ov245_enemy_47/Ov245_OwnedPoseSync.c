/* Ov245_OwnedPoseSync -- pose sync of the +0x3dc-owned actor: the +0xa0 placement takes the
 * owner's +0x448 anchor's +4 rotation and the anchor's +0x14 position plus the +0x3d0 offset;
 * the owner is notified (020c5c54), the base sync (020c6980) runs, then the placement is copied
 * into the +0x38c item's +0x10 and that one's +0x10 into the +0x388 target's +0x10. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int m[11]; } Pose44;
struct Ov245Item { char pad[0x10]; Pose44 pose; };

extern void Srt_SetRotationQuat(int placement, void *rotation);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Ov107_ProcessObjectTick(int self, int a);

void Ov245_OwnedPoseSync(int self, int a) {
    VecFx32 pos;

    Srt_SetRotationQuat(self + 0xa0, (void *)(*(int *)(*(int *)(self + 0x3dc) + 0x448) + 4));
    VEC_Add((VecFx32 *)(*(int *)(*(int *)(self + 0x3dc) + 0x448) + 0x14), (VecFx32 *)(self + 0x3d0), &pos);
    Ov107_MoveNodeAndRelayout((Actor *)self, &pos);
    Ov107_ProcessObjectTick(self, a);
    ((struct Ov245Item *)*(int *)(self + 0x38c))->pose = *(Pose44 *)(self + 0xa0);
    ((struct Ov245Item *)**(int **)(self + 0x388))->pose = ((struct Ov245Item *)*(int *)(self + 0x38c))->pose;
}

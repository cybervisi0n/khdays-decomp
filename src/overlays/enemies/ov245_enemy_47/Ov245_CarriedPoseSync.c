/* Ov245_CarriedPoseSync -- pose sync of the carried actor: runs the +0x4c8 and +0x4cc items'
 * motions (020c9ec8), the base sync (020c6980), copies the +0xa0 placement into the +0x3b8
 * item's +0x10 and that into the +0x3b4 target's +0x10, lifts both +0x24 heights by 19.0 and
 * keeps the +0x3b8 item's +0x20 position at +0x3bc. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int m[11]; } Pose44;
struct Ov245Item { char pad[0x10]; Pose44 pose; };

extern void Ov107_ProcessObjectTick(int self, int a);

void Ov245_CarriedPoseSync(int self, int a) {
    Ov107_RefreshAndSelectChild(*(int *)(self + 0x4c8), a);
    Ov107_RefreshAndSelectChild(*(int *)(self + 0x4cc), a);
    Ov107_ProcessObjectTick(self, a);
    ((struct Ov245Item *)*(int *)(self + 0x3b8))->pose = *(Pose44 *)(self + 0xa0);
    ((struct Ov245Item *)**(int **)(self + 0x3b4))->pose = ((struct Ov245Item *)*(int *)(self + 0x3b8))->pose;
    *(int *)(**(int **)(self + 0x3b4) + 0x24) += 0x4c00;
    *(int *)(*(int *)(self + 0x3b8) + 0x24) += 0x4c00;
    *(VecFx32 *)(self + 0x3bc) = *(VecFx32 *)(*(int *)(self + 0x3b8) + 0x20);
}

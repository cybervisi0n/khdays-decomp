/* Ov245_MountedPoseSync -- pose sync of the mounted actor: the +0xa0 placement takes the +0x398
 * rider's +0x44c anchor's +4 rotation, the +0x39c seat's +0x2c offset is rotated through it and
 * added to the +0x3a0 position, then the anchor's +0x14 position is added; the owner is notified
 * (020c5c54), the seat's motion runs (020c9ec8) and the base sync (020c6980) finishes. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern void Srt_SetRotationQuat(int placement, void *rotation);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Ov107_ProcessObjectTick(int self, int a);

void Ov245_MountedPoseSync(int self, int a) {
    VecFx32 off;

    Srt_SetRotationQuat(self + 0xa0, (void *)(*(int *)(*(int *)(self + 0x398) + 0x44c) + 4));
    Vec3TransformViaTempMtx(&off, (void *)(self + 0xa0), (VecFx32 *)(*(int *)(self + 0x39c) + 0x2c));
    VEC_Add((VecFx32 *)(self + 0x3a0), &off, (VecFx32 *)(self + 0x3a0));
    VEC_Add((VecFx32 *)(self + 0x3a0), (VecFx32 *)(*(int *)(*(int *)(self + 0x398) + 0x44c) + 0x14), &off);
    Ov107_MoveNodeAndRelayout((Actor *)self, &off);
    Ov107_RefreshAndSelectChild(*(int *)(self + 0x39c), a);
    Ov107_ProcessObjectTick(self, a);
}

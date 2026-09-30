/* Per-frame step of one stored shot record: a live record (+8) that no longer belongs to this
 * enemy (+0x2c vs the enemy's +0x66 id) is dropped. While the enemy has a target, the shot heads
 * for it (target position minus the shot's +0x14 position, speed capped at the +0xc value by the
 * distance); otherwise it keeps its last heading (+0x20). The unit heading is stored back, scaled
 * to the speed and swept against collision group +0x2c with a 0x800 radius: a hit stops the shot
 * at the contact fraction, else it just moves. Every 0x6000 of accumulated time (+0x10) it fires
 * (Ov039_FireStoredShot); the shot level (+4) goes up and, once it reaches the count at +0,
 * the shot fires charged and the record is closed. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

extern int Ov022_ValidateTargetRef(char *self);
extern VecFx32 *func_ov022_020ad0c0(char *self);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Mag(const VecFx32 *v);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *unit);
extern void ScaleVec3Fx12(int scale, const VecFx32 *src, VecFx32 *dst);
extern char *EntityMgr_RunSphereCastSimple(int group, const VecFx32 *origin, const VecFx32 *disp, int radius);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Ov058_FireStoredShot(char *self, char *shot, int bCharged);          /* Ov039_FireStoredShot */

void Ov058_StepStoredShot(char *self, char *shot, int dt)
{
    VecFx32 vMove;
    int speed;
    char *hit;
    int bCharged;

    if (*(int *)(shot + 8) == 0) {
        return;
    }
    if (*(u8 *)(shot + 0x2c) != *(short *)(self + 0x66)) {
        *(int *)(shot + 8) = 0;
    }
    if (Ov022_ValidateTargetRef(self) != 0) {
        VEC_Subtract(func_ov022_020ad0c0(self), (VecFx32 *)(shot + 0x14), &vMove);
        speed = *(int *)(shot + 0xc);
        if (VEC_Mag(&vMove) <= speed) {
            speed = VEC_Mag(&vMove);
        }
    } else {
        vMove = *(VecFx32 *)(shot + 0x20);
        speed = *(int *)(shot + 0xc);
    }
    if (VEC_Mag(&vMove) != 0) {
        VEC_Normalize(&vMove, &vMove);
    }
    *(VecFx32 *)(shot + 0x20) = vMove;
    ScaleVec3Fx12(speed, &vMove, &vMove);
    hit = EntityMgr_RunSphereCastSimple(*(u8 *)(shot + 0x2c), (VecFx32 *)(shot + 0x14), &vMove, 0x800);
    if (hit != 0) {
        Vec3ScaleAddQ27(*(int *)(hit + 0xc), &vMove, (VecFx32 *)(shot + 0x14), (VecFx32 *)(shot + 0x14));
    } else {
        VEC_Add((VecFx32 *)(shot + 0x14), &vMove, (VecFx32 *)(shot + 0x14));
    }
    *(int *)(shot + 0x10) += dt;
    if (*(int *)(shot + 0x10) < 0x6000) {
        return;
    }
    bCharged = 0;
    *(int *)(shot + 0x10) = 0;
    *(int *)(shot + 4) += 1;
    if (*(int *)(shot + 4) >= *(int *)shot) {
        bCharged = 1;
    }
    Ov058_FireStoredShot(self, shot, bCharged);
    if (bCharged != 0) {
        *(int *)(shot + 8) = 0;
    }
}

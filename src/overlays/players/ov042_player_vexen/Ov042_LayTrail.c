/* Lay the enemy's trail: `move` (the frame's displacement) is added to the distance accumulated
 * in the shared object's +0x2c3c; every 0x2000 of it drops one effect (Ov042_ClaimSlotAndLaunch)
 * along the unit direction of the move, at `origin` plus the direction scaled by what was left
 * over from the previous drop (0x2000 - carry), consuming 0x2000 per drop. */

#include "nitro/fx_types.h"

extern int VEC_Normalize(const VecFx32 *v, VecFx32 *unit);
extern int VEC_Mag(const VecFx32 *v);
extern void ScaleVec3Fx12(int scale, const VecFx32 *src, VecFx32 *dst);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void Ov042_ClaimSlotAndLaunch(VecFx32 *pos, VecFx32 *dir);                         /* Ov042_ClaimSlotAndLaunch */
extern char *data_ov042_020b4800;

void Ov042_LayTrail(VecFx32 *origin, VecFx32 *move)
{
    VecFx32 vDir;
    VecFx32 vPos;
    char *trail = data_ov042_020b4800 + 0x2c + 0x2c00;
    int carry = *(int *)(trail + 0x10);
    int dist;

    vDir = *move;
    VEC_Normalize(&vDir, &vDir);
    dist = *(int *)(trail + 0x10) + VEC_Mag(move);
    *(int *)(trail + 0x10) = dist;
    if (dist <= 0x2000) {
        return;
    }
    do {
        vDir = *move;
        VEC_Normalize(&vDir, &vDir);
        ScaleVec3Fx12(0x2000 - carry, &vDir, &vDir);
        VEC_Add(origin, &vDir, &vPos);
        Ov042_ClaimSlotAndLaunch(&vPos, &vDir);
        carry = *(int *)(trail + 0x10) - 0x2000;
        *(int *)(trail + 0x10) = carry;
    } while (carry > 0x2000);
}

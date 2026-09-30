/* Fires both of the mission owner's +0x2d38 rows (stride 0x240): each shot spawns at the row's
 * slot position (4ef0), aims along the normalised +0x340 vector of the row, kind 7 from a slot,
 * speed 0x1100, and is handed to Ov022_SendPlacementMessage. */

/* One object, not two: the position vector is its head and the projectile fields are its
 * tail, which is why the ROM passes a single pointer. */

#include "nitro/fx_types.h"

struct FireParams {
    VecFx32 vPos;
    short vx;
    short vy;
    short vz;
    short nSpeed;
    int bFromSlot;
    int nKind;
    int pad1c[4];
};

extern void Ov062_ComputeSlotPosition(char *self, int slot, VecFx32 *out);
extern int VEC_Mag(const VecFx32 *v);
extern void VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void Ov022_SendPlacementMessage(char *self, struct FireParams *p);
extern char *data_ov062_020b80e0;

void Ov062_FireBothRows(char *self)
{
    VecFx32 vAim;
    struct FireParams p;
    int i;
    char *pRow = data_ov062_020b80e0 + 0x138 + 0x2c00;

    for (i = 0; i < 2; i++) {
        Ov062_ComputeSlotPosition(self, i, &p.vPos);
        vAim = *(VecFx32 *)(pRow + 0x340);
        if (VEC_Mag(&vAim) != 0) {
            VEC_Normalize(&vAim, &vAim);
        }
        p.vx = (short)vAim.x;
        p.vy = (short)vAim.y;
        p.vz = (short)vAim.z;
        p.bFromSlot = 1;
        p.pad1c[0] = 0;
        p.pad1c[1] = 0;
        p.nKind = 7;
        p.pad1c[2] = 0;
        p.pad1c[3] = 0;
        p.nSpeed = 0x1100;
        Ov022_SendPlacementMessage(self, &p);
        pRow += 0x240;
    }
}

/* Facing update of the ov261 enemy (and its byte-identical twin): builds the +0x1c anchor
 * quaternion from the direction from the position to the target point (the shared forward
 * vector when degenerate), with its vertical component clamped to [-0x800, 0x800] and
 * renormalised, through the zero-origin look-at matrix. */

#include "nitro/fx_types.h"

extern void VEC_Subtract(void *a, void *b, void *d);
extern int VEC_Normalize(void *a, void *d);
extern void Mtx33_LookAt(int *out, VecFx32 *dir, const VecFx32 *origin, const VecFx32 *up);
extern void Quat_FromMtx33(int *quat, int *mtx);
extern const VecFx32 data_02042258;
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042264;

void Ov262_SetFacingAnchor(int *anchor, VecFx32 *target, VecFx32 *pos)
{
    int mtx[9];
    VecFx32 dir;
    int y;

    VEC_Subtract(target, pos, &dir);
    if (VEC_Normalize(&dir, &dir) == 0) {
        dir = data_02042258;
    }
    y = dir.y;
    if (y > 0x800) {
        y = 0x800;
    } else if (y < -0x800) {
        y = -0x800;
    }
    dir.y = y;
    VEC_Normalize(&dir, &dir);
    Mtx33_LookAt(mtx, &dir, &data_02041dc8, &data_02042264);
    Quat_FromMtx33(anchor, mtx);
}

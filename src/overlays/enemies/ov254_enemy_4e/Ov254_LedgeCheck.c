/* Ledge check of an ov254 helper: only while the actor is grounded against a wall (+0x17a bit 1)
 * and the +8 track is at least 0.2 away from the +0x24 anchor. A probe 3.85 ahead of the track
 * along the +0x30 yaw is cast through the +4 item's +0x7c world (radius 0x3d98) into +0xc; when
 * the hit normal faces the actor's +0x114 direction (dot below -0.5) the actor is knocked back in
 * place (mode 0xa), the next move is 0xa and 1 is returned; otherwise 0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Flag17a { u8 b0 : 1; u8 b1 : 1; };

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Mag(const VecFx32 *v);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Collision_CastSphere(void *world, const VecFx32 *from, VecFx32 *out, int radius);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern const short data_0203d210[];
extern const VecFx32 data_02041dc8;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

int Ov254_LedgeCheck(int *state)
{
    VecFx32 dir;
    VecFx32 facing;
    VecFx32 normal;
    int item = *(int *)(*state + 4);
    int k;

    if (((struct Flag17a *)(*state + 0x17a))->b1) {
        VEC_Subtract((VecFx32 *)state[2], (VecFx32 *)(state + 9), &dir);
        if (VEC_Mag(&dir) >= 0x333) {
            k = ANG2IDX(state[0xc]) * 2;
            dir.x = data_0203d210[k];
            dir.y = 0;
            dir.z = data_0203d210[k + 1];
            ScaleVec3Fx12(0x3d99, &dir, &dir);
            VEC_Add(&dir, (VecFx32 *)state[2], &dir);
            if (Collision_CastSphere(*(void **)(item + 0x7c), &dir, (VecFx32 *)(state + 3), 0x3d98) != 0) {
                VEC_Normalize((VecFx32 *)(*state + 0x114), &facing);
                VEC_Normalize((VecFx32 *)(state + 3), &normal);
                if (VEC_DotProduct(&facing, &normal) < -0x800) {
                    func_ov107_020c0b90(*state, 0xa, data_02041dc8, 0);
                    *(signed char *)(*state + 0x1c7) = 0xa;
                    return 1;
                }
            }
        }
    }
    return 0;
}

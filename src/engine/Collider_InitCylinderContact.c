/* Fills the contact normal and distance for a cylinder (XZ direction from the axis). */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *ab);
extern void func_01ffcfd0(VecFx32 *a, void *b);
extern int  VEC_DotProductFx16(VecFx32 *a, void *b);

void Collider_InitCylinderContact(char *a, char *b, char *c) {
    VecFx32 tmp;
    VecFx32 diff;

    Vec3ScaleAddQ27(*(int *)(b + 0x78), (const VecFx32 *)(b + 0x1c), (const VecFx32 *)(b + 0x10), &tmp);
    VEC_Subtract(&tmp, (const VecFx32 *)(a + 0x2c), &diff);
    diff.y = 0;
    func_01ffcfd0(&diff, c + 0x14);
    *(int *)(c + 0x1c) = VEC_DotProductFx16(&tmp, c + 0x14);
    *(int *)(c + 0x80) = *(int *)(a + 0x24);
}

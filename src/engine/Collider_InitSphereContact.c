/* Fills the contact normal and distance for a sphere (direction from the center). */

#include "nitro/fx_types.h"
#include "game/engine.h"

extern void VEC_Subtract();
extern void func_01ffcfd0();
extern int VEC_DotProductFx16();

void Collider_InitSphereContact(char *p1, char *p2, char *p3)
{
    VecFx32 va;
    VecFx32 vb;

    Vec3ScaleAddQ27(*(int *)(p2 + 0x78), (const VecFx32 *)(p2 + 0x1c), (const VecFx32 *)(p2 + 0x10), &va);
    VEC_Subtract(&va, p1 + 0x2c, &vb);
    vb.y = vb.y - *(int *)(p1 + 0x3c);
    func_01ffcfd0(&vb, p3 + 0x14);
    *(int *)(p3 + 0x1c) = VEC_DotProductFx16(&va, p3 + 0x14);
}

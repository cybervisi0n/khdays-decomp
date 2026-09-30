/* Vortex pull of the ov258 actor: every active object (+0x40 bit 1, +0x60 low bit 0) of the world's
 * +0xa8 list within 24.0 of the +0x1c point is pushed at 0.28 along its direction swirled by the up
 * axis (cross product added), into its +0xe4 velocity. */

#include "nitro/fx_types.h"

typedef struct { int b0 : 1; int b1 : 1; } Bits;
typedef struct { unsigned short lo : 8; unsigned short hi : 8; } Flags16;

extern int *List_First(void *list);
extern int *List_Next(void *list);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void VEC_CrossProduct(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern const VecFx32 data_02042240;

void Ov258_VortexPull(int *node)
{
    int *state = (int *)node[1];
    int world = *(int *)(*state + 4);
    VecFx32 d;
    VecFx32 push;
    VecFx32 swirl;
    int *it;
    char *obj;

    it = List_First((void *)(world + 0xa8));
    for (obj = it == 0 ? 0 : (char *)*it; obj != 0; obj = it == 0 ? 0 : (char *)*it) {
        if (((Bits *)(obj + 0x40))->b1 && (((Flags16 *)(obj + 0x60))->lo & 1)) {
            VEC_Subtract((VecFx32 *)(state + 7), (VecFx32 *)(obj + 0x74), &d);
            if (VEC_Normalize(&d, &d) <= 0x18000) {
                VEC_CrossProduct(&data_02042240, &d, &swirl);
                VEC_Add(&d, &swirl, &d);
                VEC_Normalize(&d, &d);
                ScaleVec3Fx12(0x470, &d, &push);
                VEC_Add((VecFx32 *)(obj + 0xe4), &push, (VecFx32 *)(obj + 0xe4));
            }
        }
        it = List_Next((void *)(world + 0xa8));
    }
}

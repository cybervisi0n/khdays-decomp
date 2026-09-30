/* Stores the listener position and its normalised up vector (cross product of the two axes). */

#include "nitro/fx_types.h"

extern void VEC_CrossProduct();
extern void VEC_Normalize();
extern char *data_0204c234;

void SoundMgr_SetListener(VecFx32 *src, VecFx32 *a, VecFx32 *b)
{
    char *base;
    VecFx32 local;

    base = data_0204c234;
    VEC_CrossProduct(a, b, &local);
    VEC_Normalize(&local, base + 0xb44d8);
    *(VecFx32 *)(base + 0xb44cc) = *src;
}

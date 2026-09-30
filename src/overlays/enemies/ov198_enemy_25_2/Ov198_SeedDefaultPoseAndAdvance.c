
#include "nitro/fx_types.h"

extern int data_02041dc8; /* const initial pose vector */
extern void SetSubitemState(int subitem, int a, int b, int c);

// Seed the node pose: copy the default vector into both this[+0x3a4] and
// this[+0x3b0], raise the two ready flags, then push sub-state via SetSubitemState.
void Ov198_SeedDefaultPoseAndAdvance(int *this, int arg1)
{
    VecFx32 tmp = *(VecFx32 *)&data_02041dc8;
    *(VecFx32 *)((int)this + 0x3a4) = tmp;
    *(VecFx32 *)((int)this + 0x3b0) = tmp;
    *(int *)((int)this + 0x3e4) = 1;
    *(int *)((int)this + 0x3e8) = 1;
    SetSubitemState(*(int *)((int)this + 0x388), 0, (short)arg1, 0);
}

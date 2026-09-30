/* Whether the active player is in the element's group and within its range. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern int QueryActiveStateOrDelegate(void);
extern void *GetEntryField20ByIndex(int index);
extern int Ov002_GetCtxTableByte(int arg0);
extern int VEC_Distance(const VecFx32 *a, const VecFx32 *b);

int Ov014_IsPlayerInRange(void *self)
{
    void *owner = *(void **)((char *)self + 8);
    void *entry = GetEntryField20ByIndex(QueryActiveStateOrDelegate());
    s16 key;
    VecFx32 position;
    int distance;

    if (entry == 0)
        return 0;
    key = *(s16 *)((char *)entry + 0x66);
    if (key != Ov002_GetCtxTableByte(*(u8 *)((char *)self + 0x10)))
        return 0;
    position = *(VecFx32 *)((char *)entry + 0x48c);
    distance = VEC_Distance((const VecFx32 *)((char *)self + 0x1c), &position);
    return *(s32 *)((char *)owner + 0x6c) >= distance;
}

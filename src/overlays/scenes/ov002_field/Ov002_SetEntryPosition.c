/* Write a VecFx32 into the entry's object at +0x104, indexing the 0x18-byte entry table at ctx+0x44
 * by the id's resolved slot. */

#include "nitro/fx_types.h"

extern int data_ov002_0207fa14;
extern int Ov002_FindKeyIndex(int arg0);

void Ov002_SetEntryPosition(int arg0, const VecFx32 *pos) {
    int ctx = *(int *)&data_ov002_0207fa14;
    int idx = Ov002_FindKeyIndex(arg0);
    int entry = *(int *)(*(int *)(ctx + 0x44) + idx * 0x18);

    *(VecFx32 *)(entry + 0x104) = *pos;
}

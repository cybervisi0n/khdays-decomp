/* hw60 `hi |= 1` takes the EXPLICIT extract/reassemble form; the `|= 1` at +8 is a BYTE
 * field and needs a real bitfield type. */

#include "nitro/fx_types.h"

typedef struct { unsigned int lo : 8, rest : 24; } Byte8;
extern void SetIndexedSlot(int self, int index, void *cb);
extern void Ov197_SeekTarget(void);

void Ov197_ResetOrientationAndAdvance(int self) {
    int *obj = *(int **)(self + 4);
    unsigned short w;

    obj[8] = 0;
    obj[9] = 0;
    w = *(unsigned short *)(*obj + 0x60);
    *(unsigned short *)(*obj + 0x60) =
        (unsigned short)((w & ~0xff00)
                         | (((((unsigned int)w << 0x10) >> 0x18 | 1) << 0x18) >> 0x10));
    ((Byte8 *)(*(int *)(*obj + 0x388) + 8))->lo |= 1;
    *(VecFx32 *)(obj + 10) = *(VecFx32 *)obj[1];
    SetIndexedSlot(self, *(signed char *)(self + 0x20), &Ov197_SeekTarget);
}

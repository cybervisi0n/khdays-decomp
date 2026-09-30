/* Constructor tail of an ov259 helper: installs its handlers (+8 update veneer 020d28c0, +0xc 020d28cc,
 * +0x30 020d29b0, +0x1d0 020d29a8), +0x70 = 0xa00, sets bits 2 and 4 of +0x1ae, drops bit 6 of the
 * +0x60 high byte and builds its hit capsule (rest axis, length -0x500, radius 0x700) into a
 * +0x22c pool slot at +0x384, marked with bit 1; +0x388 (busy) clears. */

#include "nitro/fx_types.h"

typedef struct { unsigned f : 8; } B8;
typedef struct { VecFx32 pos; VecFx32 axis; int length; int radius; } Capsule;

extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern void *Ov107_Mover_New(const Capsule *capsule);
extern void func_ov259_020d28c0(void);
extern void Ov259_Item_TickSyncXform(void);
extern void Ov259_Item_CreateAiTask(void);
extern void Ov259_OnHitIgnore_2(void);
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042240;

void Ov259_HelperInitTail(char *self)
{
    Capsule cap;

    *(void **)(self + 8) = func_ov259_020d28c0;
    *(void **)(self + 0xc) = Ov259_Item_TickSyncXform;
    *(void **)(self + 0x30) = Ov259_Item_CreateAiTask;
    *(void **)(self + 0x1d0) = Ov259_OnHitIgnore_2;
    *(int *)(self + 0x70) = 0xa00;
    *(unsigned short *)(self + 0x1ae) |= 0x14;
    {
        unsigned short hw = *(unsigned short *)(self + 0x60);
        *(unsigned short *)(self + 0x60) = (hw & ~0xff00) |
            (((unsigned int)(unsigned short)((((unsigned int)hw << 0x10) >> 0x18) & ~0x40) << 0x18) >> 0x10);
    }
    cap.pos = data_02041dc8;
    cap.axis = data_02042240;
    cap.length = -0x500;
    cap.radius = 0x700;
    *(int **)(self + 0x384) = List_InsertSorted(self + 0x22c, 0x10, 100);
    **(void ***)(self + 0x384) = Ov107_Mover_New(&cap);
    ((B8 *)(*(char **)(self + 0x384) + 8))->f |= 2;
    *(int *)(self + 0x388) = 0;
}

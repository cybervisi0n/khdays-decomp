/* Build the ov283 helper: its brain (020ceee8), update (020cef04) and spawn (020cef4c) callbacks are
 * installed, bits 1-3 and 6 of the +0x60 high byte and bits 2/4 of +0x1ae are set, the body is a
 * 0.125 sphere at the origin with no speed, model 1 of the +0x388 set becomes the +0x384 rig
 * (subscribed to the scene) and a 1.0-long upright capsule of that radius is registered in the
 * +0x144 pool (+0x38c). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 pos; VecFx32 axis; int length; int radius; } Capsule;

extern void Ov283_OnDespawn(void);
extern void Ov283_Item_TickSyncXform(void);
extern void Ov283_Item_CreateAiTask(void);
extern void *Ov107_PackTextureHandle(int set, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern int *List_InsertSorted(void *pool, int count, int size);
extern int Ov107_Mover_New(const Capsule *capsule);
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042258;

void Ov283_BuildHelper(char *self)
{
    VecFx32 origin;
    Capsule body;
    int set = *(int *)(self + 0x388);
    int *cyl;

    *(void **)(self + 8) = Ov283_OnDespawn;
    *(void **)(self + 0xc) = Ov283_Item_TickSyncXform;
    *(void **)(self + 0x30) = Ov283_Item_CreateAiTask;
    {
        u16 hw = *(u16 *)(self + 0x60);
        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x4e) << 0x18) >> 0x10);
    }
    *(u16 *)(self + 0x1ae) |= 0x14;
    *(int *)(self + 0x70) = 0x200;
    origin = data_02041dc8;
    *(VecFx32 *)(self + 0x64) = origin;
    *(int *)(self + 0x54) = 0;
    *(int *)(self + 0x58) = 0;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(set, 1));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    body.pos = origin;
    body.axis = data_02042258;
    body.length = 0x1000;
    body.radius = *(int *)(self + 0x70);
    cyl = List_InsertSorted(self + 0x144, 4, 0x64);
    *(int *)(self + 0x38c) = *cyl = Ov107_Mover_New(&body);
}

/* Constructor of the ov258 thrown item: installs its brain (020cfed8), update (020cfef4) and spawn
 * (020cffb0) callbacks, sets bits 5 and 6 of the +0x60 high byte and bits 2-4 of +0x1ae, a tiny
 * radius (3), the body at the origin with no speed; model 0x28 of the owner's +0x390 set becomes the
 * +0x384 rig (subscribed to +0x9c) and a 13.9-long upright capsule of radius 0.7 is registered in the
 * +0x144 pool (+0x388). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { VecFx32 pos; VecFx32 axis; int length; int radius; } Capsule;

extern void Ov258_OnDespawn(void);
extern void Ov258_TickSyncXform(void);
extern void Ov258_CreateAiTask_2(void);
extern void *Ov107_PackTextureHandle(int set, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern int *List_InsertSorted(void *pool, int count, int size);
extern int Ov107_Mover_New(const Capsule *capsule);
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042258;

void Ov258_ItemConstruct(char *self)
{
    Capsule body;
    VecFx32 origin;
    int set = *(int *)(self + 0x390);
    int *slot;

    *(Callback *)(self + 8) = Ov258_OnDespawn;
    *(Callback *)(self + 0xc) = Ov258_TickSyncXform;
    *(Callback *)(self + 0x30) = Ov258_CreateAiTask_2;
    {
        u16 hw = *(u16 *)(self + 0x60);

        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x40) << 0x18) >> 0x10);
    }
    *(int *)(self + 0x70) = 3;
    origin = data_02041dc8;
    *(VecFx32 *)(self + 0x64) = origin;
    *(int *)(self + 0x54) = 0;
    *(int *)(self + 0x58) = 0;
    *(u16 *)(self + 0x1ae) |= 0xc;
    {
        u16 hw = *(u16 *)(self + 0x60);

        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x20) << 0x18) >> 0x10);
    }
    *(u16 *)(self + 0x1ae) |= 0x10;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(set, 0x28));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    body.pos = origin;
    body.axis = data_02042258;
    body.length = 0xde00;
    body.radius = 0xb40;
    slot = List_InsertSorted(self + 0x144, 4, 100);
    *(int *)(self + 0x388) = *slot = Ov107_Mover_New(&body);
}

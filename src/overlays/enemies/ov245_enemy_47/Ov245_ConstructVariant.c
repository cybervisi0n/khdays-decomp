/* Ov245_ConstructVariant -- constructor of the ov245 actor variant: installs the handlers (+8 tick,
 * +0xc draw, +0x28 / +0x2c / +0x30 / +0x34 callbacks, +0x1c message, +0x24 hook, +0x1dc finish),
 * raises bits 2-6 of the +0x60 high byte, sets the +0x70 scale to 2.0, builds the primary item
 * from pool entry 0x15 of the +0x3c8 pool (+0x384, subscribed to +0x9c) and its "tag_00" joint
 * (+0x3b8), a shape on the +0x144 list (+0x388) from a request at the origin along
 * data_02042258 with rate 7.0 / value 2.0, ten +0x390 slots from 020d284c, and clears +0x38c. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 pos; VecFx32 axis; int rate; int value; } ShapeRequest;
typedef void (*Callback)(void);
struct Ov245Self { char pad[0x390]; int slots[10]; };

extern void Ov245_OnDespawn(void);
extern void Ov245_Variant_TickFollowOwner(void);
extern void Ov245_Variant_ForwardRegionEventToParts(void);
extern void Ov245_Variant_NotifyPartsThenBase(void);
extern void Ov245_Variant_CreateAiTask(void);
extern void Ov245_Variant_PostTickAim(void);
extern void Ov245_FilterStateMsg(void);
extern void Ov245_FilterMessage(void);
extern void Ov245_Variant_ApplyAnim(void);
extern void *Ov107_PackTextureHandle(int pool, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern int InsertSortedEntryWithKey(int item, int kind, const char *name);
extern int *List_InsertSorted(void *list, int stride, int max);
extern int Ov107_Mover_New(ShapeRequest *req);
extern int Ov245_Hopper_New(int self);
extern const char data_ov245_020d722c[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042258;

void Ov245_ConstructVariant(int self) {
    int pool = *(int *)(self + 0x3c8);
    ShapeRequest req;
    int *slot;
    int i;

    *(Callback *)(self + 0x8) = Ov245_OnDespawn;
    *(Callback *)(self + 0xc) = Ov245_Variant_TickFollowOwner;
    *(Callback *)(self + 0x28) = Ov245_Variant_ForwardRegionEventToParts;
    *(Callback *)(self + 0x2c) = Ov245_Variant_NotifyPartsThenBase;
    *(Callback *)(self + 0x30) = Ov245_Variant_CreateAiTask;
    *(Callback *)(self + 0x34) = Ov245_Variant_PostTickAim;
    *(Callback *)(self + 0x1c) = Ov245_FilterStateMsg;
    *(Callback *)(self + 0x24) = Ov245_FilterMessage;
    *(Callback *)(self + 0x1dc) = Ov245_Variant_ApplyAnim;
    {
        u16 hw = *(u16 *)(self + 0x60);
        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x7c) << 0x18) >> 0x10);
    }
    *(int *)(self + 0x70) = 0x2000;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(pool, 0x15));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    *(int *)(self + 0x3b8) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov245_020d722c);
    req.pos = data_02041dc8;
    req.axis = data_02042258;
    req.rate = 0x7000;
    req.value = 0x2000;
    slot = List_InsertSorted((void *)(self + 0x144), 4, 0x64);
    *(int *)(self + 0x388) = *slot = Ov107_Mover_New(&req);
    for (i = 0; i < 10; i++) {
        ((struct Ov245Self *)self)->slots[i] = Ov245_Hopper_New(self);
    }
    *(int *)(self + 0x38c) = 0;
}

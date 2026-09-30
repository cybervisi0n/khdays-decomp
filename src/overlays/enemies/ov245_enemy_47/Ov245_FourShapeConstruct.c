/* Ov245_FourShapeConstruct -- constructor of the ov245 four-shape actor: installs the handlers (+8
 * tick, +0xc draw, +0x1c message, +0x30 / +0x34 callbacks, +0x24 hook, +0x1d0 hit, +0x1dc
 * finish), raises bits 2, 4, 5 and 6 of the +0x60 high byte, seeds the +0x64 pose at scale 1.5,
 * builds the primary item from pool entry 0x13 of the +0x3b4 pool (+0x384, subscribed) with its
 * four named joints (+0x3b0 kind 3, +0x3a4 / +0x3a8 / +0x3ac kind 1), the +0x3b8 / +0x3c0
 * sub-items from entries 0x1e / 0x1f (attached, bit 1 of +0x5c), a shape on the +0x22c list
 * (+0x388, rate 3.0 / value 1.5 along data_02042264, bit 1 of its +8 low byte) and four on the
 * +0x144 list (+0x38c.., value 0.75, the last 1.5); +0x39c starts empty. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 pos; VecFx32 axis; int rate; int value; } ShapeRequest;
typedef void (*Callback)(void);
struct w8 { unsigned int lo : 8, rest : 24; };
struct Ov245Self { char pad[0x38c]; int shapes[4]; };

extern void Ov245_TeardownRig(void);
extern void Ov245_FourShape_TickFollowOwner(void);
extern void Ov245_SlotSpawnMsg2(void);
extern void Ov245_FourShape_CreateAiTask(void);
extern void Ov245_ChainTeardown(void);
extern void Ov245_FilterMessage(void);
extern void Ov245_FourShape_OnHit(void);
extern void Ov245_FourShape_ApplyAnim(void);
extern void *Ov107_PackTextureHandle(int pool, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern int InsertSortedEntryWithKey(int item, int kind, const char *name);
extern void Ov107_EnqueueValue(int self, int item);
extern int *List_InsertSorted(void *list, int stride, int max);
extern int Ov107_Mover_New(ShapeRequest *req);
extern const char data_ov245_020d7234[];
extern const char data_ov245_020d723c[];
extern const char data_ov245_020d7244[];
extern const char data_ov245_020d724c[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042264;

static inline void VecSetP_(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

/* self is a byte pointer, as in the ROM's unsigned address arithmetic */
void Ov245_FourShapeConstruct(char *self) {
    int pool = *(int *)(self + 0x3b4);
    ShapeRequest req;
    int *slot;
    int item;
    int i;

    *(Callback *)(self + 0x8) = Ov245_TeardownRig;
    *(Callback *)(self + 0xc) = Ov245_FourShape_TickFollowOwner;
    *(Callback *)(self + 0x1c) = Ov245_SlotSpawnMsg2;
    *(Callback *)(self + 0x30) = Ov245_FourShape_CreateAiTask;
    *(Callback *)(self + 0x34) = Ov245_ChainTeardown;
    *(Callback *)(self + 0x24) = Ov245_FilterMessage;
    *(Callback *)(self + 0x1d0) = Ov245_FourShape_OnHit;
    *(Callback *)(self + 0x1dc) = Ov245_FourShape_ApplyAnim;
    {
        u16 hw = *(u16 *)(self + 0x60);
        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x74) << 0x18) >> 0x10);
    }
    *(int *)(self + 0x70) = 0x1800;
    VecSetP_((VecFx32 *)(self + 0x64), 0, *(int *)(self + 0x70), 0);
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(pool, 0x13));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    *(int *)(self + 0x3b0) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov245_020d7234);
    *(int *)(self + 0x3a4) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov245_020d723c);
    *(int *)(self + 0x3a8) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov245_020d7244);
    *(int *)(self + 0x3ac) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov245_020d724c);
    item = *(int *)(self + 0x3b8) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(pool, 0x1e));
    Ov107_EnqueueValue(self, item);
    *(int *)(item + 0x5c) |= 2;
    item = *(int *)(self + 0x3c0) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(pool, 0x1f));
    Ov107_EnqueueValue(self, item);
    *(int *)(item + 0x5c) |= 2;
    req.pos = data_02041dc8;
    req.axis = data_02042264;
    req.rate = 0x3000;
    req.value = 0x1800;
    *(int **)(self + 0x388) = List_InsertSorted((void *)(self + 0x22c), 0x10, 0x64);
    **(int **)(self + 0x388) = Ov107_Mover_New(&req);
    ((struct w8 *)(*(int *)(self + 0x388) + 8))->lo |= 2;
    for (i = 0; i < 4; i++) {
        req.value = i == 3 ? 0x1800 : 0xc00;
        slot = List_InsertSorted((void *)(self + 0x144), 4, 0x64);
        ((struct Ov245Self *)self)->shapes[i] = *slot = Ov107_Mover_New(&req);
    }
    *(int *)(self + 0x39c) = 0;
}

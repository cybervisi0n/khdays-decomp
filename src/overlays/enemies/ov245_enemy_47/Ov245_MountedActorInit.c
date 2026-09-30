/* Ov245_MountedActorInit -- constructor of the ov245 mounted actor: installs the handlers (+8
 * tick, +0xc draw, +0x1c message, +0x30 / +0x34 callbacks, +0x24 hook, +0x1dc finish), raises bit
 * 2 of +0x1ae and bits 2-6 of the +0x60 high byte, seeds the +0x64 pose at scale 1.25, builds the
 * primary item from pool entry 0x14 of the +0x398 pool (+0x384, subscribed), its named joint
 * (+0x394, kind 3) and the named motion of entry 0xf (+0x39c), the three +0x3b4 slot items from
 * entries 0x21..0x23 (attached, bit 1 of +0x5c), and from a request along data_02042258 at the
 * +0x70 scale two shapes: rate 5.0 on the pool's +0x22c list (+0x388) and rate 10.0 on the
 * +0x144 list (+0x38c); +0x390 starts empty. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 pos; VecFx32 axis; int rate; int value; } ShapeRequest;
typedef void (*Callback)(void);
struct Ov245Slot { int pItem; int pad4; };
struct Ov245Self { char pad[0x3b4]; struct Ov245Slot slots[3]; };

extern void Ov245_Actor3_Destroy(void);
extern void Ov245_MountedPoseSync(void);
extern void Ov245_SlotSpawnMsg5(void);
extern void Ov245_SpawnActorRegistryEntry_3(void);
extern void Ov245_UpdateClaws(void);
extern void Ov245_FilterMessage(void);
extern void Ov245_Mounted_ApplyAnim(void);
extern void *Ov107_PackTextureHandle(int pool, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern int InsertSortedEntryWithKey(int item, int kind, const char *name);
extern int Ov107_CreateNamedResourceBinding(void *item, const char *name);
extern void Ov107_EnqueueValue(int self, int item);
extern int *List_InsertSorted(void *list, int stride, int max);
extern int Ov107_Mover_New(ShapeRequest *req);
extern const char data_ov245_020d7254[];
extern const char data_ov245_020d725c[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042258;

static inline void VEC_Set(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void Ov245_MountedActorInit(int self) {
    int pool = *(int *)(self + 0x398);
    ShapeRequest req;
    int *slot;
    int i;
    int item;

    *(Callback *)(self + 0x8) = Ov245_Actor3_Destroy;
    *(Callback *)(self + 0xc) = Ov245_MountedPoseSync;
    *(Callback *)(self + 0x1c) = Ov245_SlotSpawnMsg5;
    *(Callback *)(self + 0x30) = Ov245_SpawnActorRegistryEntry_3;
    *(Callback *)(self + 0x34) = Ov245_UpdateClaws;
    *(Callback *)(self + 0x24) = Ov245_FilterMessage;
    *(Callback *)(self + 0x1dc) = Ov245_Mounted_ApplyAnim;
    *(u16 *)(self + 0x100 + 0xae) |= 4;
    {
        u16 hw = *(u16 *)(self + 0x60);
        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x7c) << 0x18) >> 0x10);
    }
    *(int *)(self + 0x70) = 0x1400;
    VEC_Set((VecFx32 *)(self + 0x64), 0, *(int *)(self + 0x70), 0);
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(pool, 0x14));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    *(int *)(self + 0x394) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov245_020d7254);
    *(int *)(self + 0x39c) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(pool, 0xf), data_ov245_020d725c);
    for (i = 0; i < 3; i++) {
        item = ((struct Ov245Self *)self)->slots[i].pItem = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(pool, i + 0x21));
        Ov107_EnqueueValue(self, item);
        *(int *)(((struct Ov245Self *)self)->slots[i].pItem + 0x5c) |= 2;
    }
    req.pos = data_02041dc8;
    req.axis = data_02042258;
    req.value = *(int *)(self + 0x70);
    req.rate = 0x5000;
    *(int **)(self + 0x388) = List_InsertSorted((void *)(*(int *)(self + 0x398) + 0x22c), 0x10, 0x64);
    **(int **)(self + 0x388) = Ov107_Mover_New(&req);
    req.rate = 0xa000;
    slot = List_InsertSorted((void *)(self + 0x144), 4, 0x64);
    *(int *)(self + 0x38c) = *slot = Ov107_Mover_New(&req);
    *(int *)(self + 0x390) = 0;
}

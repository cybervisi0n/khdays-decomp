/* Constructor of the ov204 enemy (and its byte-identical twin ov205; variant of the ov139/140
 * constructor): installs the handlers (+8 tick, +0xc draw, +0x1c message, +0x30 callback,
 * +0x1d0 hit, +0x1e0 callback, +0x1dc finish), clears +0x1f4 and seeds the +0x64 pose (scale
 * 0x1000, y 0x1000); builds the primary item from pool entry 0 (+0x384, subscribed), keeps the
 * named motion handle from pool entry 1 (+0x390), the seven sub-items listed by the overlay's
 * 0x020d35e0 table into a fresh 56-byte slot table (+0x394, attached, bit 1), registers four
 * reactions (id 0x3000: 0/1 at the 0x020d35c4 lift, 1/1, 2/1 and 4/1 without one) and two
 * placements on the +0x22c/+0x144 lists (+0x388/+0x38c) from the +0x64 pose, then loads sound
 * 0x132. */

#include "nitro/fx_types.h"

typedef void (*Callback)(void);

struct PoolIds {
    int id[7];
};

struct Ov139SubitemSlot {
    int pItem;
    int pad4;
};

extern void Ov204_ReleaseSubObjectsAndListThenNotify(void);
extern void Ov204_TickAndSyncChildren(void);
extern void Ov204_OnMessage(void);
extern void Ov204_CreateRegistryEntryAndLink(void);
extern void Ov204_OnHit(void);
extern void Ov204_RequestSubState10IfNotState9(void);
extern void Ov204_Model_SetTrack0(void);
extern void *Ov107_PackTextureHandle(int pool, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern int Ov107_CreateNamedResourceBinding(void *item, const char *name);
extern void *CallocInstance(int size);
extern void Ov107_EnqueueValue(char *self, int item);
extern void Ov107_Actor_SetAttachSlot(char *self, int a, int b, VecFx32 *lift, int id);
extern int *List_InsertSorted(void *list, int stride, int max);
extern int Ov107_CloneResourceTransform(char *pose);
extern void Res_RequestIdPair(int id);
extern const struct PoolIds data_ov204_020d35e0;
extern const VecFx32 data_ov204_020d35c4;
extern const char data_ov204_020d36ac[];

void Ov204_Construct(char *self)
{
    struct PoolIds pools;
    VecFx32 lift;
    int *p;
    int i;

    pools = data_ov204_020d35e0;
    lift = data_ov204_020d35c4;
    *(Callback *)(self + 0x8) = Ov204_ReleaseSubObjectsAndListThenNotify;
    *(Callback *)(self + 0xc) = Ov204_TickAndSyncChildren;
    *(Callback *)(self + 0x1c) = Ov204_OnMessage;
    *(Callback *)(self + 0x30) = Ov204_CreateRegistryEntryAndLink;
    *(Callback *)(self + 0x1d0) = Ov204_OnHit;
    *(Callback *)(self + 0x1e0) = Ov204_RequestSubState10IfNotState9;
    *(Callback *)(self + 0x1dc) = Ov204_Model_SetTrack0;
    *(int *)(self + 0x1f4) = 0;
    *(int *)(self + 0x70) = 0x1000;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x1000;
    *(int *)(self + 0x6c) = 0;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle((int)self, 0));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    *(int *)(self + 0x390) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle((int)self, 1), data_ov204_020d36ac);
    *(void **)(self + 0x394) = CallocInstance(0x38);
    for (i = 0; i < 7; i++) {
        (*(struct Ov139SubitemSlot **)(self + 0x394))[i].pItem =
            CreateSubitemInstance0xB4(Ov107_PackTextureHandle((int)self, pools.id[i]));
        Ov107_EnqueueValue(self, (*(struct Ov139SubitemSlot **)(self + 0x394))[i].pItem);
        *(int *)((*(struct Ov139SubitemSlot **)(self + 0x394))[i].pItem + 0x5c) |= 2;
    }
    Ov107_Actor_SetAttachSlot(self, 0, 1, &lift, 0x3000);
    Ov107_Actor_SetAttachSlot(self, 1, 1, 0, 0x3000);
    Ov107_Actor_SetAttachSlot(self, 2, 1, 0, 0x3000);
    Ov107_Actor_SetAttachSlot(self, 4, 1, 0, 0x3000);
    *(int **)(self + 0x388) = List_InsertSorted(self + 0x22c, 0x10, 0x64);
    **(int **)(self + 0x388) = Ov107_CloneResourceTransform(self + 0x64);
    p = List_InsertSorted(self + 0x144, 4, 0x64);
    *(int *)(self + 0x38c) = *p = Ov107_CloneResourceTransform(self + 0x64);
    Res_RequestIdPair(0x132);
}

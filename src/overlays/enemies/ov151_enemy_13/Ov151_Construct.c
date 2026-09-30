/* Constructor of the ov151 enemy (and its byte-identical twin): installs the handlers (+8 tick,
 * +0xc draw, +0x1c message, +0x30/+0x28/+0x2c/+0x34 callbacks, +0x1d0 hit, +0x1dc finish) and
 * seeds the +0x64 pose (scale 0xc00, y 0xc00); builds the primary item from pool entry 0
 * (+0x384, subscribed) with two named attachments (+0x394 kind 3, +0x398 kind 1 whose +0x14
 * point is kept in +0x2cc), keeps the named motion handle from pool entry 1 (+0x3cc), the five
 * sub-items listed by the overlay's +0xebe0 table into a fresh 40-byte slot table (+0x390,
 * attached, bit 1), registers four reactions (0/1/2/4, id 0x2999) and two placements on the
 * +0x22c/+0x144 lists (+0x388/+0x38c) from the pose at the origin with scale 0xc00, then three
 * summoned pets (cc994) into a 12-byte table (+0x3c8) and loads sound 0x14f. */

#include "nitro/fx_types.h"

typedef void (*Callback)(void);

struct PoolIds {
    int id[5];
};

struct Pose {
    VecFx32 pos;
    int scale;
};

struct Ov151SubitemSlot {
    int pItem;
    int pad4;
};

extern void Ov151_DestroySubListAndArraySlotsThenNotify(void);
extern void Ov151_TickAndSyncMarkerSrt(void);
extern void Ov151_HandleCommand(void);
extern void Ov151_CreateRegistryEntryAndLink_2(void);
extern void Ov151_ForwardRegionEventToParts(void);
extern void Ov151_NotifyPartsThenBase(void);
extern void Ov151_ReleaseByStateAndSyncSrt(void);
extern void Ov151_StaggerFlipTick(void);
extern void Ov151_Model_SetTrack0(void);
extern void *Ov107_PackTextureHandle(int pool, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern int InsertSortedEntryWithKey(int item, int a, const char *name);
extern int Ov107_CreateNamedResourceBinding(void *item, const char *name);
extern void *CallocInstance(int size);
extern void Ov107_EnqueueValue(char *self, int item);
extern void Ov107_Actor_SetAttachSlot(char *self, int a, int b, void *lift, int id);
extern int *List_InsertSorted(void *list, int stride, int max);
extern int Ov107_CloneResourceTransform(struct Pose *pose);
extern int Ov151_New(char *self);
extern void Res_RequestIdPair(int id);
extern const struct PoolIds data_ov151_020cebe0;
extern const char data_ov151_020cec6c[];
extern const char data_ov151_020cec70[];
extern const char data_ov151_020cec78[];
extern const VecFx32 data_02041dc8;

void Ov151_Construct(char *self)
{
    struct PoolIds pools;
    struct Pose pose;
    int *p;
    int i;

    pools = data_ov151_020cebe0;
    *(Callback *)(self + 0x8) = Ov151_DestroySubListAndArraySlotsThenNotify;
    *(Callback *)(self + 0xc) = Ov151_TickAndSyncMarkerSrt;
    *(Callback *)(self + 0x1c) = Ov151_HandleCommand;
    *(Callback *)(self + 0x30) = Ov151_CreateRegistryEntryAndLink_2;
    *(Callback *)(self + 0x28) = Ov151_ForwardRegionEventToParts;
    *(Callback *)(self + 0x2c) = Ov151_NotifyPartsThenBase;
    *(Callback *)(self + 0x34) = Ov151_ReleaseByStateAndSyncSrt;
    *(Callback *)(self + 0x1d0) = Ov151_StaggerFlipTick;
    *(Callback *)(self + 0x1dc) = Ov151_Model_SetTrack0;
    *(int *)(self + 0x70) = 0xc00;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0xc00;
    *(int *)(self + 0x6c) = 0;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle((int)self, 0));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    *(int *)(self + 0x394) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov151_020cec6c);
    *(int *)(self + 0x398) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov151_020cec70);
    *(int *)(self + 0x2cc) = *(int *)(self + 0x398) + 0x14;
    *(int *)(self + 0x3cc) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle((int)self, 1), data_ov151_020cec78);
    *(void **)(self + 0x390) = CallocInstance(0x28);
    for (i = 0; i < 5; i++) {
        (*(struct Ov151SubitemSlot **)(self + 0x390))[i].pItem =
            CreateSubitemInstance0xB4(Ov107_PackTextureHandle((int)self, pools.id[i]));
        Ov107_EnqueueValue(self, (*(struct Ov151SubitemSlot **)(self + 0x390))[i].pItem);
        *(int *)((*(struct Ov151SubitemSlot **)(self + 0x390))[i].pItem + 0x5c) |= 2;
    }
    Ov107_Actor_SetAttachSlot(self, 0, 1, 0, 0x2999);
    Ov107_Actor_SetAttachSlot(self, 1, 1, 0, 0x2999);
    Ov107_Actor_SetAttachSlot(self, 2, 1, 0, 0x2999);
    Ov107_Actor_SetAttachSlot(self, 4, 1, 0, 0x2999);
    pose.pos = data_02041dc8;
    pose.scale = 0xc00;
    *(int **)(self + 0x388) = List_InsertSorted(self + 0x22c, 0x10, 0x64);
    **(int **)(self + 0x388) = Ov107_CloneResourceTransform(&pose);
    p = List_InsertSorted(self + 0x144, 4, 0x64);
    *(int *)(self + 0x38c) = *p = Ov107_CloneResourceTransform(&pose);
    *(void **)(self + 0x3c8) = CallocInstance(0xc);
    for (i = 0; i < 3; i++) {
        (*(int **)(self + 0x3c8))[i] = Ov151_New(self);
    }
    Res_RequestIdPair(0x14f);
}

/* Constructor of the ov156 enemy (and its byte-identical twin): sets bit 8 of the +0 flags,
 * installs the handlers (+8 tick, +0x1c message, +0x30/+0x28/+0x2c/+0x10/+0x34 callbacks, +0x1e0
 * callback, +0x1d0 hit, +0x1dc finish), seeds the +0x64 pose (scale 0xb00, y 0xb00) and bit 4 of
 * +0x1ae; builds the primary item from pool entry 0 (+0x384, subscribed) with two named
 * attachments (+0x398/+0x39c), the four sub-items listed by the overlay's +0xed80 table into a
 * fresh 32-byte slot table (+0x3a0, attached, bit 1), registers four reactions (0/1/2/4 at the
 * +0xed74 lift), two placements on the +0x144 list (+0x390 at the origin with scale 0x500,
 * +0x394 at y 0xb00 with scale 0xb00) and two on the +0x22c list (+0x38c at the origin with
 * scale 0xd33, +0x388 at y 0xb00 with scale 0x266, bit 1 raised on its +8 flags), two held items
 * (cdee4) into an 8-byte table (+0x3a4) and loads sound 0x13d. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);

struct bf { unsigned int b : 8; };

struct PoolIds {
    int id[4];
};

struct Pose {
    VecFx32 pos;
    int scale;
};

struct Ov156SubitemSlot {
    int pItem;
    int pad4;
};

extern void Ov157_DestroySubObjectsAndSlotsThenNotify(void);
extern void Ov157_HandleSpawnMessage(void);
extern void Ov157_CreateRegistryEntryAndLink(void);
extern void Ov157_ForwardRegionEventToParts(void);
extern void Ov157_NotifyPartsThenBase(void);
extern void Ov157_RefreshPose(void);
extern void Ov157_TickHook(void);
extern void Ov157_TryBeginSubState7(void);
extern void Ov157_OnHit(void);
extern void Ov157_Model_SetTracks0And3(void);
extern void *Ov107_PackTextureHandle(int pool, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern int InsertSortedEntryWithKey(int item, int a, const char *name);
extern void *CallocInstance(int size);
extern void Ov107_EnqueueValue(char *self, int item);
extern void Ov107_Actor_SetAttachSlot(char *self, int a, int b, VecFx32 *lift, int id);
extern int *List_InsertSorted(void *list, int stride, int max);
extern int Ov107_CloneResourceTransform(struct Pose *pose);
extern int Ov157_Actor_New(char *self);
extern void Res_RequestIdPair(int id);
extern const struct PoolIds data_ov157_020d0ba0;
extern const VecFx32 data_ov157_020d0b94;
extern const char data_ov157_020d0c0c[];
extern const char data_ov157_020d0c14[];
extern const VecFx32 data_02041dc8;

void Ov157_Construct(char *self)
{
    struct PoolIds pools;
    struct Pose pose;
    VecFx32 lift;
    VecFx32 zero;
    int *p;
    int i;

    pools = data_ov157_020d0ba0;
    lift = data_ov157_020d0b94;
    *(u16 *)self |= 0x100;
    *(Callback *)(self + 0x8) = Ov157_DestroySubObjectsAndSlotsThenNotify;
    *(Callback *)(self + 0x1c) = Ov157_HandleSpawnMessage;
    *(Callback *)(self + 0x30) = Ov157_CreateRegistryEntryAndLink;
    *(Callback *)(self + 0x28) = Ov157_ForwardRegionEventToParts;
    *(Callback *)(self + 0x2c) = Ov157_NotifyPartsThenBase;
    *(Callback *)(self + 0x10) = Ov157_RefreshPose;
    *(Callback *)(self + 0x34) = Ov157_TickHook;
    *(Callback *)(self + 0x1e0) = Ov157_TryBeginSubState7;
    *(Callback *)(self + 0x1d0) = Ov157_OnHit;
    *(Callback *)(self + 0x1dc) = Ov157_Model_SetTracks0And3;
    *(int *)(self + 0x70) = 0xb00;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0xb00;
    *(int *)(self + 0x6c) = 0;
    *(u16 *)(self + 0x100 + 0xae) |= 0x10;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle((int)self, 0));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    *(int *)(self + 0x398) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov157_020d0c0c);
    *(int *)(self + 0x39c) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov157_020d0c14);
    *(void **)(self + 0x3a0) = CallocInstance(0x20);
    for (i = 0; i < 4; i++) {
        (*(struct Ov156SubitemSlot **)(self + 0x3a0))[i].pItem =
            CreateSubitemInstance0xB4(Ov107_PackTextureHandle((int)self, pools.id[i]));
        Ov107_EnqueueValue(self, (*(struct Ov156SubitemSlot **)(self + 0x3a0))[i].pItem);
        *(int *)((*(struct Ov156SubitemSlot **)(self + 0x3a0))[i].pItem + 0x5c) |= 2;
    }
    Ov107_Actor_SetAttachSlot(self, 0, 1, &lift, 0x3e66);
    Ov107_Actor_SetAttachSlot(self, 1, 1, &lift, 0x1f33);
    Ov107_Actor_SetAttachSlot(self, 2, 1, &lift, 0x1f33);
    Ov107_Actor_SetAttachSlot(self, 4, 1, &lift, 0x1f33);
    zero = data_02041dc8;
    pose.pos = zero;
    pose.scale = 0x500;
    p = List_InsertSorted(self + 0x144, 4, 0x64);
    *(int *)(self + 0x390) = *p = Ov107_CloneResourceTransform(&pose);
    pose.pos = zero;
    pose.scale = 0xd33;
    *(int **)(self + 0x38c) = List_InsertSorted(self + 0x22c, 0x10, 0x64);
    **(int **)(self + 0x38c) = Ov107_CloneResourceTransform(&pose);
    pose.pos.x = 0;
    pose.pos.y = 0xb00;
    pose.pos.z = 0;
    pose.scale = 0xb00;
    p = List_InsertSorted(self + 0x144, 4, 0x64);
    *(int *)(self + 0x394) = *p = Ov107_CloneResourceTransform(&pose);
    pose.pos.x = 0;
    pose.pos.y = 0xb00;
    pose.pos.z = 0;
    pose.scale = 0x266;
    *(int **)(self + 0x388) = List_InsertSorted(self + 0x22c, 0x10, 0x64);
    **(int **)(self + 0x388) = Ov107_CloneResourceTransform(&pose);
    ((struct bf *)(*(int *)(self + 0x388) + 8))->b |= 2;
    *(void **)(self + 0x3a4) = CallocInstance(8);
    for (i = 0; i < 2; i++) {
        (*(int **)(self + 0x3a4))[i] = Ov157_Actor_New(self);
    }
    Res_RequestIdPair(0x13d);
}

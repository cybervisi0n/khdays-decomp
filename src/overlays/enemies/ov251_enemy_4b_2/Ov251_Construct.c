/* Constructor of the ov250 enemy (and its byte-identical twin): installs the handlers (+8 tick,
 * +0xc draw, +0x20/+0x1c message callbacks, +0x30/+0x34 callbacks, +0x1e0 callback, +0x1d0 hit,
 * +0x1dc finish), copies the overlay's bounding box into +0x1fc, seeds the +0x64 pose (scale
 * 0x2120, y 0x2120) and bits 3/4 of +0x1ae; builds the primary item from pool entry 0 (+0x384,
 * its +4 block configured with 0x80, subscribed) with three named attachments (+0x39c/+0x3a0/
 * +0x3a4), keeps the named motion handle from pool entry 1 (+0x390), the three sub-items listed
 * by the overlay's +0x2890 table into a fresh 24-byte slot table (+0x398, attached, bit 1),
 * registers reaction 2/2 (id 0x2120) and two placements on the +0x22c/+0x144 lists (+0x388/
 * +0x38c) from the pose at the origin, then loads sound 0x159. */

#include "nitro/fx_types.h"

typedef void (*Callback)(void);

struct PoolIds {
    int id[3];
};

struct Box {
    int xmin, ymin, zmin;
    int xmax, ymax, zmax;
};

struct Pose {
    VecFx32 pos;
    int scale;
};

struct Ov250SubitemSlot {
    int pItem;
    int pad4;
};

extern void Ov251_Destroy(void);
extern void Ov251_TickWithChildRefresh(void);
extern void Ov251_SendMessage28(void);
extern void Ov251_HandleMessage(void);
extern void Ov251_CreateRegistryEntryAndLink(void);
extern void Ov251_PropagateBlockChainThenNotify(void);
extern void Ov251_ReactionRequestSubState11(void);
extern void Ov251_OnHit(void);
extern void Ov251_Model_SetTrack0(void);
extern void *Ov107_PackTextureHandle(int pool, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern void Srt_SetTranslationXYZ(void *block, int a, int b, int c);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern int InsertSortedEntryWithKey(int item, int a, const char *name);
extern int Ov107_CreateNamedResourceBinding(void *item, const char *name);
extern void *CallocInstance(int size);
extern void Ov107_EnqueueValue(char *self, int item);
extern void Ov107_Actor_SetAttachSlot(char *self, int a, int b, void *lift, int id);
extern int *List_InsertSorted(void *list, int stride, int max);
extern int Ov107_CloneResourceTransform(struct Pose *pose);
extern void Res_RequestIdPair(int id);
extern const struct PoolIds data_ov251_020d64d0;
extern const struct Box data_ov251_020d64dc;
extern const char data_ov251_020d654c[];
extern const char data_ov251_020d6554[];
extern const char data_ov251_020d6564[];
extern const char data_ov251_020d6574[];
extern const VecFx32 data_02041dc8;

void Ov251_Construct(char *self)
{
    struct PoolIds pools;
    struct Pose pose;
    int *p;
    int i;

    pools = data_ov251_020d64d0;
    *(Callback *)(self + 0x8) = Ov251_Destroy;
    *(Callback *)(self + 0xc) = Ov251_TickWithChildRefresh;
    *(Callback *)(self + 0x20) = Ov251_SendMessage28;
    *(Callback *)(self + 0x1c) = Ov251_HandleMessage;
    *(Callback *)(self + 0x30) = Ov251_CreateRegistryEntryAndLink;
    *(Callback *)(self + 0x34) = Ov251_PropagateBlockChainThenNotify;
    *(Callback *)(self + 0x1e0) = Ov251_ReactionRequestSubState11;
    *(Callback *)(self + 0x1d0) = Ov251_OnHit;
    *(Callback *)(self + 0x1dc) = Ov251_Model_SetTrack0;
    *(struct Box *)(self + 0x1fc) = data_ov251_020d64dc;
    *(int *)(self + 0x70) = 0x2120;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x2120;
    *(int *)(self + 0x6c) = 0;
    *(unsigned short *)(self + 0x100 + 0xae) |= 0x18;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle((int)self, 0));
    Srt_SetTranslationXYZ((void *)(*(int *)(self + 0x384) + 4), 0, 0x80, 0);
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    *(int *)(self + 0x39c) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov251_020d654c);
    *(int *)(self + 0x3a0) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov251_020d6554);
    *(int *)(self + 0x3a4) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov251_020d6564);
    pose = *(struct Pose *)(self + 0x64);
    pose.pos = data_02041dc8;
    *(int *)(self + 0x390) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle((int)self, 1), data_ov251_020d6574);
    *(void **)(self + 0x398) = CallocInstance(0x18);
    for (i = 0; i < 3; i++) {
        (*(struct Ov250SubitemSlot **)(self + 0x398))[i].pItem =
            CreateSubitemInstance0xB4(Ov107_PackTextureHandle((int)self, pools.id[i]));
        Ov107_EnqueueValue(self, (*(struct Ov250SubitemSlot **)(self + 0x398))[i].pItem);
        *(int *)((*(struct Ov250SubitemSlot **)(self + 0x398))[i].pItem + 0x5c) |= 2;
    }
    Ov107_Actor_SetAttachSlot(self, 2, 2, 0, 0x2120);
    *(int **)(self + 0x388) = List_InsertSorted(self + 0x22c, 0x10, 0x64);
    **(int **)(self + 0x388) = Ov107_CloneResourceTransform(&pose);
    p = List_InsertSorted(self + 0x144, 4, 0x64);
    *(int *)(self + 0x38c) = *p = Ov107_CloneResourceTransform(&pose);
    Res_RequestIdPair(0x159);
}

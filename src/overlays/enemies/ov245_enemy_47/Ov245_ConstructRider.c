/* Ov245_ConstructRider -- constructor of the ov245 rider: installs the handlers (+8 tick, +0xc
 * draw, +0x1c message, +0x30 / +0x34 callbacks, +0x1d0 hit, +0x1dc finish), seeds the +0x64
 * pose at scale 0.5, builds the primary item from pool entry 0x26 of the +0x3cc pool (+0x384,
 * subscribed) with four named joints (+0x390..+0x39c), binds pool entry 0x27 as its motion
 * (+0x3a8 track, slot 0xc) and the named motion of entry 0x32 (+0x3a0), registers reactions
 * 1/1 and 2/1 at 1.5, then a fresh 40-byte slot table (+0x3a4) holding five sub-items: the
 * first two from the shared +0x88 model base (kinds from the data_ov245_020d71b0 table), the
 * rest from pool entries listed there, all attached (bit 1 of +0x5c). Two placements at the
 * origin (scale 0.5) go on the +0x22c list (+0x388) and the +0x144 list (+0x38c); sound 0x11a
 * is loaded. */

#include "nitro/fx_types.h"

typedef struct { VecFx32 pos; int scale; } Pose;
typedef void (*Callback)(void);
struct PoolIds { int id[5]; };
struct Ov245Slot { int pItem; int pad4; };
struct Ov245Model { char pad[0x88]; int track; };

extern void Ov245_Rider_Destroy(void);
extern void Ov245_PoseSyncChain(void);
extern void Ov245_SlotSpawnMsg5Scaled(void);
extern void Ov245_Rider_CreateAiTask(void);
extern void Ov245_ReleaseChildHeld1c(void);
extern void Ov245_HitFilterMounted(void);
extern void Ov245_BindMotion2(void);
extern void *Ov107_PackTextureHandle(int pool, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern int InsertSortedEntryWithKey(int item, int kind, const char *name);
extern void Snd_RegisterSeqAndBind(void *track, int model, void *resource, int slot);
extern void MainBlob_ResetSlotRows(int item, void *track);
extern int Ov107_CreateNamedResourceBinding(void *item, const char *name);
extern void Ov107_Actor_SetAttachSlot(int self, int a, int b, VecFx32 *lift, int id);
extern void *CallocInstance(int size);
extern void *Ov107_GetActorManager(void);
extern void Ov107_EnqueueValue(int self, int item);
extern int *List_InsertSorted(void *list, int stride, int max);
extern int Ov107_CloneResourceTransform(void *pose);
extern void Res_RequestIdPair(int id);
extern const struct PoolIds data_ov245_020d71b0;
extern const char data_ov245_020d7268[];
extern const char data_ov245_020d7274[];
extern const char data_ov245_020d7284[];
extern const char data_ov245_020d7294[];
extern const char data_ov245_020d729c[];
extern const VecFx32 data_02041dc8;

void Ov245_ConstructRider(int selfArg) {
    char *self = (char *)selfArg;   /* codegen: the local copy keeps `mov r1,#1` in the call shadow */
    struct PoolIds pools;
    Pose pose;
    int *slot;
    int i;

    pools = data_ov245_020d71b0;
    *(Callback *)(self + 0x8) = Ov245_Rider_Destroy;
    *(Callback *)(self + 0xc) = Ov245_PoseSyncChain;
    *(Callback *)(self + 0x1c) = Ov245_SlotSpawnMsg5Scaled;
    *(Callback *)(self + 0x30) = Ov245_Rider_CreateAiTask;
    *(Callback *)(self + 0x34) = Ov245_ReleaseChildHeld1c;
    *(Callback *)(self + 0x1d0) = Ov245_HitFilterMounted;
    *(Callback *)(self + 0x1dc) = Ov245_BindMotion2;
    *(int *)(self + 0x70) = 0x800;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x800;
    *(int *)(self + 0x6c) = 0;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(*(int *)(self + 0x3cc), 0x26));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    *(int *)(self + 0x390) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov245_020d7268);
    *(int *)(self + 0x394) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov245_020d7274);
    *(int *)(self + 0x398) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov245_020d7284);
    *(int *)(self + 0x39c) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov245_020d7294);
    Snd_RegisterSeqAndBind((void *)(self + 0x3a8), ((struct Ov245Model *)*(int *)(self + 0x384))->track,
                  Ov107_PackTextureHandle(*(int *)(self + 0x3cc), 0x27), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x384), (void *)(self + 0x3a8));
    *(int *)(self + 0x3a0) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(*(int *)(self + 0x3cc), 0x32), data_ov245_020d729c);
    Ov107_Actor_SetAttachSlot(self, 1, 1, 0, 0x1800);
    Ov107_Actor_SetAttachSlot(self, 2, 1, 0, 0x1800);
    *(void **)(self + 0x3a4) = CallocInstance(0x28);
    for (i = 0; i < 5; i++) {
        void *node;
        if (i < 2) {
            void *os = Ov107_GetActorManager();
            unsigned int kind = pools.id[i] & 0x1ff;
            unsigned int addr = (*(int *)((char *)os + 0x88) + 0x8000) & 0x00fffffc;
            addr = addr << 7;
            addr = addr | 0x80000000;
            node = CreateSubitemInstance0xB4((void *)(kind | addr));
        } else {
            node = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(*(int *)(self + 0x3cc), pools.id[i]));
        }
        (*(struct Ov245Slot **)(self + 0x3a4))[i].pItem = (int)node;
        Ov107_EnqueueValue(self, (*(struct Ov245Slot **)(self + 0x3a4))[i].pItem);
        *(int *)((*(struct Ov245Slot **)(self + 0x3a4))[i].pItem + 0x5c) |= 2;
    }
    pose.pos = data_02041dc8;
    pose.scale = 0x800;
    *(int **)(self + 0x388) = List_InsertSorted((void *)(self + 0x22c), 0x10, 0x64);
    **(int **)(self + 0x388) = Ov107_CloneResourceTransform(&pose);
    slot = List_InsertSorted((void *)(self + 0x144), 4, 0x64);
    *(int *)(self + 0x38c) = *slot = Ov107_CloneResourceTransform(&pose);
    Res_RequestIdPair(0x11a);
}

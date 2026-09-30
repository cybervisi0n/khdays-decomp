/* Ov245_ConstructBoss -- constructor of the ov245 boss: installs the handlers (+8 tick, +0xc
 * draw, +0x1c message, +0x30 / +0x28 / +0x2c / +0x20 callbacks, +0x24 hook, +0x1d0 hit, +0x1dc
 * finish), kind byte 2 at +0x1c9, the +0x64 pose at scale 4.75, bit 5 of the +0x60 high byte and
 * bits 3-4 of +0x1ae; builds the primary item from pool entry 0 (+0x384, callback 020cbfc8 at
 * +0x74, owner at +0x84, subscribed), binds entry 1 as its motion (+0x390 track, slot 0xc) and
 * four named joints (+0x448..+0x454), the two +0x388 items from entries 0xb / 0xc (subscribed,
 * bit 0 of +0x5c), the +0x4dc sub-item from entry 0x20 (attached, bit 1), the named motions of
 * entries 0xd / 0xe (+0x4c8 / +0x4cc), a +0x3bc query block (origin, the three axes, 8.0 and
 * 4.75 x 2) shaping two placements (+0x3b4 on the +0x22c list, +0x3b8 on +0x144), registers
 * the +0x458 spawner (kind 4), nine +0x3fc slots (020ce87c), three +0x420 parts (020cf284), three
 * +0x43c riders (020d54f0), the four +0x42c..+0x438 helpers and loads sound 0x15a. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
struct Flags5c { int bit0 : 1; };
struct Ov245Track { char pad[0x88]; int track; };
struct Ov245Query { VecFx32 pos; VecFx32 a; VecFx32 b; VecFx32 c; int w0; int w1; int w2; };
struct Ov245Boss {
    char pad[0x388];
    int items[2];          /* 0x388 */
    char pad390[0x3fc - 0x390];
    int slots[9];          /* 0x3fc */
    int parts[3];          /* 0x420 */
    int helpers[4];        /* 0x42c */
    int riders[3];         /* 0x43c */
};

extern void Ov245_Actor_Destroy(void);
extern void Ov245_CarriedPoseSync(void);
extern void Ov245_PairAndSlotMsg(void);
extern void Ov245_SpawnActorRegistryEntry(void);
extern void Ov245_UnregisterDrawList(void);
extern void Ov245_RegisterDrawList(void);
extern void Ov245_SendBlankStatus(void);
extern void Ov245_PublishCountersToMessage(void);
extern void Ov245_HitFilterStock(void);
extern void Ov245_BindMotion(void);
extern void Ov245_WingPose(void);
extern void *Ov107_PackTextureHandle(int pool, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void Snd_RegisterSeqAndBind(void *track, int model, void *resource, int slot);
extern void MainBlob_ResetSlotRows(int item, void *track);
extern int InsertSortedEntryWithKey(int item, int kind, const char *name);
extern void Ov107_EnqueueValue(int self, int item);
extern int Ov107_CreateNamedResourceBinding(void *item, const char *name);
extern int *List_InsertSorted(void *list, int stride, int max);
extern int Ov107_HitShape_NewBox(struct Ov245Query *query);
extern void Ov107_LoadSpawnRecord(int kind, void *spawner);
extern int Ov245_Projectile_New(int self);
extern int Ov245_Carrier_New(int self, int index);
extern int Ov245_Rider_New(int self, void *spawner);
extern int Ov245_Thrown_New(int self);
extern int Ov245_Variant_New(int self);
extern int Ov245_FourShape_New(int self);
extern int Ov245_Mounted_New(int self);
extern void Res_RequestIdPair(int id);
extern const char data_ov245_020d71ec[];
extern const char data_ov245_020d71f4[];
extern const char data_ov245_020d7200[];
extern const char data_ov245_020d720c[];
extern const char data_ov245_020d7218[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042264;
extern const VecFx32 data_02042258;

void Ov245_ConstructBoss(int selfArg) {
    char *self = (char *)selfArg;   /* codegen: the local copy keeps the query-block copies and address temps in ROM order */
    int *slot;
    int item;
    int i;

    *(Callback *)(self + 0x8) = Ov245_Actor_Destroy;
    *(Callback *)(self + 0xc) = Ov245_CarriedPoseSync;
    *(Callback *)(self + 0x1c) = Ov245_PairAndSlotMsg;
    *(Callback *)(self + 0x30) = Ov245_SpawnActorRegistryEntry;
    *(Callback *)(self + 0x28) = Ov245_UnregisterDrawList;
    *(Callback *)(self + 0x2c) = Ov245_RegisterDrawList;
    *(Callback *)(self + 0x20) = Ov245_SendBlankStatus;
    *(Callback *)(self + 0x24) = Ov245_PublishCountersToMessage;
    *(Callback *)(self + 0x1d0) = Ov245_HitFilterStock;
    *(Callback *)(self + 0x1dc) = Ov245_BindMotion;
    *(unsigned char *)(self + 0x1c9) = 2;
    *(int *)(self + 0x70) = 0x4c00;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x4c00;
    *(int *)(self + 0x6c) = 0;
    {
        u16 hw = *(u16 *)(self + 0x60);
        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x20) << 0x18) >> 0x10);
    }
    *(u16 *)(self + 0x100 + 0xae) |= 0x18;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    *(Callback *)(*(int *)(self + 0x384) + 0x74) = Ov245_WingPose;
    *(int *)(*(int *)(self + 0x384) + 0x84) = self;
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    Snd_RegisterSeqAndBind((void *)(self + 0x390), ((struct Ov245Track *)*(int *)(self + 0x384))->track,
                  Ov107_PackTextureHandle(self, 1), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x384), (void *)(self + 0x390));
    *(int *)(self + 0x448) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov245_020d71ec);
    *(int *)(self + 0x44c) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov245_020d71f4);
    *(int *)(self + 0x450) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov245_020d7200);
    *(int *)(self + 0x454) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov245_020d720c);
    *(int *)(self + 0x388) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0xb));
    *(int *)(self + 0x38c) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0xc));
    for (i = 0; i < 2; i++) {
        RegisterSubscriberSlot(*(int *)(self + 0x9c), ((struct Ov245Boss *)self)->items[i]);
        ((struct Flags5c *)(((struct Ov245Boss *)self)->items[i] + 0x5c))->bit0 = 1;
    }
    item = *(int *)(self + 0x4dc) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0x20));
    Ov107_EnqueueValue(self, item);
    *(int *)(item + 0x5c) |= 2;
    *(int *)(self + 0x4c8) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(self, 0xd), data_ov245_020d7218);
    *(int *)(self + 0x4cc) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(self, 0xe), data_ov245_020d7218);
    *(VecFx32 *)(self + 0x3bc) = data_02041dc8;
    *(VecFx32 *)(self + 0x3c8) = data_02042270;
    *(VecFx32 *)(self + 0x3d4) = data_02042264;
    *(VecFx32 *)(self + 0x3e0) = data_02042258;
    *(int *)(self + 0x3ec) = 0x8000;
    *(int *)(self + 0x3f0) = 0x4c00;
    *(int *)(self + 0x3f4) = 0x4c00;
    *(int **)(self + 0x3b4) = List_InsertSorted((void *)(self + 0x22c), 0x10, 0x64);
    **(int **)(self + 0x3b4) = Ov107_HitShape_NewBox((struct Ov245Query *)(self + 0x3bc));
    slot = List_InsertSorted((void *)(self + 0x144), 4, 0x64);
    *(int *)(self + 0x3b8) = *slot = Ov107_HitShape_NewBox((struct Ov245Query *)(self + 0x3bc));
    Ov107_LoadSpawnRecord(4, (void *)(self + 0x58 + 0x400));
    for (i = 0; i < 9; i++) {
        ((struct Ov245Boss *)self)->slots[i] = Ov245_Projectile_New(self);
    }
    for (i = 0; i < 3; i++) {
        ((struct Ov245Boss *)self)->parts[i] = Ov245_Carrier_New(self, i);
    }
    for (i = 0; i < 3; i++) {
        ((struct Ov245Boss *)self)->riders[i] = Ov245_Rider_New(self, (void *)(self + 0x58 + 0x400));
    }
    *(int *)(self + 0x42c) = Ov245_Thrown_New(self);
    *(int *)(self + 0x430) = Ov245_Variant_New(self);
    *(int *)(self + 0x434) = Ov245_FourShape_New(self);
    *(int *)(self + 0x438) = Ov245_Mounted_New(self);
    Res_RequestIdPair(0x15a);
}

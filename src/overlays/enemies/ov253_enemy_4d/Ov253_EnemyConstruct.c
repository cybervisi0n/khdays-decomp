/* Constructor of the ov253 enemy. Installs the handlers (+8, +0xc, +0x28, +0x2c, +0x20, +0x1c message,
 * +0x34, +0x30, +0x1d0 hit, +0x1dc, +0x1ec), the +0x1c9 kind (2), the +0x64 pose (scale 4.0), bit 5 of
 * the +0x60 high byte and bits 3-4 of +0x1ae. Builds the +0x384 body rig (pose 0, owned, callback
 * 020cc090) with two bones (+0x394, +0x398) and its animation block (+0x388, pose 1, 12 frames), the
 * +0x38c arm rig (pose 0xe, owned, callback 020cc248, hidden flag) with its +0x43c node, animation block
 * (+0x390, pose 0xf) and three bones (+0x39c..+0x3a4). Four groups of five body bones (name tables
 * data_ov253_020d486c / 4844 / 4880 / 4858 into +0x3ac, +0x3d0, +0x3f4, +0x418) each get three capsules
 * (origin, world x, scale 1.0, range 0.81) on the +0x144 list (+0x3c0, +0x3e4, +0x408, +0x42c); a tall
 * capsule 2.06 up (radius 4.94) is registered on the +0x144 (+0x440) and +0x22c (+0x444) lists. The
 * +0x3a8 effect (pose 0x27, callback 020cbfc8) is attached and hidden, the +0x464 and +0x460 helpers
 * are created, sound bank 0x69 is set up at +0x46c, four +0x458 orbiters (indexed), eight +0x45c shards
 * and the two +0x468 slot models (kinds of data_ov253_020d482c) are built, and sound 0x16b loads.
 * Codegen: the first name table is copied after the capsule request is complete, and the +0x60
 * halfword is read through an explicit int conversion. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { int id[2]; } IdTable;
typedef struct { const char *name[5]; } NameTable;
typedef struct { VecFx32 vA; VecFx32 vB; int nScale; int nRange; } PlaceReq;
typedef struct { int pItem; int pad; } SubitemSlot;
struct Bit0 { unsigned int b0 : 1; };

extern void Ov253_ReleaseNodeResources(void);
extern void Ov253_ApplyFacing(void);
extern void Ov253_DrawListRemove(void);
extern void Ov253_DrawListRegister(void);
extern void Ov253_SendHandleMsg(void);
extern void Ov253_MsgHook(void);
extern void func_ov253_020cccf4(void);
extern void Ov253_CreateRegistryEntryAndLink(void);
extern void Ov253_HitFilterItem(void);
extern void Ov253_RebuildCarriedLists(void);
extern void Ov253_NotifyPartsThenBase(void);
extern void Ov253_ChainUpdate(void);
extern void Ov253_CarriedItemUpdate(void);
extern void Ov253_DrawRingMarkEmpty(void);
extern void *Ov107_PackTextureHandle(char *self, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern int InsertSortedEntryWithKey(int item, int kind, const char *name);
extern int *CallocInstance(int size);
extern void Snd_RegisterSeqAndBind(void *dst, int a, void *b, int n);
extern int FindResourceIndexByName(int item, const char *name);
extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern int Ov107_Mover_New(PlaceReq *req);
extern int JointModel_New(void *item, int size);
extern void Ov107_EnqueueValue(char *self, int item);
extern int Ov253_CreateRingTask(char *self);
extern int Ov253_QueueActor_New(char *self);
extern void Ov107_LoadSpawnRecord(int bank, void *out);
extern int Ov253_Segment_New(char *self, void *bank);
extern int Ov253_Item_New(char *self);
extern void Res_RequestIdPair(int resourceId);
extern const IdTable data_ov253_020d482c;
extern const char data_ov253_020d4b64[];
extern const char data_ov253_020d4b6c[];
extern const char data_ov253_020d4b74[];
extern const char data_ov253_020d4b80[];
extern const char data_ov253_020d4b88[];
extern const char data_ov253_020d4b94[];
extern const NameTable data_ov253_020d486c;
extern const NameTable data_ov253_020d4844;
extern const NameTable data_ov253_020d4880;
extern const NameTable data_ov253_020d4858;
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042240;

void Ov253_EnemyConstruct(char *self)
{
    PlaceReq req;
    NameTable names1;
    NameTable names2;
    NameTable names3;
    NameTable names4;
    int i;
    IdTable ids = data_ov253_020d482c;
    int *p;
    PlaceReq tall;
    u16 hw;

    *(Callback *)(self + 0x8) = Ov253_ReleaseNodeResources;
    *(Callback *)(self + 0xc) = Ov253_ApplyFacing;
    *(Callback *)(self + 0x28) = Ov253_DrawListRemove;
    *(Callback *)(self + 0x2c) = Ov253_DrawListRegister;
    *(Callback *)(self + 0x20) = Ov253_SendHandleMsg;
    *(Callback *)(self + 0x1c) = Ov253_MsgHook;
    *(Callback *)(self + 0x34) = func_ov253_020cccf4;
    *(Callback *)(self + 0x30) = Ov253_CreateRegistryEntryAndLink;
    *(Callback *)(self + 0x1d0) = Ov253_HitFilterItem;
    *(Callback *)(self + 0x1dc) = Ov253_RebuildCarriedLists;
    *(signed char *)(self + 0x1c9) = 2;
    *(Callback *)(self + 0x1ec) = Ov253_NotifyPartsThenBase;
    *(int *)(self + 0x70) = 0x4000;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x4000;
    *(int *)(self + 0x6c) = 0;
    hw = (int)*(u16 *)(self + 0x60);
    *(u16 *)(self + 0x60) = (hw & ~0xff00) |
        ((((((unsigned int)hw << 0x10) >> 0x18) | 0x20) << 0x18) >> 0x10);
    *(u16 *)(self + 0x100 + 0xae) |= 0x18;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    *(Callback *)(*(int *)(self + 0x384) + 0x74) = Ov253_ChainUpdate;
    *(char **)(*(int *)(self + 0x384) + 0x84) = self;
    *(int *)(self + 0x394) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov253_020d4b64);
    *(int *)(self + 0x398) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov253_020d4b6c);
    *(int **)(self + 0x388) = CallocInstance(0x24);
    Snd_RegisterSeqAndBind(*(void **)(self + 0x388), *(int *)(*(int *)(self + 0x384) + 0x88), Ov107_PackTextureHandle(self, 1), 0xc);
    *(int *)(self + 0x38c) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0xe));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x38c));
    *(Callback *)(*(int *)(self + 0x38c) + 0x6c) = Ov253_CarriedItemUpdate;
    *(char **)(*(int *)(self + 0x38c) + 0x84) = self;
    ((struct Bit0 *)(*(int *)(self + 0x38c) + 0x5c))->b0 = 1;
    *(int *)(self + 0x43c) = FindResourceIndexByName(*(int *)(self + 0x38c), data_ov253_020d4b74);
    *(int **)(self + 0x390) = CallocInstance(0x24);
    Snd_RegisterSeqAndBind(*(void **)(self + 0x390), *(int *)(*(int *)(self + 0x38c) + 0x88), Ov107_PackTextureHandle(self, 0xf), 0xc);
    *(int *)(self + 0x39c) = InsertSortedEntryWithKey(*(int *)(self + 0x38c), 3, data_ov253_020d4b80);
    *(int *)(self + 0x3a0) = InsertSortedEntryWithKey(*(int *)(self + 0x38c), 1, data_ov253_020d4b88);
    *(int *)(self + 0x3a4) = InsertSortedEntryWithKey(*(int *)(self + 0x38c), 1, data_ov253_020d4b94);
    req.vA = data_02041dc8;
    req.vB = data_02042270;
    req.nScale = 0x1000;
    req.nRange = 0xd00;
    names1 = data_ov253_020d486c;
    for (i = 0; i < 5; i++) {
        ((int *)(self + 0x3ac))[i] = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, names1.name[i]);
    }
    for (i = 0; i < 3; i++) {
        p = List_InsertSorted(self + 0x144, 4, 100);
        ((int *)(self + 0x3c0))[i] = *p = Ov107_Mover_New(&req);
    }
    names2 = data_ov253_020d4844;
    for (i = 0; i < 5; i++) {
        ((int *)(self + 0x3d0))[i] = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, names2.name[i]);
    }
    for (i = 0; i < 3; i++) {
        p = List_InsertSorted(self + 0x144, 4, 100);
        ((int *)(self + 0x3e4))[i] = *p = Ov107_Mover_New(&req);
    }
    names3 = data_ov253_020d4880;
    for (i = 0; i < 5; i++) {
        ((int *)(self + 0x3f4))[i] = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, names3.name[i]);
    }
    for (i = 0; i < 3; i++) {
        p = List_InsertSorted(self + 0x144, 4, 100);
        ((int *)(self + 0x408))[i] = *p = Ov107_Mover_New(&req);
    }
    names4 = data_ov253_020d4858;
    for (i = 0; i < 5; i++) {
        ((int *)(self + 0x418))[i] = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, names4.name[i]);
    }
    for (i = 0; i < 3; i++) {
        p = List_InsertSorted(self + 0x144, 4, 100);
        ((int *)(self + 0x42c))[i] = *p = Ov107_Mover_New(&req);
    }
    tall.vA.x = 0;
    tall.vA.y = 0x2100;
    tall.vA.z = 0;
    tall.vB = data_02042240;
    tall.nScale = 0x4f00;
    tall.nRange = 0x2100;
    p = List_InsertSorted(self + 0x144, 4, 100);
    *(int *)(self + 0x440) = *p = Ov107_Mover_New(&tall);
    *(int **)(self + 0x444) = List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x444) = Ov107_Mover_New(&tall);
    *(int *)(self + 0x3a8) = JointModel_New(Ov107_PackTextureHandle(self, 0x27), 0xc);
    Ov107_EnqueueValue(self, *(int *)(self + 0x3a8));
    *(int *)(self + 0x464) = Ov253_CreateRingTask(self);
    *(Callback *)(*(int *)(self + 0x3a8) + 0x6c) = Ov253_DrawRingMarkEmpty;
    *(char **)(*(int *)(self + 0x3a8) + 0x84) = self;
    *(int *)(*(int *)(self + 0x3a8) + 0x5c) |= 2;
    *(int *)(self + 0x460) = Ov253_QueueActor_New(self);
    Ov107_LoadSpawnRecord(0x69, self + 0x46c);
    *(int **)(self + 0x458) = CallocInstance(0x10);
    for (i = 0; i < 4; i++) {
        (*(int **)(self + 0x458))[i] = Ov253_Segment_New(self, self + 0x46c);
        *(int *)((*(int **)(self + 0x458))[i] + 0x388) = i;
    }
    *(int **)(self + 0x45c) = CallocInstance(0x20);
    for (i = 0; i < 8; i++) {
        (*(int **)(self + 0x45c))[i] = Ov253_Item_New(self);
    }
    *(int **)(self + 0x468) = CallocInstance(0x10);
    for (i = 0; i < 2; i++) {
        (*(SubitemSlot **)(self + 0x468))[i].pItem = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ids.id[i]));
        Ov107_EnqueueValue(self, (*(SubitemSlot **)(self + 0x468))[i].pItem);
        *(int *)((*(SubitemSlot **)(self + 0x468))[i].pItem + 0x5c) |= 2;
    }
    Res_RequestIdPair(0x16b);
}

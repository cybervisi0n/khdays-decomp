/* Constructor of the ov175 enemy (twins by byte identity). Installs the handlers (+8 release, +0xc
 * draw veneer, +0x1c spawn message, +0x28/+0x2c/+0x30 callbacks, +0x34 tick, +0x1d0 hit,
 * +0x1dc/+0x1e0 finish), sets bit 6 of the +0x60 high byte, the +0x70 latch (0xc00), zeroes
 * the +0x64 camera vector, writes the {-1,0,-1,1,1,1} bounds at +0x1fc, builds the model item
 * from table entry 0 (subscribed, translated 0xc00 down, its +0x74 callback and +0x84 owner
 * set, +0xad cleared) and resolves the "Bone04"/"body" handles into +0x38c/+0x390/+0x394; then
 * allocates the 3-entry effect set at +0x39c from the table's three ids (each attached and
 * flagged), registers the four action slots (0/2/1/4) with 0x3000, creates the two pools at
 * +0x388/+0x398 seeded with the camera key, allocates the +0x3ac child and requests resource
 * 0x142. Codegen: the item stores are spelled `*(Callback *)(*(int *)(self + 0x384) + 0x74)`
 * on the `char *self` parameter (the pool load of the value is then emitted before the item
 * load and the shared zero sits in r4); the bounds block sits before the +0x60 update. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int a, b, c; } Vec3b;
typedef struct { VecFx32 vector; int scalar; } CameraWork;
typedef struct { int w[6]; } Bounds;
typedef struct { int id[3]; } IdTable;
typedef struct { int subitem; int pad; } Slot;
typedef void (*Callback)(void);

extern void Ov177_Actor_DestroyWithParts(void);
extern void func_ov177_020d3bfc(void);
extern void Ov177_HandleSpawnMessage(void);
extern void Ov177_registryCreateEntry(void);
extern void Ov177_ForwardEventToChild(void);
extern void Ov177_NotifyPartThenBase(void);
extern void Ov177_ReleaseByStateAndSyncSrt(void);
extern void Ov177_OnHit(void);
extern void Ov177_Model_SetTrack0(void);
extern void Ov177_RequestSubState14IfNotCurrent(void);
extern void Ov177_OrientPartAlongDirection(void);
extern void *Ov107_PackTextureHandle(char *self, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void Srt_SetTranslationXYZ(void *srt, int x, int y, int z);
extern int InsertSortedEntryWithKey(int item, int kind, void *name);
extern int *CallocInstance(int size);
extern void Ov107_EnqueueValue(char *self, int item);
extern void Ov107_Actor_SetAttachSlot(char *self, int slot, int a, int b, int c);
extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern int Ov107_CloneResourceTransform(void *camera);
extern void Res_RequestIdPair(int resourceId);
extern int *Ov177_Actor_New(char *self);
extern VecFx32 data_02041dc8;
extern IdTable data_ov177_020d62f8;
extern char data_ov177_020d632c[];
extern char data_ov177_020d6334[];

void Ov177_Construct(char *self)
{
    IdTable ids = data_ov177_020d62f8;
    Bounds bounds;
    CameraWork work;
    VecFx32 v;
    u16 hw;
    int i;
    int *slot;

    *(Callback *)(self + 0x8) = Ov177_Actor_DestroyWithParts;
    *(Callback *)(self + 0xc) = func_ov177_020d3bfc;
    *(Callback *)(self + 0x1c) = Ov177_HandleSpawnMessage;
    *(Callback *)(self + 0x30) = Ov177_registryCreateEntry;
    *(Callback *)(self + 0x28) = Ov177_ForwardEventToChild;
    *(Callback *)(self + 0x2c) = Ov177_NotifyPartThenBase;
    *(Callback *)(self + 0x34) = Ov177_ReleaseByStateAndSyncSrt;
    *(Callback *)(self + 0x1d0) = Ov177_OnHit;
    *(Callback *)(self + 0x1dc) = Ov177_Model_SetTrack0;
    *(Callback *)(self + 0x1e0) = Ov177_RequestSubState14IfNotCurrent;
    bounds.w[0] = -0x1000;
    bounds.w[2] = -0x1000;
    bounds.w[1] = 0;
    bounds.w[3] = 0x1000;
    bounds.w[4] = 0x1000;
    bounds.w[5] = 0x1000;
    hw = *(u16 *)(self + 0x60);
    *(u16 *)(self + 0x60) = (hw & ~0xff00) |
        ((((((unsigned int)hw << 0x10) >> 0x18) | 0x40) << 0x18) >> 0x10);
    *(int *)(self + 0x70) = 0xc00;
    v = data_02041dc8;
    *(Vec3b *)(self + 0x64) = *(Vec3b *)&data_02041dc8;
    *(Bounds *)(self + 0x1fc) = bounds;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    Srt_SetTranslationXYZ((void *)(*(int *)(self + 0x384) + 4), 0, -0xc00, 0);
    *(Callback *)(*(int *)(self + 0x384) + 0x74) = Ov177_OrientPartAlongDirection;
    *(char **)(*(int *)(self + 0x384) + 0x84) = self;
    *(unsigned char *)(*(int *)(self + 0x384) + 0xad) = 0;
    *(int *)(self + 0x38c) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov177_020d632c);
    *(int *)(self + 0x390) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov177_020d632c);
    *(int *)(self + 0x394) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov177_020d6334);
    *(int **)(self + 0x39c) = CallocInstance(0x18);
    for (i = 0; i < 3; i++) {
        if (i < 0) {
            (*(Slot **)(self + 0x39c))[i].subitem = CreateSubitemInstance0xB4((void *)ids.id[i]);
        } else {
            (*(Slot **)(self + 0x39c))[i].subitem = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ids.id[i]));
        }
        Ov107_EnqueueValue(self, (*(Slot **)(self + 0x39c))[i].subitem);
        *(int *)((*(Slot **)(self + 0x39c))[i].subitem + 0x5c) |= 2;
    }
    Ov107_Actor_SetAttachSlot(self, 0, 1, 0, 0x3000);
    Ov107_Actor_SetAttachSlot(self, 2, 1, 0, 0x3000);
    Ov107_Actor_SetAttachSlot(self, 1, 1, 0, 0x3000);
    Ov107_Actor_SetAttachSlot(self, 4, 1, 0, 0x3000);
    work = *(CameraWork *)(self + 0x64);
    work.vector = v;
    *(int **)(self + 0x388) = List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x388) = Ov107_CloneResourceTransform(&work);
    slot = List_InsertSorted(self + 0x144, 4, 100);
    *(int *)(self + 0x398) = *slot = Ov107_CloneResourceTransform(&work);
    *(int **)(self + 0x3ac) = Ov177_Actor_New(self);
    Res_RequestIdPair(0x142);
}

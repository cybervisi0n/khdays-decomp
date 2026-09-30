/* Constructor of the ov115 enemy (byte-identical to ov116's). Installs the handlers (+8, +0xc,
 * +0x1c, +0x28/+0x2c/+0x30, +0x34 tick, +0x1d0 hit, +0x1dc/+0x1e0), sets bit 6 of the +0x60 high
 * byte, the +0x70 scale (0x600), zeroes the +0x64 pose, writes the {-1,0,-1,1,1,1} bounds at
 * +0x1fc, builds the model item from table entry 0 (subscribed, translated 0x600 down, +0x74
 * callback, +0x84 owner, +0xad cleared) and resolves three bone handles into +0x38c/+0x390/+0x394;
 * allocates the 7-entry effect set at +0x39c from the id table (each attached and flagged),
 * registers the four action slots (0/2/1/4) at 0x1800, creates the two pools at +0x388/+0x398
 * seeded with the pose key and requests resource 0x114. Codegen: the shared zero vector
 * data_02041dc8 is const -- declared non-const, its loads may alias the +0x70 store, which then
 * stays behind the v copy and swaps the registers of the whole handler block. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int a, b, c; } Vec3b;
typedef struct { VecFx32 vector; int scalar; } CameraWork;
typedef struct { int w[6]; } Bounds;
typedef struct { int id[7]; } IdTable;
typedef struct { int subitem; int pad; } Slot;
typedef void (*Callback)(void);

extern void Ov115_ReleaseSubObjectsListThenNotify(void);
extern void func_ov115_020cc374(void);
extern void Ov115_HandleSpawnMessage(void);
extern void Ov115_registryCreateEntry(void);
extern void func_ov115_020cc380(void);
extern void func_ov115_020cc38c(void);
extern void Ov115_ReleaseTasks(void);
extern void Ov115_OnHit(void);
extern void Ov115_Model_SetTrack0(void);
extern void Ov115_RequestSubState14IfNotCurrent(void);
extern void Ov115_OrientPartAlongDirection(void);
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
extern const VecFx32 data_02041dc8;
extern IdTable data_ov115_020ceb38;
extern char data_ov115_020ceb8c[];
extern char data_ov115_020ceb94[];

void Ov115_EnemyConstruct(char *self)
{
    IdTable ids = data_ov115_020ceb38;
    Bounds bounds;
    CameraWork work;
    VecFx32 v;
    u16 hw;
    int i;
    int *slot;

    bounds.w[0] = -0x1000;
    bounds.w[2] = -0x1000;
    bounds.w[1] = 0;
    bounds.w[3] = 0x1000;
    bounds.w[4] = 0x1000;
    bounds.w[5] = 0x1000;
    *(Callback *)(self + 0x8) = Ov115_ReleaseSubObjectsListThenNotify;
    *(Callback *)(self + 0xc) = func_ov115_020cc374;
    *(Callback *)(self + 0x1c) = Ov115_HandleSpawnMessage;
    *(Callback *)(self + 0x30) = Ov115_registryCreateEntry;
    *(Callback *)(self + 0x28) = func_ov115_020cc380;
    *(Callback *)(self + 0x2c) = func_ov115_020cc38c;
    *(Callback *)(self + 0x34) = Ov115_ReleaseTasks;
    *(Callback *)(self + 0x1d0) = Ov115_OnHit;
    *(Callback *)(self + 0x1dc) = Ov115_Model_SetTrack0;
    *(Callback *)(self + 0x1e0) = Ov115_RequestSubState14IfNotCurrent;
    hw = *(u16 *)(self + 0x60);
    *(u16 *)(self + 0x60) = (hw & ~0xff00) |
        ((((((unsigned int)hw << 0x10) >> 0x18) | 0x40) << 0x18) >> 0x10);
    *(int *)(self + 0x70) = 0x600;
    v = data_02041dc8;
    *(Vec3b *)(self + 0x64) = *(Vec3b *)&data_02041dc8;
    *(Bounds *)(self + 0x1fc) = bounds;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    Srt_SetTranslationXYZ((void *)(*(int *)(self + 0x384) + 4), 0, -0x600, 0);
    *(Callback *)(*(int *)(self + 0x384) + 0x74) = Ov115_OrientPartAlongDirection;
    *(char **)(*(int *)(self + 0x384) + 0x84) = self;
    *(unsigned char *)(*(int *)(self + 0x384) + 0xad) = 0;
    *(int *)(self + 0x38c) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov115_020ceb8c);
    *(int *)(self + 0x390) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov115_020ceb8c);
    *(int *)(self + 0x394) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov115_020ceb94);
    *(int **)(self + 0x39c) = CallocInstance(0x38);
    for (i = 0; i < 7; i++) {
        if (i < 0) {
            (*(Slot **)(self + 0x39c))[i].subitem = CreateSubitemInstance0xB4((void *)ids.id[i]);
        } else {
            (*(Slot **)(self + 0x39c))[i].subitem = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ids.id[i]));
        }
        Ov107_EnqueueValue(self, (*(Slot **)(self + 0x39c))[i].subitem);
        *(int *)((*(Slot **)(self + 0x39c))[i].subitem + 0x5c) |= 2;
    }
    Ov107_Actor_SetAttachSlot(self, 0, 1, 0, 0x1800);
    Ov107_Actor_SetAttachSlot(self, 2, 1, 0, 0x1800);
    Ov107_Actor_SetAttachSlot(self, 1, 1, 0, 0x1800);
    Ov107_Actor_SetAttachSlot(self, 4, 1, 0, 0x1800);
    work = *(CameraWork *)(self + 0x64);
    work.vector = v;
    *(int **)(self + 0x388) = List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x388) = Ov107_CloneResourceTransform(&work);
    slot = List_InsertSorted(self + 0x144, 4, 100);
    *(int *)(self + 0x398) = *slot = Ov107_CloneResourceTransform(&work);
    Res_RequestIdPair(0x114);
}

/* Setup of the ov253 enemy: installs the handlers (+8, +0xc, +0x1c message, +0x34, +0x30, +0x1d0 hit,
 * +0x1dc), sets the +0x1c9 kind (2), the +0x1b0 group (4), the +0x19c id (0x69), +0x54 = 0 and
 * +0x58 = 0.5, the +0x64 pose (scale 2.0), bits 4-5 of the +0x60 high byte and bits 3-4 of +0x1ae.
 * The +0x38c rig (item 0x1d of the +0x384 pool, owned, callback 020ce478) is subscribed to +0x9c and
 * its +0x3ac bone becomes the +0x2cc anchor (+0x14); four more bones (names of data_ov253_020d4940)
 * fill +0x390..+0x39c, three capsules (origin, world x, scale 1.0, range 0.19) the +0x3a0 handles on the
 * +0x144 list; the +0x3b8 effect (item 0x2c, callback 020ce3f8) is attached and hidden; the five
 * +0x3b0 slot models (kinds of data_ov253_020d4950) are attached and hidden, the second one also
 * flagged; a placement at the origin (scale 1.0) fills +0x3b4, +0x3c0 clears and sound 0x16c loads. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { int id[5]; } IdTable;
typedef struct { const char *name[4]; } NameTable;
typedef struct { VecFx32 vA; VecFx32 vB; int nScale; int nRange; } PlaceReq;
typedef struct { VecFx32 vec; int scale; } CameraWork;
typedef struct { int pItem; int pad; } SubitemSlot;
struct Bit0 { unsigned int b0 : 1; };

extern void Ov253_Segment_Destroy(void);
extern void Ov253_CopyBlockToLinkedNode(void);
extern void Ov253_MsgHookSlots(void);
extern void Ov253_Teardown(void);
extern void Ov253_CreateRegistryEntryAndLink_2(void);
extern void Ov253_HitFilterCore(void);
extern void Ov253_Model_SetTrack0(void);
extern void Ov253_UpdateSegmentDirs(void);
extern void Ov253_RenderMarkers(void);
extern void *Ov107_PackTextureHandle(int pool, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern int InsertSortedEntryWithKey(int item, int kind, const char *name);
extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern int Ov107_Mover_New(PlaceReq *req);
extern int JointModel_New(void *item, int size);
extern void Ov107_EnqueueValue(char *self, int item);
extern int *CallocInstance(int size);
extern int Ov107_CloneResourceTransform(void *camera);
extern void Res_RequestIdPair(int resourceId);
extern const IdTable data_ov253_020d4950;
extern const NameTable data_ov253_020d4940;
extern const char data_ov253_020d4bc0[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042270;

void Ov253_Setup(char *self)
{
    IdTable ids = data_ov253_020d4950;
    CameraWork work;
    PlaceReq req;
    int *p;
    int i;
    u16 hw;

    *(Callback *)(self + 0x8) = Ov253_Segment_Destroy;
    *(Callback *)(self + 0xc) = Ov253_CopyBlockToLinkedNode;
    *(Callback *)(self + 0x1c) = Ov253_MsgHookSlots;
    *(Callback *)(self + 0x34) = Ov253_Teardown;
    *(Callback *)(self + 0x30) = Ov253_CreateRegistryEntryAndLink_2;
    *(Callback *)(self + 0x1d0) = Ov253_HitFilterCore;
    *(Callback *)(self + 0x1dc) = Ov253_Model_SetTrack0;
    *(signed char *)(self + 0x1c9) = 2;
    *(u16 *)(self + 0x100 + 0xb0) = 4;
    *(unsigned char *)(self + 0x19c) = 0x69;
    *(int *)(self + 0x54) = 0;
    *(int *)(self + 0x58) = 0x800;
    *(int *)(self + 0x70) = 0x2000;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x2000;
    *(int *)(self + 0x6c) = 0;
    hw = *(u16 *)(self + 0x60);
    *(u16 *)(self + 0x60) = (hw & ~0xff00) |
        ((((((unsigned int)hw << 0x10) >> 0x18) | 0x30) << 0x18) >> 0x10);
    *(u16 *)(self + 0x100 + 0xae) |= 0x18;
    *(int *)(self + 0x38c) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(*(int *)(self + 0x384), 0x1d));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x38c));
    *(Callback *)(*(int *)(self + 0x38c) + 0x74) = Ov253_UpdateSegmentDirs;
    *(char **)(*(int *)(self + 0x38c) + 0x84) = self;
    *(int *)(self + 0x2cc) = (*(int *)(self + 0x3ac) = InsertSortedEntryWithKey(*(int *)(self + 0x38c), 3, data_ov253_020d4bc0)) + 0x14;
    NameTable names;
    NameTable *pNames = &names;
    *pNames = data_ov253_020d4940;
    VecFx32 v = data_02041dc8;
    req.vA = data_02041dc8;
    req.vB = data_02042270;
    req.nScale = 0x1000;
    req.nRange = 0x300;
    for (i = 0; i < 4; i++) {
        ((int *)(self + 0x390))[i] = InsertSortedEntryWithKey(*(int *)(self + 0x38c), 1, pNames->name[i]);
    }
    for (i = 0; i < 3; i++) {
        p = List_InsertSorted(self + 0x144, 4, 100);
        ((int *)(self + 0x3a0))[i] = *p = Ov107_Mover_New(&req);
    }
    *(int *)(self + 0x3b8) = JointModel_New(Ov107_PackTextureHandle(*(int *)(self + 0x384), 0x2c), 0x10);
    Ov107_EnqueueValue(self, *(int *)(self + 0x3b8));
    *(Callback *)(*(int *)(self + 0x3b8) + 0x6c) = Ov253_RenderMarkers;
    *(int *)(*(int *)(self + 0x3b8) + 0x5c) |= 2;
    *(int **)(self + 0x3b0) = CallocInstance(0x28);
    for (i = 0; i < 5; i++) {
        (*(SubitemSlot **)(self + 0x3b0))[i].pItem = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(*(int *)(self + 0x384), ids.id[i]));
        Ov107_EnqueueValue(self, (*(SubitemSlot **)(self + 0x3b0))[i].pItem);
        *(int *)((*(SubitemSlot **)(self + 0x3b0))[i].pItem + 0x5c) |= 2;
    }
    ((struct Bit0 *)((*(SubitemSlot **)(self + 0x3b0))[1].pItem + 0x5c))->b0 = 1;
    work.vec = v;
    work.scale = 0x1000;
    *(int **)(self + 0x3b4) = List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x3b4) = Ov107_CloneResourceTransform(&work);
    *(int *)(self + 0x3c0) = 0;
    Res_RequestIdPair(0x16c);
}

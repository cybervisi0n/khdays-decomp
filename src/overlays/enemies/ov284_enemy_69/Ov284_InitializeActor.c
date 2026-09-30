/* Constructor of the ov284 enemy: raises bit 8 of the +0 flag halfword, installs the handlers
 * (+8 tick, +0xc draw, +0x1c message, +0x34/+0x30 callbacks, +0x1d0 hit, +0x1e0/+0x1dc
 * finish), seeds the +0x64 pose (scale 0x2000, y 0x2000), bits 4/5 of the +0x60 high byte and
 * bit 4 of +0x1ae; builds the primary item from pool entry 0 (+0x384, subscribed, +0x74
 * callback 020cc044, +0x84 owner), resolves the kind-3 joint into +0x3a4 (its +0x14 position
 * kept at +0x2cc) and the four kind-1 joints of the 0x020cd5a4 names into +0x388..; creates three
 * placements on the +0x144 list (+0x398..) from a zero/zero/0x02042270 request with scale
 * 0x1000 and 0x300, a 16-byte effect from pool entry 5 (+0x3ac, attached, +0x6c callback
 * 020cbfc4, bit 1), a 32-byte slot table (+0x3b0) holding the four sub-items of the 0x020cd594
 * ids (attached, bit 1), a placement on the +0x22c list (+0x3a8) from the zero pose at scale
 * 1.0, then loads sound 0x16c. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { int id[4]; } IdTable;
typedef struct { const char *name[4]; } NameTable;
typedef struct { VecFx32 vA; VecFx32 vB; int nScale; int nRange; } PlaceReq;
typedef struct { VecFx32 vec; int scale; } CameraWork;
typedef struct { int pItem; int pad; } SubitemSlot;

extern void Ov284_Destroy(void);
extern void Ov284_CopyBlockToLinkedNode(void);
extern void Ov284_HandleMessage(void);
extern void Ov284_PostTickCancelStaleTasks(void);
extern void Ov284_ForwardAnimEvent(void);
extern void Ov284_OnHitStaggerFlip(void);
extern void Ov284_ArbitrateSubStateEntry(void);
extern void Ov284_CreateRegistryEntryAndLink(void);
extern void Ov284_UpdateSegmentDirections(void);
extern void Ov284_RenderMarkers(void);
extern void *Ov107_PackTextureHandle(char *self, int index);
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
extern const IdTable data_ov284_020cd594;
extern const NameTable data_ov284_020cd5a4;
extern const char data_ov284_020cd60c[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042270;

void Ov284_InitializeActor(char *self)
{
    IdTable ids = data_ov284_020cd594;
    CameraWork work;
    PlaceReq req;
    int *p;
    int i;
    u16 hw;

    *(u16 *)self |= 0x100;
    *(Callback *)(self + 0x8) = Ov284_Destroy;
    *(Callback *)(self + 0xc) = Ov284_CopyBlockToLinkedNode;
    *(Callback *)(self + 0x1c) = Ov284_HandleMessage;
    *(Callback *)(self + 0x34) = Ov284_PostTickCancelStaleTasks;
    *(Callback *)(self + 0x30) = Ov284_CreateRegistryEntryAndLink;
    *(Callback *)(self + 0x1d0) = Ov284_OnHitStaggerFlip;
    *(Callback *)(self + 0x1e0) = Ov284_ArbitrateSubStateEntry;
    *(Callback *)(self + 0x1dc) = Ov284_ForwardAnimEvent;
    *(int *)(self + 0x70) = 0x2000;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x2000;
    *(int *)(self + 0x6c) = 0;
    hw = *(u16 *)(self + 0x60);
    *(u16 *)(self + 0x60) = (hw & ~0xff00) |
        ((((((unsigned int)hw << 0x10) >> 0x18) | 0x30) << 0x18) >> 0x10);
    *(u16 *)(self + 0x100 + 0xae) |= 0x10;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    *(Callback *)(*(int *)(self + 0x384) + 0x74) = Ov284_UpdateSegmentDirections;
    *(char **)(*(int *)(self + 0x384) + 0x84) = self;
    *(int *)(self + 0x3a4) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov284_020cd60c);
    *(int *)(self + 0x2cc) = *(int *)(self + 0x3a4) + 0x14;
    NameTable names;
    NameTable *pNames = &names;
    *pNames = data_ov284_020cd5a4;
    VecFx32 v = data_02041dc8;
    req.vA = data_02041dc8;
    req.vB = data_02042270;
    req.nScale = 0x1000;
    req.nRange = 0x300;
    for (i = 0; i < 4; i++) {
        ((int *)(self + 0x388))[i] = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, pNames->name[i]);
    }
    for (i = 0; i < 3; i++) {
        p = List_InsertSorted(self + 0x144, 4, 100);
        ((int *)(self + 0x398))[i] = *p = Ov107_Mover_New(&req);
    }
    *(int *)(self + 0x3ac) = JointModel_New(Ov107_PackTextureHandle(self, 5), 0x10);
    Ov107_EnqueueValue(self, *(int *)(self + 0x3ac));
    *(Callback *)(*(int *)(self + 0x3ac) + 0x6c) = Ov284_RenderMarkers;
    *(int *)(*(int *)(self + 0x3ac) + 0x5c) |= 2;
    *(int **)(self + 0x3b0) = CallocInstance(0x20);
    for (i = 0; i < 4; i++) {
        (*(SubitemSlot **)(self + 0x3b0))[i].pItem = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ids.id[i]));
        Ov107_EnqueueValue(self, (*(SubitemSlot **)(self + 0x3b0))[i].pItem);
        *(int *)((*(SubitemSlot **)(self + 0x3b0))[i].pItem + 0x5c) |= 2;
    }
    work.vec = v;
    work.scale = 0x1000;
    *(int **)(self + 0x3a8) = List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x3a8) = Ov107_CloneResourceTransform(&work);
    Res_RequestIdPair(0x16c);
}

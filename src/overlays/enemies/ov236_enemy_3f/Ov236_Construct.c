/* Constructor of the ov236 enemy (x2 with ov278). Installs the handlers (+8, +0xc, +0x1c message,
 * +0x28, +0x2c, +0x30 update, +0x1d0 hit filter, +0x1dc move player, +0x1ec, +0x1f4), sets the
 * +0x1c9 kind to 2, the +0x64 pose (scale 2.75), bits 3-4 of +0x1ae and bits 5 and 7 of the +0x60
 * high byte; builds the +0x384 rig from pose 0 (subscribed to +0x9c, owned by the enemy with
 * callback 020cbfc4) with its +0x38c work list and two bone attachments (+0x39c, +0x3a0), and the
 * +0x388 mount rig from pose 0x3c with its +0x390 work list (scaled 1/1/1 with flag 1, raised
 * 1/8, list rate 8); resolves the +0x3ac marker, builds the ten sub-items of data_ov236_020d6290
 * into the +0x3b0 pair table (all hidden; the sixth owned by the enemy with callback 020cc110),
 * creates the two rider controllers (+0x3b4 / +0x3b8, 020cce20 / 020cdc84) sharing the +0x19c
 * team byte, and reserves the +0x144 / +0x22c shape handles: a capsule (0.5 below, length 1.0,
 * radius 0.45) at +0x394 and +0x3a4 (radius scaled by 1.25), and a placement (scale 0.75) at
 * +0x398 and +0x3a8 (scaled by 1.5). Loads sound 0x127. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { int id[10]; } IdTable;
typedef struct { VecFx32 pos; int scale; } Placement;
typedef struct { VecFx32 pos; VecFx32 axis; int length; int radius; } Capsule;
struct Pair { int res; int handle; };

extern void Ov236_Destroy(void);
extern void Ov236_UpdateShadowA(void);
extern void Ov236_RemoveFromDrawList(void);
extern void Ov236_VisitSubObjects(void);
extern void Ov236_HandleMessage(void);
extern void Ov236_SpawnActorRegistryEntry(void);
extern void Ov236_MountHitFilter(void);
extern void Ov236_PlayRiderMove(void);
extern void Ov236_ForwardHitToSubObjects(void);
extern void Ov236_SyncRiderHitPoints(void);
extern void Ov236_SetupModelPoses(void);
extern void Ov236_RenderAtOwnerModel(void);
extern void *Ov107_PackTextureHandle(char *self, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern int CallocInstance(int size);
extern void Snd_RegisterSeqAndBind(int a, int b, void *c, int d);
extern void MainBlob_ResetSlotRows(int a, int b);
extern int InsertSortedEntryWithKey(int item, int kind, void *name);
extern void Ov107_EnqueueValue(char *self, int item);
extern void Srt_SetScaleXYZ(int srt, int sx, int sy, int sz);
extern void Srt_SetTranslationXYZ(int srt, int x, int y, int z);
extern void NNS_G3dMdlSetMdlAlphaAll(int nList, int nValue);
extern int Ov107_CreateNamedResourceBinding(void *item, const char *name);
extern int Ov236_FrontRiders_New(char *self);
extern int Ov236_RearRiders_New(char *self);
extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern int Ov107_Mover_New(const Capsule *capsule);
extern int Ov107_CloneResourceTransform(const Placement *placement);
extern void Res_RequestIdPair(int resourceId);
extern IdTable data_ov236_020d6290;
extern char data_ov236_020d64ac[];
extern char data_ov236_020d64b4[];
extern const char data_ov236_020d64c0[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042264;

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov236_Construct(char *self)
{
    IdTable ids = data_ov236_020d6290;
    Capsule cap;
    Placement place;
    u16 hw;
    int i;
    int *slot;

    *(Callback *)(self + 0x8) = Ov236_Destroy;
    *(Callback *)(self + 0xc) = Ov236_UpdateShadowA;
    *(Callback *)(self + 0x28) = Ov236_RemoveFromDrawList;
    *(Callback *)(self + 0x2c) = Ov236_VisitSubObjects;
    *(Callback *)(self + 0x1c) = Ov236_HandleMessage;
    *(Callback *)(self + 0x30) = Ov236_SpawnActorRegistryEntry;
    *(Callback *)(self + 0x1d0) = Ov236_MountHitFilter;
    *(Callback *)(self + 0x1dc) = Ov236_PlayRiderMove;
    *(Callback *)(self + 0x1f4) = Ov236_ForwardHitToSubObjects;
    *(Callback *)(self + 0x1ec) = Ov236_SyncRiderHitPoints;
    *(unsigned char *)(self + 0x1c9) = 2;
    *(int *)(self + 0x70) = 0x2c00;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x2c00;
    *(int *)(self + 0x6c) = 0;
    *(u16 *)(self + 0x100 + 0xae) |= 0x18;
    hw = *(u16 *)(self + 0x60);
    *(u16 *)(self + 0x60) = (hw & ~0xff00) |
        ((((((unsigned int)hw << 0x10) >> 0x18) | 0x20) << 0x18) >> 0x10);
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    *(int *)(self + 0x38c) = CallocInstance(0x24);
    Snd_RegisterSeqAndBind(*(int *)(self + 0x38c), *(int *)(*(int *)(self + 0x384) + 0x88), Ov107_PackTextureHandle(self, 1), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x384), *(int *)(self + 0x38c));
    *(Callback *)(*(int *)(self + 0x384) + 0x74) = Ov236_SetupModelPoses;
    *(char **)(*(int *)(self + 0x384) + 0x84) = self;
    *(int *)(self + 0x39c) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov236_020d64ac);
    *(int *)(self + 0x3a0) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov236_020d64b4);
    hw = *(u16 *)(self + 0x60);
    *(u16 *)(self + 0x60) = (hw & ~0xff00) |
        ((((((unsigned int)hw << 0x10) >> 0x18) | 0x80) << 0x18) >> 0x10);
    *(int *)(self + 0x388) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0x3c));
    Ov107_EnqueueValue(self, *(int *)(self + 0x388));
    *(int *)(self + 0x390) = CallocInstance(0x24);
    Snd_RegisterSeqAndBind(*(int *)(self + 0x390), *(int *)(*(int *)(self + 0x388) + 0x88), Ov107_PackTextureHandle(self, 1), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x388), *(int *)(self + 0x390));
    Srt_SetScaleXYZ(*(int *)(self + 0x388) + 4, 0x1000, 1, 0x1000);
    Srt_SetTranslationXYZ(*(int *)(self + 0x388) + 4, 0, 0x200, 0);
    NNS_G3dMdlSetMdlAlphaAll(*(int *)(*(int *)(*(int *)(self + 0x388) + 0x88) + 0x78), 8);
    *(int *)(self + 0x3ac) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(self, 0x3f), data_ov236_020d64c0);
    *(int *)(self + 0x3b0) = CallocInstance(0x50);
    for (i = 0; i < 10; i++) {
        ((struct Pair *)*(int *)(self + 0x3b0))[i].res = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ids.id[i]));
        Ov107_EnqueueValue(self, ((struct Pair *)*(int *)(self + 0x3b0))[i].res);
        *(int *)(((struct Pair *)*(int *)(self + 0x3b0))[i].res + 0x5c) |= 2;
    }
    *(Callback *)(((struct Pair *)*(int *)(self + 0x3b0))[5].res + 0x6c) = Ov236_RenderAtOwnerModel;
    *(char **)(((struct Pair *)*(int *)(self + 0x3b0))[5].res + 0x84) = self;
    *(int *)(self + 0x3b4) = Ov236_FrontRiders_New(self);
    *(unsigned char *)(*(int *)(self + 0x3b4) + 0x19c) = *(unsigned char *)(self + 0x19c);
    *(int *)(self + 0x3b8) = Ov236_RearRiders_New(self);
    *(unsigned char *)(*(int *)(self + 0x3b8) + 0x19c) = *(unsigned char *)(self + 0x19c);
    *(int *)(self + 0x3c0) = 0;
    cap.pos.x = 0;
    cap.pos.y = -0x800;
    cap.pos.z = 0;
    cap.axis = data_02042264;
    cap.length = 0x1000;
    cap.radius = 0x732;
    slot = List_InsertSorted(self + 0x144, 4, 100);
    *(int *)(self + 0x394) = *slot = Ov107_Mover_New(&cap);
    place.pos = data_02041dc8;
    place.scale = 0xc00;
    slot = List_InsertSorted(self + 0x144, 4, 100);
    *(int *)(self + 0x398) = *slot = Ov107_CloneResourceTransform(&place);
    place.scale = FX_Mul(place.scale, 0x1800);
    *(int *)(self + 0x3a8) = (int)List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x3a8) = Ov107_CloneResourceTransform(&place);
    cap.radius = FX_Mul(cap.radius, 0x1400);
    *(int *)(self + 0x3a4) = (int)List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x3a4) = Ov107_Mover_New(&cap);
    Res_RequestIdPair(0x127);
}

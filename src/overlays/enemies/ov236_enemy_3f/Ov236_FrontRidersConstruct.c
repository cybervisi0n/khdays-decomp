/* Constructor of the front rider controller (created by 020cce20). Installs its handlers (+8,
 * +0xc, +0x10, +0x1c message, +0x20, +0x28, +0x30 update, +0x34, +0x1d0 hit filter, +0x1d4, +0x1dc
 * move player), sets kind 2, clears +0x54 / +0x58, the +0x64 pose (scale 2.75), bits 3-4 of +0x1ae,
 * bits 3, 7 and 11 of +0x1b0 and bit 5 of the +0x60 high byte. From the owner's +0x394 pool it
 * builds the +0x384 rider (pose 0x18, subscribed to +0x9c) with its +0x388 work list (pose 0x19)
 * and four bone attachments (+0x3a0..+0x3ac), and the +0x38c mount (pose 0x3d) with its +0x390 work
 * list (scaled 1/1/1 with flag 1, raised 1/8, list rate 8), then the five hidden sub-items of
 * data_ov236_020d6314 into the +0x3b8 pair table. Two capsules (length 1.5, radius 0.5, along the
 * x and z axes) are reserved at +0x398 / +0x39c on the +0x144 pool and, with radius scaled by 1.125
 * and flag bit 0 set, at +0x3b0 / +0x3b4 on the +0x22c pool. Both rider counters (+0x3bc / +0x3be)
 * start at 1, +0x3c0 bit 0 is set, the presence hook runs and both rigs re-init. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { int id[5]; } IdTable5;
typedef struct { VecFx32 pos; VecFx32 axis; int length; int radius; } Capsule;
struct Pair { int res; int handle; };
typedef struct { unsigned f : 8; } B8;
struct b1 { unsigned int b0 : 1; };

extern void Ov236_FrontRiders_Destroy(void);
extern void Ov236_UpdateShadowB(void);
extern void Ov236_InitModelPoses(void);
extern void Ov236_SendStatusWithPos(void);
extern void Ov236_HandleRiderMessage(void);
extern void Ov236_RidersA_CreateAiTask(void);
extern void Ov236_TeardownHook(void);
extern void Ov236_CopyPoseFromParent394(void);
extern void Ov236_FrontRiders_Release(void);
extern void Ov236_RiderHitFilter(void);
extern void Ov236_RebuildRiderListsA(void);
extern void *Ov107_PackTextureHandle(int pool, int index);
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
extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern int Ov107_Mover_New(const Capsule *capsule);
extern void Ov236_RiderPresenceHook(char *self);
extern void RefreshObjectCallbacks(int item, int a);
extern IdTable5 data_ov236_020d6314;
extern char data_ov236_020d64c8[];
extern char data_ov236_020d64d4[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_0204224c;
extern const VecFx32 data_02042270;

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov236_FrontRidersConstruct(char *self)
{
    IdTable5 ids = data_ov236_020d6314;
    Capsule cap;
    VecFx32 zero;
    u16 hw;
    int i;
    int *slot;

    *(Callback *)(self + 0x8) = Ov236_FrontRiders_Destroy;
    *(Callback *)(self + 0xc) = Ov236_UpdateShadowB;
    *(Callback *)(self + 0x10) = Ov236_InitModelPoses;
    *(Callback *)(self + 0x20) = Ov236_SendStatusWithPos;
    *(Callback *)(self + 0x1c) = Ov236_HandleRiderMessage;
    *(Callback *)(self + 0x30) = Ov236_RidersA_CreateAiTask;
    *(Callback *)(self + 0x34) = Ov236_TeardownHook;
    *(Callback *)(self + 0x28) = Ov236_CopyPoseFromParent394;
    *(Callback *)(self + 0x1d4) = Ov236_FrontRiders_Release;
    *(Callback *)(self + 0x1d0) = Ov236_RiderHitFilter;
    *(Callback *)(self + 0x1dc) = Ov236_RebuildRiderListsA;
    *(unsigned char *)(self + 0x1c9) = 2;
    *(int *)(self + 0x54) = 0;
    *(int *)(self + 0x58) = 0;
    /* default scales first: the overwritten stores are dropped after scheduling but still spend
     * the block's scheduling budget, which keeps the ROM's argument order for the rest of it */
    *(int *)(self + 0x70) = 0x1000;
    *(int *)(self + 0x70) = 0x2c00;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x1000;
    *(int *)(self + 0x68) = 0x2c00;
    *(int *)(self + 0x6c) = 0;
    *(u16 *)(self + 0x100 + 0xae) |= 0x18;
    *(u16 *)(self + 0x100 + 0xb0) |= 0x888;
    hw = *(u16 *)(self + 0x60);
    *(u16 *)(self + 0x60) = (hw & ~0xff00) |
        ((((((unsigned int)hw << 0x10) >> 0x18) | 0x20) << 0x18) >> 0x10);
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(*(int *)(self + 0x394), 0x18));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    *(int *)(self + 0x388) = CallocInstance(0x24);
    Snd_RegisterSeqAndBind(*(int *)(self + 0x388), *(int *)(*(int *)(self + 0x384) + 0x88), Ov107_PackTextureHandle(*(int *)(self + 0x394), 0x19), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x384), *(int *)(self + 0x388));
    *(int *)(self + 0x3a0) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov236_020d64c8);
    *(int *)(self + 0x3a4) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov236_020d64d4);
    *(int *)(self + 0x3a8) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov236_020d64c8);
    *(int *)(self + 0x3ac) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov236_020d64d4);
    *(int *)(self + 0x38c) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(*(int *)(self + 0x394), 0x3d));
    Ov107_EnqueueValue(self, *(int *)(self + 0x38c));
    *(int *)(self + 0x390) = CallocInstance(0x24);
    Snd_RegisterSeqAndBind(*(int *)(self + 0x390), *(int *)(*(int *)(self + 0x38c) + 0x88), Ov107_PackTextureHandle(*(int *)(self + 0x394), 0x19), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x38c), *(int *)(self + 0x390));
    Srt_SetScaleXYZ(*(int *)(self + 0x38c) + 4, 0x1000, 1, 0x1000);
    Srt_SetTranslationXYZ(*(int *)(self + 0x38c) + 4, 0, 0x200, 0);
    NNS_G3dMdlSetMdlAlphaAll(*(int *)(*(int *)(*(int *)(self + 0x38c) + 0x88) + 0x78), 8);
    *(int *)(self + 0x3b8) = CallocInstance(0x28);
    for (i = 0; i < 5; i++) {
        ((struct Pair *)*(int *)(self + 0x3b8))[i].res = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(*(int *)(self + 0x394), ids.id[i]));
        Ov107_EnqueueValue(self, ((struct Pair *)*(int *)(self + 0x3b8))[i].res);
        *(int *)(((struct Pair *)*(int *)(self + 0x3b8))[i].res + 0x5c) |= 2;
    }
    zero = data_02041dc8;
    cap.pos = zero;
    cap.axis = data_0204224c;
    cap.length = 0x1800;
    cap.radius = 0x800;
    slot = List_InsertSorted(self + 0x144, 4, 100);
    *(int *)(self + 0x398) = *slot = Ov107_Mover_New(&cap);
    cap.radius = FX_Mul(cap.radius, 0x1200);
    *(int *)(self + 0x3b0) = (int)List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x3b0) = Ov107_Mover_New(&cap);
    ((B8 *)(*(int *)(self + 0x3b0) + 8))->f |= 1;
    cap.pos = zero;
    cap.axis = data_02042270;
    cap.length = 0x1800;
    cap.radius = 0x800;
    slot = List_InsertSorted(self + 0x144, 4, 100);
    *(int *)(self + 0x39c) = *slot = Ov107_Mover_New(&cap);
    cap.radius = FX_Mul(cap.radius, 0x1200);
    *(int *)(self + 0x3b4) = (int)List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x3b4) = Ov107_Mover_New(&cap);
    ((B8 *)(*(int *)(self + 0x3b4) + 8))->f |= 1;
    *(short *)(self + 0x300 + 0xbc) = *(short *)(self + 0x300 + 0xbe) = 1;
    ((struct b1 *)(self + 0x3c0))->b0 = 1;
    Ov236_RiderPresenceHook(self);
    RefreshObjectCallbacks(*(int *)(self + 0x384), 0);
    RefreshObjectCallbacks(*(int *)(self + 0x38c), 0);
}

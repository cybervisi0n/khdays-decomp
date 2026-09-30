/* Constructor of the rear rider controller (created by 020cdc84). Installs its handlers (+8, +0xc,
 * +0x10, +0x1c message, +0x20, +0x28, +0x30 update, +0x34, +0x1d0 hit filter, +0x1d4, +0x1dc move
 * player), sets kind 2, +0x54 = 0 / +0x58 = 0.5, the +0x64 pose (scale 2.75), bits 3-4 of +0x1ae,
 * bits 3, 7 and 11 of +0x1b0 and bit 5 of the +0x60 high byte. From the owner's +0x384 pool it
 * builds the +0x388 rider (pose 0x2a, subscribed to +0x9c) with its +0x38c work list (pose 0x2b)
 * and six bone attachments (+0x3a8..+0x3bc), and the +0x390 mount (pose 0x3e) with its +0x394 work
 * list (scaled 1/1/1 with flag 1, raised 1/8, list rate 8), resolves the +0x3c8 marker and builds
 * the four hidden sub-items of data_ov236_020d636c into the +0x3cc pair table. Four capsules
 * (length 1.4, radius 0.375, along y, z, y and x) are reserved at +0x398..+0x3a4 on the +0x144 pool
 * and two (radius scaled by 1.125, along z and x) at +0x3c0 / +0x3c4 on the +0x22c pool. Both
 * rider counters (+0x3d0 / +0x3d2) start at 1, +0x3d4 bit 0 is set, the presence hook runs and
 * both rigs re-init. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { int id[4]; } IdTable4;
typedef struct { VecFx32 pos; VecFx32 axis; int length; int radius; } Capsule;
struct Pair { int res; int handle; };
struct b1 { unsigned int b0 : 1; };

extern void Ov236_RearRiders_Destroy(void);
extern void Ov236_UpdateShadow(void);
extern void Ov236_InitRiderAnchors(void);
extern void Ov236_SendStatusWithPosB(void);
extern void Ov236_HandleRiderMessageB(void);
extern void Ov236_RidersB_CreateAiTask(void);
extern void func_ov236_020ce6fc(void);
extern void Ov236_CopyPoseFromParent384(void);
extern void Ov236_SuspendSubObjects(void);
extern void Ov236_RiderHitFilter_2(void);
extern void Ov236_RebuildRiderListsB(void);
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
extern int Ov107_CreateNamedResourceBinding(void *item, const char *name);
extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern int Ov107_Mover_New(const Capsule *capsule);
extern void Ov236_RefreshRearRiders(char *self);
extern void RefreshObjectCallbacks(int item, int a);
extern IdTable4 data_ov236_020d636c;
extern char data_ov236_020d64e0[];
extern char data_ov236_020d64ec[];
extern char data_ov236_020d64f8[];
extern char data_ov236_020d6504[];
extern const char data_ov236_020d6510[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042264;
extern const VecFx32 data_02042270;
extern const VecFx32 data_0204224c;

static inline int FX_Mul(int a, int b) {
    return (int)(((long long)a * b + 0x800) >> 12);
}

void Ov236_RearRidersConstruct(char *self)
{
    IdTable4 ids = data_ov236_020d636c;
    Capsule cap;
    VecFx32 axisY;
    VecFx32 axisZ;
    VecFx32 axisX;
    u16 hw;
    int i;
    int *slot;

    *(Callback *)(self + 0x8) = Ov236_RearRiders_Destroy;
    *(Callback *)(self + 0xc) = Ov236_UpdateShadow;
    *(Callback *)(self + 0x10) = Ov236_InitRiderAnchors;
    *(Callback *)(self + 0x20) = Ov236_SendStatusWithPosB;
    *(Callback *)(self + 0x1c) = Ov236_HandleRiderMessageB;
    *(Callback *)(self + 0x30) = Ov236_RidersB_CreateAiTask;
    *(Callback *)(self + 0x34) = func_ov236_020ce6fc;
    *(Callback *)(self + 0x28) = Ov236_CopyPoseFromParent384;
    *(Callback *)(self + 0x1d4) = Ov236_SuspendSubObjects;
    *(Callback *)(self + 0x1d0) = Ov236_RiderHitFilter_2;
    *(Callback *)(self + 0x1dc) = Ov236_RebuildRiderListsB;
    *(unsigned char *)(self + 0x1c9) = 2;
    *(int *)(self + 0x54) = 0;
    *(int *)(self + 0x58) = 0x800;
    *(int *)(self + 0x70) = 0x2c00;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x2c00;
    *(int *)(self + 0x6c) = 0;
    *(u16 *)(self + 0x100 + 0xae) |= 0x18;
    *(u16 *)(self + 0x100 + 0xb0) |= 0x888;
    hw = *(u16 *)(self + 0x60);
    *(u16 *)(self + 0x60) = (hw & ~0xff00) |
        ((((((unsigned int)hw << 0x10) >> 0x18) | 0x20) << 0x18) >> 0x10);
    *(int *)(self + 0x388) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(*(int *)(self + 0x384), 0x2a));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x388));
    *(int *)(self + 0x38c) = CallocInstance(0x24);
    Snd_RegisterSeqAndBind(*(int *)(self + 0x38c), *(int *)(*(int *)(self + 0x388) + 0x88), Ov107_PackTextureHandle(*(int *)(self + 0x384), 0x2b), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x388), *(int *)(self + 0x38c));
    *(int *)(self + 0x3a8) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 1, data_ov236_020d64e0);
    *(int *)(self + 0x3ac) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 3, data_ov236_020d64ec);
    *(int *)(self + 0x3b0) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 1, data_ov236_020d64f8);
    *(int *)(self + 0x3b4) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 3, data_ov236_020d6504);
    *(int *)(self + 0x3b8) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 3, data_ov236_020d64e0);
    *(int *)(self + 0x3bc) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 3, data_ov236_020d64f8);
    *(int *)(self + 0x390) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(*(int *)(self + 0x384), 0x3e));
    Ov107_EnqueueValue(self, *(int *)(self + 0x390));
    *(int *)(self + 0x394) = CallocInstance(0x24);
    Snd_RegisterSeqAndBind(*(int *)(self + 0x394), *(int *)(*(int *)(self + 0x390) + 0x88), Ov107_PackTextureHandle(*(int *)(self + 0x384), 0x2b), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x390), *(int *)(self + 0x394));
    Srt_SetScaleXYZ(*(int *)(self + 0x390) + 4, 0x1000, 1, 0x1000);
    Srt_SetTranslationXYZ(*(int *)(self + 0x390) + 4, 0, 0x200, 0);
    NNS_G3dMdlSetMdlAlphaAll(*(int *)(*(int *)(*(int *)(self + 0x390) + 0x88) + 0x78), 8);
    *(int *)(self + 0x3c8) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(*(int *)(self + 0x384), 0x3f), data_ov236_020d6510);
    *(int *)(self + 0x3cc) = CallocInstance(0x20);
    for (i = 0; i < 4; i++) {
        ((struct Pair *)*(int *)(self + 0x3cc))[i].res = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(*(int *)(self + 0x384), ids.id[i]));
        Ov107_EnqueueValue(self, ((struct Pair *)*(int *)(self + 0x3cc))[i].res);
        *(int *)(((struct Pair *)*(int *)(self + 0x3cc))[i].res + 0x5c) |= 2;
    }
    cap.pos = data_02041dc8;
    axisY = data_02042264;
    cap.axis = axisY;
    cap.length = 0x1666;
    cap.radius = 0x600;
    slot = List_InsertSorted(self + 0x144, 4, 100);
    *(int *)(self + 0x398) = *slot = Ov107_Mover_New(&cap);
    axisZ = data_02042270;
    cap.axis = axisZ;
    slot = List_InsertSorted(self + 0x144, 4, 100);
    *(int *)(self + 0x39c) = *slot = Ov107_Mover_New(&cap);
    cap.axis = axisY;
    slot = List_InsertSorted(self + 0x144, 4, 100);
    *(int *)(self + 0x3a0) = *slot = Ov107_Mover_New(&cap);
    axisX = data_0204224c;
    cap.axis = axisX;
    slot = List_InsertSorted(self + 0x144, 4, 100);
    *(int *)(self + 0x3a4) = *slot = Ov107_Mover_New(&cap);
    cap.radius = FX_Mul(cap.radius, 0x1200);
    cap.axis = axisZ;
    *(int *)(self + 0x3c0) = (int)List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x3c0) = Ov107_Mover_New(&cap);
    cap.axis = axisX;
    *(int *)(self + 0x3c4) = (int)List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x3c4) = Ov107_Mover_New(&cap);
    *(short *)(self + 0x300 + 0xd0) = *(short *)(self + 0x300 + 0xd2) = 1;
    ((struct b1 *)(self + 0x3d4))->b0 = 1;
    Ov236_RefreshRearRiders(self);
    RefreshObjectCallbacks(*(int *)(self + 0x388), 0);
    RefreshObjectCallbacks(*(int *)(self + 0x390), 0);
}

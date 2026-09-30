/* Constructor of the ov231 enemy (x5 with ov232/ov263/ov265/ov280). Installs the handlers (+8,
 * +0xc draw, +0x1c message, +0x28, +0x2c, +0x30, +0x34 update, +0x1d0 hit filter, +0x1dc, +0x1e0),
 * sets the +0x1fc bounds box (+/-2.0 wide, 4.0 tall), the +0x64 pose (scale 1.07) and bit 3 of
 * +0x1ae, builds the +0x384 rig from pose 0 (subscribed to +0x9c, 12-channel animation set 1 bound at
 * +0x394) and resolves its four bones (+0x3cc/+0x3d0 in set 1, +0x3d4/+0x3d8 in set 3); raises bit
 * 6 of the +0x60 high byte, keeps the "move" handle of set 0x11 (+0x388), builds the eight sub-items
 * of data_ov232_020d3650 into the +0x3b8 pair table (registered, bit 1 of +0x5c) and spawns the two
 * +0x38c children (Ov232_Item_New, bit 2 of their body's +0x5c). Registers actions 0/2, 1/2
 * and 4/2 lifted 1.5 above the +0xb0 point and 2/3 (all at rate 0.64); reserves the +0x22c
 * placement (+0x3bc) and the three +0x144 handles (+0x3c0: a placement, then two oriented boxes of
 * half-extents 0.5/0.25/0.5), and loads sound 0x155. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { void *node; int pad; } Slot;
typedef struct { int w[8]; } KindTable;
typedef struct { int w[6]; } ParamBlock;
typedef struct { VecFx32 pos; int scale; } Placement;
typedef struct { VecFx32 center; VecFx32 ax; VecFx32 ay; VecFx32 az; int extent[3]; } Obb;

extern void *Ov107_PackTextureHandle(void *self, int slot);
extern void *CreateSubitemInstance0xB4(void *res);
extern void RegisterSubscriberSlot(void *list, void *node);
extern void Snd_RegisterSeqAndBind(void *dst, void *a, void *b, int n);
extern void MainBlob_ResetSlotRows(void *obj, void *block);
extern int InsertSortedEntryWithKey(void *obj, int set, const char *name);
extern int Ov107_CreateNamedResourceBinding(void *res, const char *name);
extern void *CallocInstance(int size);
extern void Ov107_EnqueueValue(void *self, void *obj);
extern void *Ov232_Item_New(void *self);
extern void Ov107_Actor_SetAttachSlot(void *self, int a, int b, const VecFx32 *v, int e);
extern int FX_Div(int num, int den);
extern void *List_InsertSorted(void *list, int size, int count);
extern void *Ov107_CloneResourceTransform(const Placement *placement);
extern void *Ov107_HitShape_NewBox(const Obb *box);
extern void Res_RequestIdPair(int id);

extern KindTable data_ov232_020d3650;
extern const char data_ov232_020d36ac[];
extern const char data_ov232_020d36b8[];
extern const char data_ov232_020d36c4[];
extern const char data_ov232_020d36d0[];
extern const char data_ov232_020d36dc[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042264;
extern const VecFx32 data_02042258;

extern void Ov232_Destroy(void);
extern void Ov232_TickWithChildRefresh(void);
extern void Ov232_OnEffectMessage(void);
extern void Ov232_CreateAiTask(void);
extern void Ov232_ForwardEventToChildren(void);
extern void Ov232_NotifyPartsThenBase(void);
extern void Ov232_DrawPrePass(void);
extern void Ov232_FilterHit(void);
extern void Ov232_RebindClip(void);
extern void Ov232_TryRequestState13(void);

void Ov232_Construct(char *self)
{
    ParamBlock params;
    KindTable kinds;
    Placement place;
    Obb box;
    VecFx32 lift;
    VecFx32 zero;
    signed char i;

    kinds = data_ov232_020d3650;
    params.w[0] = -0x2000;
    params.w[1] = 0;
    params.w[2] = -0x2000;
    params.w[3] = params.w[0] + 0x4000;
    params.w[4] = params.w[1] + 0x4000;
    params.w[5] = params.w[2] + 0x4000;
    *(void **)(self + 0x8) = (void *)Ov232_Destroy;
    *(void **)(self + 0xc) = (void *)Ov232_TickWithChildRefresh;
    *(void **)(self + 0x1c) = (void *)Ov232_OnEffectMessage;
    *(void **)(self + 0x30) = (void *)Ov232_CreateAiTask;
    *(void **)(self + 0x28) = (void *)Ov232_ForwardEventToChildren;
    *(void **)(self + 0x2c) = (void *)Ov232_NotifyPartsThenBase;
    *(void **)(self + 0x34) = (void *)Ov232_DrawPrePass;
    *(void **)(self + 0x1d0) = (void *)Ov232_FilterHit;
    *(void **)(self + 0x1dc) = (void *)Ov232_RebindClip;
    *(ParamBlock *)(self + 0x1fc) = params;
    *(void **)(self + 0x1e0) = (void *)Ov232_TryRequestState13;
    *(int *)(self + 0x70) = 0x1119;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x1119;
    *(int *)(self + 0x6c) = 0;
    *(u16 *)(self + 0x1ae) |= 8;

    *(void **)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    RegisterSubscriberSlot(*(void **)(self + 0x9c), *(void **)(self + 0x384));
    {
        void *anim = Ov107_PackTextureHandle(self, 1);
        Snd_RegisterSeqAndBind(self + 0x394, *(void **)(*(char **)(self + 0x384) + 0x88), anim, 0xc);
        MainBlob_ResetSlotRows(*(void **)(self + 0x384), self + 0x394);
    }
    *(int *)(self + 0x3cc) = InsertSortedEntryWithKey(*(void **)(self + 0x384), 1, data_ov232_020d36ac);
    *(int *)(self + 0x3d0) = InsertSortedEntryWithKey(*(void **)(self + 0x384), 1, data_ov232_020d36b8);
    *(int *)(self + 0x3d4) = InsertSortedEntryWithKey(*(void **)(self + 0x384), 3, data_ov232_020d36c4);
    *(int *)(self + 0x3d8) = InsertSortedEntryWithKey(*(void **)(self + 0x384), 3, data_ov232_020d36d0);
    {
        unsigned int v = *(u16 *)(self + 0x60);
        *(u16 *)(self + 0x60) = (u16)((v & ~0xff00) | ((((v << 0x10) >> 0x18 | 0x40) << 0x18) >> 0x10));
    }
    *(int *)(self + 0x388) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(self, 0x11), data_ov232_020d36dc);
    *(void **)(self + 0x3b8) = CallocInstance(0x40);
    for (i = 0; i < 8; i++) {
        (*(Slot **)(self + 0x3b8))[i].node = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, kinds.w[i]));
        Ov107_EnqueueValue(self, (*(Slot **)(self + 0x3b8))[i].node);
        *(int *)((char *)(*(Slot **)(self + 0x3b8))[i].node + 0x5c) |= 2;
    }
    for (i = 0; i < 2; i++) {
        ((void **)(self + 0x38c))[i] = Ov232_Item_New(self);
        *(int *)(*(int *)((char *)((void **)(self + 0x38c))[i] + 0x9c) + 0x5c) |= 4;
    }
    lift = *(VecFx32 *)(self + 0xb0);
    lift.y += 0x1800;
    Ov107_Actor_SetAttachSlot(self, 0, 2, &lift, 0xa3c);
    Ov107_Actor_SetAttachSlot(self, 1, 2, &lift, 0xa3c);
    Ov107_Actor_SetAttachSlot(self, 2, 3, 0, 0xa3c);
    Ov107_Actor_SetAttachSlot(self, 4, 2, &lift, 0xa3c);

    place = *(Placement *)(self + 0x64);
    zero = data_02041dc8;
    place.pos = zero;
    box.center = zero;
    box.ax = data_02042270;
    box.ay = data_02042264;
    box.az = data_02042258;
    box.extent[0] = (int)(((long long)FX_Div(0x1119, 0x1119) * 0x800 + 0x800) >> 12);
    box.extent[1] = 0x400;
    box.extent[2] = (int)(((long long)FX_Div(0x1119, 0x1119) * 0x800 + 0x800) >> 12);
    *(void **)(self + 0x3bc) = List_InsertSorted(self + 0x22c, 0x10, 100);
    **(void ***)(self + 0x3bc) = Ov107_CloneResourceTransform(&place);
    for (i = 0; i < 3; i++) {
        void **slot = (void **)List_InsertSorted(self + 0x144, 4, 100);
        void *node = i == 0 ? Ov107_CloneResourceTransform(&place) : Ov107_HitShape_NewBox(&box);
        ((void **)(self + 0x3c0))[i] = *slot = node;
    }
    Res_RequestIdPair(0x155);
}

/* Constructor of the ov273 enemy (x2 with ov273). Installs the handlers (+8, +0xc, +0x1c message,
 * +0x20, +0x28, +0x2c, +0x30, +0x34 update, +0x1d0 hit filter, +0x1dc, +0x1e0), sets bit 6 of the
 * +0x60 high byte, the +0x64 pose (scale 2.0) and bits 3-4 of +0x1ae; builds the +0x384 body rig
 * (pose 0, subscribed to +0x9c, lowered 2.0, owned by the enemy with callback 020cbfc8) with its
 * embedded +0x38c work list (pose 1) and four bone attachments (+0x3e8..+0x3f4, the first's +0x14
 * becoming the +0x2cc anchor), and the +0x388 tail rig (pose 0x1d, lowered 2.0) with its embedded
 * +0x3b0 list (pose 0x1e); builds the eight hidden sub-items of data_ov273_020d69b4 into the +0x430
 * pair table and registers action 2/3 (rate 0.5). Shapes: a placement (scale 2.0) at +0x3d4 on
 * the +0x22c pool and an oriented box (half-extents 0.85 / 1.7 / 0.68) at +0x3d8 on the +0x144
 * pool. Creates the 020d0574 companion (+0x3dc) and eight 020d1628 / 020d238c children (+0x3e0 /
 * +0x3e4), then loads sound 0x162. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { int id[8]; } IdTable8;
typedef struct { VecFx32 pos; int scale; } Placement;
typedef struct { VecFx32 pos; VecFx32 axis[3]; int ext[3]; } Box;
struct Pair { int res; int handle; };

extern void Ov273_ReleaseNodeResources(void);
extern void Ov273_UpdateModelTransform(void);
extern void Ov273_SendEnabledStatusMsg(void);
extern void Ov273_HandleMessage(void);
extern void Ov273_TeardownHook(void);
extern void Ov273_CreateAiTask(void);
extern void Ov273_ForwardRegionEventToParts(void);
extern void Ov273_NotifyPartsThenBase(void);
extern void Ov273_OnHit(void);
extern void Ov273_RebindCollisionSlot(void);
extern void Ov273_ReserveNodeIfStateActive(void);
extern void Ov273_BroadcastWorkingPose(void);
extern void *Ov107_PackTextureHandle(char *self, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void Srt_SetTranslationXYZ(int srt, int x, int y, int z);
extern void Snd_RegisterSeqAndBind(void *list, int b, void *c, int d);
extern void MainBlob_ResetSlotRows(int a, void *list);
extern int InsertSortedEntryWithKey(int item, int kind, void *name);
extern int CallocInstance(int size);
extern void Ov107_EnqueueValue(char *self, int item);
extern void Ov107_Actor_SetAttachSlot(char *self, int slot, int a, const VecFx32 *v, int c);
extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern int Ov107_CloneResourceTransform(const Placement *placement);
extern int Ov107_HitShape_NewBox(const Box *box);
extern int Ov273_New(char *self);
extern int Ov273_New_2(char *self);
extern int Ov273_New_3(char *self);
extern void Res_RequestIdPair(int resourceId);
extern IdTable8 data_ov273_020d69b4;
extern char data_ov273_020d6bec[];
extern char data_ov273_020d6bf4[];
extern char data_ov273_020d6bfc[];
extern char data_ov273_020d6c0c[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042264;
extern const VecFx32 data_02042258;

void Ov273_Construct(char *self)
{
    IdTable8 ids = data_ov273_020d69b4;
    Box box;
    Placement place;
    VecFx32 zero;
    u16 hw;
    int i;
    int *slot;

    *(Callback *)(self + 0x8) = Ov273_ReleaseNodeResources;
    *(Callback *)(self + 0xc) = Ov273_UpdateModelTransform;
    *(Callback *)(self + 0x20) = Ov273_SendEnabledStatusMsg;
    *(Callback *)(self + 0x1c) = Ov273_HandleMessage;
    *(Callback *)(self + 0x34) = Ov273_TeardownHook;
    *(Callback *)(self + 0x30) = Ov273_CreateAiTask;
    *(Callback *)(self + 0x28) = Ov273_ForwardRegionEventToParts;
    *(Callback *)(self + 0x2c) = Ov273_NotifyPartsThenBase;
    *(Callback *)(self + 0x1d0) = Ov273_OnHit;
    *(Callback *)(self + 0x1dc) = Ov273_RebindCollisionSlot;
    *(Callback *)(self + 0x1e0) = Ov273_ReserveNodeIfStateActive;
    hw = *(u16 *)(self + 0x60);
    *(u16 *)(self + 0x60) = (hw & ~0xff00) |
        ((((((unsigned int)hw << 0x10) >> 0x18) | 0x40) << 0x18) >> 0x10);
    /* default scale / pose first: the overwritten stores are dropped after scheduling but spend
     * the block's scheduling budget (keeps the ROM's argument order of the attachment calls) */
    *(int *)(self + 0x70) = 0x1000;
    *(int *)(self + 0x70) = 0x2000;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x1000;
    *(int *)(self + 0x68) = 0;
    *(int *)(self + 0x6c) = 0;
    *(u16 *)(self + 0x100 + 0xae) |= 0x18;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    Srt_SetTranslationXYZ(*(int *)(self + 0x384) + 4, 0, -0x2000, 0);
    Snd_RegisterSeqAndBind(self + 0x38c, *(int *)(*(int *)(self + 0x384) + 0x88), Ov107_PackTextureHandle(self, 1), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x384), self + 0x38c);
    *(Callback *)(*(int *)(self + 0x384) + 0x74) = Ov273_BroadcastWorkingPose;
    *(char **)(*(int *)(self + 0x384) + 0x84) = self;
    *(int *)(self + 0x3e8) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov273_020d6bec);
    *(int *)(self + 0x3ec) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov273_020d6bf4);
    *(int *)(self + 0x3f0) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov273_020d6bfc);
    *(int *)(self + 0x3f4) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov273_020d6c0c);
    *(int *)(self + 0x2cc) = *(int *)(self + 0x3e8) + 0x14;
    *(int *)(self + 0x388) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0x1d));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x388));
    Srt_SetTranslationXYZ(*(int *)(self + 0x388) + 4, 0, -0x2000, 0);
    Snd_RegisterSeqAndBind(self + 0x3b0, *(int *)(*(int *)(self + 0x388) + 0x88), Ov107_PackTextureHandle(self, 0x1e), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x388), self + 0x3b0);
    *(int *)(self + 0x430) = CallocInstance(0x40);
    for (i = 0; i < 8; i++) {
        ((struct Pair *)*(int *)(self + 0x430))[i].res = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ids.id[i]));
        Ov107_EnqueueValue(self, ((struct Pair *)*(int *)(self + 0x430))[i].res);
        *(int *)(((struct Pair *)*(int *)(self + 0x430))[i].res + 0x5c) |= 2;
    }
    Ov107_Actor_SetAttachSlot(self, 2, 3, 0, 0x800);
    zero = data_02041dc8;
    place.pos = zero;
    place.scale = 0x2000;
    *(int *)(self + 0x3d4) = (int)List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x3d4) = Ov107_CloneResourceTransform(&place);
    box.pos = zero;
    box.axis[0] = data_02042270;
    box.axis[1] = data_02042264;
    box.axis[2] = data_02042258;
    box.ext[0] = 0xd99;
    box.ext[1] = 0x1b33;
    box.ext[2] = 0xae0;
    slot = List_InsertSorted(self + 0x144, 4, 100);
    *(int *)(self + 0x3d8) = *slot = Ov107_HitShape_NewBox(&box);
    *(int *)(self + 0x3dc) = Ov273_New(self);
    *(int *)(self + 0x3e0) = CallocInstance(0x20);
    for (i = 0; i < 8; i++) {
        (*(int **)(self + 0x3e0))[i] = Ov273_New_2(self);
    }
    *(int *)(self + 0x3e4) = CallocInstance(0x20);
    for (i = 0; i < 8; i++) {
        (*(int **)(self + 0x3e4))[i] = Ov273_New_3(self);
    }
    Res_RequestIdPair(0x162);
}

/* Constructor of the ov283 enemy. Installs the handlers (+8, +0xc, +0x1c message, +0x28, +0x2c,
 * +0x30, +0x38, +0x1d0 hit filter, +0x1dc), sets kind 2, the +0x64 pose (scale 1.0) and the +0x1fc
 * bounds (-0.93 / 0.006 / -0.52 to 0.93 / 1.77 / 0.32); builds the +0x384 body rig (pose 0,
 * subscribed to +0x9c) with its two hand attachments (xig_h_L / xig_h_R at +0x394 / +0x398), a
 * placement of the pose on the +0x22c pool (+0x388) and on the +0x144 pool (+0x38c); creates the two
 * 020ced9c helpers (+0x39c) and sixteen 020cf21c helpers (+0x3a4), builds the six hidden sub-items of
 * data_ov283_020cfb4c into the +0x3ec pair table, sets the +0x3e8 threshold to 70 % and loads sound
 * 0x17e (with a +0x3e4 partner) or 0x173. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { int id[6]; } IdTable6;
typedef struct { VecFx32 min; VecFx32 max; } Bounds;
typedef struct { VecFx32 pos; int scale; } Placement;
struct Pair { int res; int handle; };
struct Ov283Parts { char pad[0x39c]; int small[2]; int helpers[16]; char pad3e4[4]; int threshold; struct Pair items[6]; };

extern void Ov283_Destroy_2(void);
extern void Ov283_PropagateBlockToLinkedNodes(void);
extern void Ov283_ActorOnMessage(void);
extern void Ov283_ForwardRegionEventToParts(void);
extern void Ov283_NotifyPartsThenBase(void);
extern void Ov283_CreateAiTask(void);
extern void Ov283_Slot38_StoreValue(void);
extern void Ov283_OnDamage(void);
extern void Ov283_Model_ReapplyTrack0(void);
extern void *Ov107_PackTextureHandle(char *self, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern int InsertSortedEntryWithKey(int item, int kind, void *name);
extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern int Ov107_CloneResourceTransform(const Placement *placement);
extern int Ov283_AllocLinkChild390(char *self);
extern int Ov283_New(char *self);
extern void Ov107_EnqueueValue(char *self, int item);
extern void Res_RequestIdPair(int resourceId);
extern IdTable6 data_ov283_020cfb4c;
extern char data_ov283_020cfbec[];
extern char data_ov283_020cfbf4[];

void Ov283_Construct(char *self)
{
    IdTable6 ids = data_ov283_020cfb4c;
    Bounds bounds;
    int i;
    int *slot;

    bounds.min.x = -0xeea;
    bounds.min.y = 0x17;
    bounds.min.z = -0x858;
    bounds.max.x = bounds.min.x + 0x1dd3;
    bounds.max.y = bounds.min.y + 0x1c27;
    bounds.max.z = bounds.min.z + 0xd64;
    *(Callback *)(self + 0x8) = Ov283_Destroy_2;
    *(Callback *)(self + 0x1c) = Ov283_ActorOnMessage;
    *(Callback *)(self + 0xc) = Ov283_PropagateBlockToLinkedNodes;
    *(Callback *)(self + 0x30) = Ov283_CreateAiTask;
    *(Callback *)(self + 0x28) = Ov283_ForwardRegionEventToParts;
    *(Callback *)(self + 0x2c) = Ov283_NotifyPartsThenBase;
    *(Callback *)(self + 0x38) = Ov283_Slot38_StoreValue;
    *(Callback *)(self + 0x1d0) = Ov283_OnDamage;
    *(Callback *)(self + 0x1dc) = Ov283_Model_ReapplyTrack0;
    *(u8 *)(self + 0x1c9) = 2;
    *(int *)(self + 0x70) = 0x1000;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x1000;
    *(int *)(self + 0x6c) = 0;
    *(Bounds *)(self + 0x1fc) = bounds;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    *(int *)(self + 0x394) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov283_020cfbec);
    *(int *)(self + 0x398) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov283_020cfbf4);
    *(int *)(self + 0x388) = (int)List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x388) = Ov107_CloneResourceTransform((Placement *)(self + 0x64));
    slot = List_InsertSorted(self + 0x144, 4, 100);
    *(int *)(self + 0x38c) = *slot = Ov107_CloneResourceTransform((Placement *)(self + 0x64));
    for (i = 0; i < 2; i++) {
        ((struct Ov283Parts *)self)->small[i] = Ov283_AllocLinkChild390(self);
    }
    for (i = 0; i < 0x10; i++) {
        ((struct Ov283Parts *)self)->helpers[i] = Ov283_New(self);
    }
    for (i = 0; i < 6; i++) {
        ((struct Ov283Parts *)self)->items[i].res = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ids.id[i]));
        Ov107_EnqueueValue(self, ((struct Ov283Parts *)self)->items[i].res);
        *(int *)(((struct Ov283Parts *)self)->items[i].res + 0x5c) |= 2;
    }
    ((struct Ov283Parts *)self)->threshold = 0x46;
    Res_RequestIdPair(*(int *)(self + 0x3e4) != 0 ? 0x17e : 0x173);
}

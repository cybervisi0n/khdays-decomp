/* Constructor of the ov259 enemy. Keeps the save's variant flag (bit 2 of data_0204c240) in +0x428,
 * installs the handlers (+8, +0xc, +0x1c message, +0x30, +0x34, +0x28, +0x2c, +0x1d0 hit, +0x1dc), the
 * +0x1c9 kind (2), the unit +0x64 pose and the +0x1fc bounds box; builds the +0x38c body from pose 0
 * (1 for the variant; owned, callback 020cbfc8, subscribed to +0x9c), clears the two +0x394 slots
 * (-1), builds the +0x390 wing rig (pose 0x20, animation 0x21 at +0x3e0), the +0x40c bone of the body,
 * the +0x414 bone of pose 0x1f and the thirteen +0x430 sub-items of data_ov259_020d2f2c (attached,
 * hidden); the +0x404 / +0x408 placements come from the pose, the +0x384 and +0x388 helpers are
 * created (+0x42c set in between), and sound 0x17d (variant) or 0x172 loads. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { int id[13]; } IdTable13;
typedef struct { VecFx32 min; VecFx32 max; } Bounds;
struct Pair { int res; int handle; };
struct Ov259Parts { char pad[0x430]; struct Pair items[13]; };

extern void Ov259_Actor_Destroy(void);
extern void Ov259_Update(void);
extern void Ov259_OnMessage(void);
extern void Ov259_CreateAiTask(void);
extern void Ov259_DrawPrePass(void);
extern void Ov259_ForwardRegionEventToParts(void);
extern void Ov259_NotifyPartsThenBase(void);
extern void Ov259_OnHit(void);
extern void Ov259_SetPose(void);
extern void Ov259_ModelAnimTick(void);
extern void *Ov107_PackTextureHandle(char *self, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void Snd_RegisterSeqAndBind(void *dst, int a, void *b, int n);
extern void MainBlob_ResetSlotRows(int obj, void *block);
extern int InsertSortedEntryWithKey(int item, int kind, const char *name);
extern int Ov107_CreateNamedResourceBinding(void *item, const char *name);
extern void Ov107_EnqueueValue(char *self, int item);
extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern int Ov107_CloneResourceTransform(void *pose);
extern int Ov259_New(char *self);
extern int Ov259_New_2(char *self);
extern void Res_RequestIdPair(int resourceId);
extern IdTable13 data_ov259_020d2f2c;
extern u8 data_0204c240;
extern const char data_ov259_020d2fcc[];
extern const char data_ov259_020d2fd4[];

void Ov259_Construct(char *self)
{
    IdTable13 ids = data_ov259_020d2f2c;
    Bounds bounds;
    int i;
    int *slot;
    int node;

    *(int *)(self + 0x428) = data_0204c240 & 4;
    bounds.min.x = -0x190b;
    bounds.min.y = -6;
    bounds.min.z = -0x1fc4;
    bounds.max.x = bounds.min.x + 0x3217;
    bounds.max.y = bounds.min.y + 0x2cc3;
    bounds.max.z = bounds.min.z + 0x2597;
    *(Callback *)(self + 0x8) = Ov259_Actor_Destroy;
    *(Callback *)(self + 0xc) = Ov259_Update;
    *(Callback *)(self + 0x1c) = Ov259_OnMessage;
    *(Callback *)(self + 0x30) = Ov259_CreateAiTask;
    *(Callback *)(self + 0x34) = Ov259_DrawPrePass;
    *(Callback *)(self + 0x28) = Ov259_ForwardRegionEventToParts;
    *(Callback *)(self + 0x2c) = Ov259_NotifyPartsThenBase;
    *(Callback *)(self + 0x1d0) = Ov259_OnHit;
    *(Callback *)(self + 0x1dc) = Ov259_SetPose;
    *(u8 *)(self + 0x1c9) = 2;
    *(int *)(self + 0x70) = 0x1000;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x1000;
    *(int *)(self + 0x6c) = 0;
    *(Bounds *)(self + 0x1fc) = bounds;
    *(int *)(self + 0x38c) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, (data_0204c240 & 4) ? 1 : 0));
    *(char **)(*(int *)(self + 0x38c) + 0x84) = self;
    *(Callback *)(*(int *)(self + 0x38c) + 0x68) = Ov259_ModelAnimTick;
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x38c));
    for (i = 0; i < 2; i++) {
        *(signed char *)(self + 0x394 + i) = -1;
    }
    *(int *)(self + 0x390) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0x20));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x390));
    Snd_RegisterSeqAndBind(self + 0x3e0, *(int *)(*(int *)(self + 0x390) + 0x88), Ov107_PackTextureHandle(self, 0x21), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x390), self + 0x3e0);
    *(int *)(self + 0x40c) = InsertSortedEntryWithKey(*(int *)(self + 0x38c), 3, data_ov259_020d2fcc);
    *(int *)(self + 0x414) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(self, 0x1f), data_ov259_020d2fd4);
    for (i = 0; i < 13; i++) {
        ((struct Ov259Parts *)self)->items[i].res = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ids.id[i]));
        Ov107_EnqueueValue(self, ((struct Ov259Parts *)self)->items[i].res);
        *(int *)(((struct Ov259Parts *)self)->items[i].res + 0x5c) |= 2;
    }
    *(int **)(self + 0x404) = List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x404) = Ov107_CloneResourceTransform(self + 0x64);
    slot = List_InsertSorted(self + 0x144, 4, 100);
    node = Ov107_CloneResourceTransform(self + 0x64);
    *(int *)(self + 0x408) = *slot = node;
    *(int *)(self + 0x384) = Ov259_New(self);
    *(int *)(self + 0x42c) = 1;
    *(int *)(self + 0x388) = Ov259_New_2(self);
    Res_RequestIdPair(*(int *)(self + 0x428) != 0 ? 0x17d : 0x172);
}

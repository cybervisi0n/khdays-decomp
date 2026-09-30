/* Constructor of the ov256 enemy. Clears +0x464, installs the handlers (+8, +0xc, +0x1c message,
 * +0x30, +0x28, +0x2c, +0x34, +0x1d0 hit, +0x1dc), sets bit 6 of the +0x60 high byte, the +0x64 pose
 * (scale 2.2) and the +0x1fc bounds box; builds the +0x384 body rig (pose 0x23, animation 0x25 bound
 * at +0x388) and the +0x3ac shell rig (pose 0, animation 2 at +0x3b0), both subscribed to +0x9c; then
 * five hidden joint groups (+0x3d8/+0x3e4/+0x3f0/+0x3fc/+0x408) each holding a segment model (poses
 * 0x49-0x4d at +0x3d4/+0x3e0/+0x3ec/+0x3f8/+0x404) and two more empty groups (+0x414, +0x420); sets
 * bit 3 of +0x1ae and resolves seven bones of the two rigs; the +0x450 bone of pose 0x46; the sixteen
 * sub-items of data_ov256_020d2444 into the +0x46c pair table (attached, hidden); reserves the
 * +0x22c/+0x144 handles of a placement at the origin with the pose scale (+0x428, +0x42c); creates the
 * two +0x434 and five +0x43c helpers and clears +0x45c and +0x468.
 * Codegen: compiled with opt_common_subs off (push/pop scoped); the +0x60 update re-reads the
 * halfword instead of reusing a copy. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { int id[16]; } IdTable;
typedef struct { VecFx32 min; VecFx32 max; } Bounds;
typedef struct { VecFx32 pos; int scale; } Placement;
struct Pair { int res; int handle; };
struct Pairs { char pad[0x46c]; struct Pair pairs[16]; };
struct Bit0 { unsigned int b0 : 1; };

extern void Ov256_ReleaseNodeResources(void);
extern void Ov256_ModelUpdate(void);
extern void Ov256_MessageHandler(void);
extern void Ov256_CreateAiTask(void);
extern void Ov256_AddItemsToScene(void);
extern void Ov256_RemoveItemsFromScene(void);
extern void Ov256_DrawPrePass(void);
extern void Ov256_DamageHandler(void);
extern void Ov256_PlayRigMove(void);
extern void *Ov107_PackTextureHandle(char *self, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void Snd_RegisterSeqAndBind(void *dst, int a, void *b, int n);
extern void MainBlob_ResetSlotRows(int obj, void *block);
extern int ModelNode_New(void);
extern int InsertSortedEntryWithKey(int item, int kind, const char *name);
extern int Ov107_CreateNamedResourceBinding(void *item, const char *name);
extern void Ov107_EnqueueValue(char *self, int item);
extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern int Ov107_CloneResourceTransform(const Placement *placement);
extern int Ov256_Claw_New(char *self, int index);
extern int Ov256_Shard_New(char *self, int index);
extern IdTable data_ov256_020d2444;
extern const char data_ov256_020d268c[];
extern const char data_ov256_020d2694[];
extern const char data_ov256_020d269c[];
extern const char data_ov256_020d26a4[];
extern const char data_ov256_020d26ac[];
extern const char data_ov256_020d26b4[];
extern const char data_ov256_020d26c0[];
extern const char data_ov256_020d26cc[];
extern const VecFx32 data_02041dc8;

#pragma push
#pragma opt_common_subs off
void Ov256_EnemyConstruct(char *self)
{
    char *binding;
    IdTable ids = data_ov256_020d2444;
    Bounds bounds;
    Placement place;
    int i;
    int node;
    int *slot;

    bounds.min.x = -0x1c7c;
    bounds.min.y = 0;
    bounds.min.z = -0xa44;
    bounds.max.x = bounds.min.x + 0x38f9;
    bounds.max.y = bounds.min.y + 0x20a8;
    bounds.max.z = bounds.min.z;
    bounds.max.z += 0x107a;
    *(int *)(self + 0x464) = 0;
    *(Callback *)(self + 0x8) = Ov256_ReleaseNodeResources;
    *(Callback *)(self + 0xc) = Ov256_ModelUpdate;
    *(Callback *)(self + 0x1c) = Ov256_MessageHandler;
    *(Callback *)(self + 0x30) = Ov256_CreateAiTask;
    *(Callback *)(self + 0x28) = Ov256_AddItemsToScene;
    *(Callback *)(self + 0x2c) = Ov256_RemoveItemsFromScene;
    *(Callback *)(self + 0x34) = Ov256_DrawPrePass;
    *(Callback *)(self + 0x1d0) = Ov256_DamageHandler;
    *(Callback *)(self + 0x1dc) = Ov256_PlayRigMove;
    *(u16 *)(self + 0x60) = (*(u16 *)(self + 0x60) & ~0xff00) |
        ((((((unsigned int)*(u16 *)(self + 0x60) << 0x10) >> 0x18) | 0x40) << 0x18) >> 0x10);
    *(int *)(self + 0x70) = 0x2300;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x2300;
    *(int *)(self + 0x6c) = 0;
    *(Bounds *)(self + 0x1fc) = bounds;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0x23));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    {
        void *anim = Ov107_PackTextureHandle(self, 0x25);

        binding = self + 0x388;
        Snd_RegisterSeqAndBind(binding, *(int *)(*(int *)(self + 0x384) + 0x88), anim, 0xc);
    }
    MainBlob_ResetSlotRows(*(int *)(self + 0x384), binding);
    *(int *)(self + 0x3ac) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x3ac));
    {
        void *anim = Ov107_PackTextureHandle(self, 2);

        Snd_RegisterSeqAndBind(self + 0x3b0, *(int *)(*(int *)(self + 0x3ac) + 0x88), anim, 0xc);
    }
    MainBlob_ResetSlotRows(*(int *)(self + 0x3ac), self + 0x3b0);
    *(int *)(self + 0x3d8) = ModelNode_New();
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x3d8));
    ((struct Bit0 *)(*(int *)(self + 0x3d8) + 0x5c))->b0 = 1;
    *(int *)(self + 0x3d4) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0x49));
    RegisterSubscriberSlot(*(int *)(self + 0x3d8), *(int *)(self + 0x3d4));
    *(int *)(self + 0x3e4) = ModelNode_New();
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x3e4));
    ((struct Bit0 *)(*(int *)(self + 0x3e4) + 0x5c))->b0 = 1;
    *(int *)(self + 0x3e0) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0x4a));
    RegisterSubscriberSlot(*(int *)(self + 0x3e4), *(int *)(self + 0x3e0));
    *(int *)(self + 0x3f0) = ModelNode_New();
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x3f0));
    ((struct Bit0 *)(*(int *)(self + 0x3f0) + 0x5c))->b0 = 1;
    *(int *)(self + 0x3ec) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0x4b));
    RegisterSubscriberSlot(*(int *)(self + 0x3f0), *(int *)(self + 0x3ec));
    *(int *)(self + 0x3fc) = ModelNode_New();
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x3fc));
    ((struct Bit0 *)(*(int *)(self + 0x3fc) + 0x5c))->b0 = 1;
    *(int *)(self + 0x3f8) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0x4c));
    RegisterSubscriberSlot(*(int *)(self + 0x3fc), *(int *)(self + 0x3f8));
    *(int *)(self + 0x408) = ModelNode_New();
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x408));
    ((struct Bit0 *)(*(int *)(self + 0x408) + 0x5c))->b0 = 1;
    *(int *)(self + 0x404) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0x4d));
    RegisterSubscriberSlot(*(int *)(self + 0x408), *(int *)(self + 0x404));
    *(int *)(self + 0x414) = ModelNode_New();
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x414));
    ((struct Bit0 *)(*(int *)(self + 0x414) + 0x5c))->b0 = 1;
    *(int *)(self + 0x420) = ModelNode_New();
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x420));
    ((struct Bit0 *)(*(int *)(self + 0x420) + 0x5c))->b0 = 1;
    *(u16 *)(self + 0x100 + 0xae) |= 8;
    *(int *)(self + 0x3dc) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov256_020d268c);
    *(int *)(self + 0x3e8) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov256_020d2694);
    *(int *)(self + 0x3f4) = InsertSortedEntryWithKey(*(int *)(self + 0x3ac), 3, data_ov256_020d269c);
    *(int *)(self + 0x400) = InsertSortedEntryWithKey(*(int *)(self + 0x3ac), 3, data_ov256_020d26a4);
    *(int *)(self + 0x40c) = InsertSortedEntryWithKey(*(int *)(self + 0x3ac), 3, data_ov256_020d26ac);
    *(int *)(self + 0x418) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov256_020d26b4);
    *(int *)(self + 0x424) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov256_020d26c0);
    place = *(Placement *)(self + 0x64);
    place.pos = data_02041dc8;
    *(int *)(self + 0x450) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(self, 0x46), data_ov256_020d26cc);
    for (i = 0; i < 16; i++) {
        node = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ids.id[i]));
        Ov107_EnqueueValue(self, ((struct Pairs *)self)->pairs[i].res = node);
        *(int *)(((struct Pairs *)self)->pairs[i].res + 0x5c) |= 2;
    }
    *(int **)(self + 0x428) = List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x428) = Ov107_CloneResourceTransform(&place);
    slot = List_InsertSorted(self + 0x144, 4, 100);
    node = Ov107_CloneResourceTransform(&place);
    *(int *)(self + 0x42c) = *slot = node;
    for (i = 0; i < 2; i++) {
        ((int *)(self + 0x434))[i] = Ov256_Claw_New(self, (u8)i);
    }
    for (i = 0; i < 5; i++) {
        ((int *)(self + 0x43c))[i] = Ov256_Shard_New(self, (u8)i);
    }
    *(int *)(self + 0x45c) = 0;
    *(int *)(self + 0x468) = 0;
}
#pragma pop

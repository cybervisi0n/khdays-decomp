/* Constructor of the ov208 enemy (x3 with ov209/ov268). Installs the handlers (+8, +0xc draw,
 * +0x1c message, +0x28, +0x2c, +0x30, +0x34 update, +0x1d0 hit filter, +0x1dc, +0x1e0), sets the
 * +0x1fc bounds box, the +0x64 pose (scale 2.36, re-read from +0x70) and bit 3 of +0x1ae, builds the +0x384 rig from
 * pose 0 (+0x74 callback Ov208_SnapshotNudgeMatrix, back-pointer at +0x84, subscribed to +0x9c,
 * 12-channel animation set 1 bound at +0x388, reset) and resolves five bones (+0x3d0/+0x3d4/+0x3d8
 * /+0x3dc in set 1, +0x3cc in set 3). Registers actions 0/2 (rate 1.05), 2/3 lowered 2.36
 * (0.49) and 1/2 (0.65); builds the five sub-items of data_ov208_020d4784 into the +0x40c pair
 * table (the first from the shared scene resources, registered, bit 1 of +0x5c); keeps the
 * "move" handle of set 0x18 (+0x3ac); reserves the +0x22c and +0x144 collision handles (+0x3b4/
 * +0x3bc) from a zero capsule pointing up (radius 1.32, height 1.47), the +0x22c/+0x144 placements
 * +0x3b8 (0.76), +0x3c0 (1.0) and +0x3c4/+0x3c8 (1.43); spawns the item into +0x3b0 and loads
 * sound 0x154. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { int w[5]; } PoseTable;
typedef struct { VecFx32 min; VecFx32 max; } Bounds;
typedef struct { VecFx32 pos; VecFx32 up; int radius; int height; } Capsule;
typedef struct { VecFx32 pos; int scale; } Placement;
struct Pair { int res; int handle; };

extern int CreateSubitemInstance0xB4(unsigned res);
extern void RegisterSubscriberSlot(int list, int obj);
extern void Snd_RegisterSeqAndBind(void *set, int model, unsigned anim, int n);
extern void MainBlob_ResetSlotRows(int obj, void *set);
extern void RefreshObjectCallbacks(int obj, int v);
extern int InsertSortedEntryWithKey(int obj, int set, const char *name);
extern void Ov107_Actor_SetAttachSlot(char *self, int a, int b, VecFx32 *lift, int rate);
extern void *CallocInstance(int size);
extern int Ov107_CreateNamedResourceBinding(unsigned res, const char *name);
extern int *List_InsertSorted(void *list, int stride, int max);
extern int Ov107_Mover_New(Capsule *capsule);
extern int Ov107_CloneResourceTransform(Placement *placement);
extern int Ov208_Item_New(char *self);
extern void Res_RequestIdPair(int id);
extern const PoseTable data_ov208_020d4784;
extern const char data_ov208_020d482c[];
extern const char data_ov208_020d4838[];
extern const char data_ov208_020d4848[];
extern const char data_ov208_020d4858[];
extern const char data_ov208_020d4860[];
extern const char data_ov208_020d4870[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042264;
extern void Ov208_Destroy(void);
extern void Ov208_DrawPushAway(void);
extern void Ov208_OnEffectMessage(void);
extern void Ov208_CreateNodeRegistryEntry(void);
extern void Ov208_ForwardRegionEventToPart(void);
extern void Ov208_DetachSubNodesHandOff(void);
extern void Ov208_Update(void);
extern void Ov208_FilterHit(void);
extern void Ov208_RequestSubState12IfIdleIn2Or4(void);
extern void Ov208_MessageMapTableForward(void);
extern void Ov208_SnapshotNudgeMatrix(void);

typedef void (*Callback)();
static inline void VEC_Set(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void Ov208_EnemyConstruct(char *self)
{
    PoseTable poses;
    Placement place;
    Capsule capsule;
    Bounds box;
    VecFx32 lift;
    VecFx32 zero;
    int i;
    int *p;
    int v;

    poses = data_ov208_020d4784;
    box.min.x = -0x209e;
    box.min.y = -0x16;
    box.min.z = -0xb9b;
    box.max.x = box.min.x + 0x4117;
    box.max.y = box.min.y + 0x1eed;
    box.max.z = box.min.z + 0x1947;
    *(Callback *)(self + 0x8) = Ov208_Destroy;
    *(Callback *)(self + 0xc) = Ov208_DrawPushAway;
    *(Callback *)(self + 0x1c) = Ov208_OnEffectMessage;
    *(Callback *)(self + 0x30) = Ov208_CreateNodeRegistryEntry;
    *(Callback *)(self + 0x28) = Ov208_ForwardRegionEventToPart;
    *(Callback *)(self + 0x2c) = Ov208_DetachSubNodesHandOff;
    *(Callback *)(self + 0x34) = Ov208_Update;
    *(Callback *)(self + 0x1d0) = Ov208_FilterHit;
    *(Callback *)(self + 0x1e0) = Ov208_RequestSubState12IfIdleIn2Or4;
    *(Callback *)(self + 0x1dc) = Ov208_MessageMapTableForward;
    *(Bounds *)(self + 0x1fc) = box;
    *(int *)(self + 0x70) = 0x25b3;
    VEC_Set((VecFx32 *)(self + 0x64), 0, *(int *)(self + 0x70), 0);
    *(u16 *)(self + 0x1ae) |= 8;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    *(void **)(*(int *)(self + 0x384) + 0x74) = (void *)Ov208_SnapshotNudgeMatrix;
    *(char **)(*(int *)(self + 0x384) + 0x84) = self;
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    Snd_RegisterSeqAndBind(self + 0x388, *(int *)(*(int *)(self + 0x384) + 0x88), Ov107_PackTextureHandle(self, 1), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x384), self + 0x388);
    RefreshObjectCallbacks(*(int *)(self + 0x384), 0);
    *(int *)(self + 0x3d0) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov208_020d482c);
    *(int *)(self + 0x3d4) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov208_020d4838);
    *(int *)(self + 0x3d8) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov208_020d4848);
    *(int *)(self + 0x3cc) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov208_020d4858);
    *(int *)(self + 0x3dc) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov208_020d4860);
    lift.x = 0;
    lift.y = -0x25b3;
    lift.z = 0;
    Ov107_Actor_SetAttachSlot(self, 0, 2, 0, 0x10da);
    Ov107_Actor_SetAttachSlot(self, 2, 3, &lift, 0x7c7);
    Ov107_Actor_SetAttachSlot(self, 1, 2, 0, 0xa5f);
    *(void **)(self + 0x40c) = CallocInstance(0x28);
    for (i = 0; i < 5; i++) {
        if (i <= 0) {
            unsigned mask = 0xfffffc;
            (*(struct Pair **)(self + 0x40c))[i].res = CreateSubitemInstance0xB4(
                (((*(unsigned *)(Ov107_GetActorManager() + 0x88) + 0x8000) & mask) << 7 | 0x80000000)
                | (poses.w[i] & 0x1ff));
        } else {
            (*(struct Pair **)(self + 0x40c))[i].res = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, poses.w[i]));
        }
        Ov107_EnqueueValue(self, (*(struct Pair **)(self + 0x40c))[i].res);
        *(int *)((*(struct Pair **)(self + 0x40c))[i].res + 0x5c) |= 2;
    }
    *(int *)(self + 0x3ac) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(self, 0x18), data_ov208_020d4870);
    zero = data_02041dc8;
    capsule.pos = zero;
    capsule.up = data_02042264;
    capsule.radius = 0x1510;
    capsule.height = 0x1790;
    *(int **)(self + 0x3b4) = List_InsertSorted(self + 0x22c, 0x10, 0x6e);
    **(int **)(self + 0x3b4) = Ov107_Mover_New(&capsule);
    p = List_InsertSorted(self + 0x144, 4, 0x64);
    v = (*p = Ov107_Mover_New(&capsule));
    *(int *)(self + 0x3bc) = v;
    place.pos = zero;
    place.scale = 0xc27;
    *(int **)(self + 0x3b8) = List_InsertSorted(self + 0x22c, 0x10, 0x64);
    **(int **)(self + 0x3b8) = Ov107_CloneResourceTransform(&place);
    place.scale = 0x1009;
    p = List_InsertSorted(self + 0x144, 4, 0x64);
    v = (*p = Ov107_CloneResourceTransform(&place));
    *(int *)(self + 0x3c0) = v;
    place.pos = zero;
    place.scale = 0x16d8;
    p = List_InsertSorted(self + 0x144, 4, 0x64);
    v = (*p = Ov107_CloneResourceTransform(&place));
    *(int *)(self + 0x3c4) = v;
    p = List_InsertSorted(self + 0x144, 4, 0x64);
    v = (*p = Ov107_CloneResourceTransform(&place));
    *(int *)(self + 0x3c8) = v;
    *(int *)(self + 0x3b0) = Ov208_Item_New(self);
    Res_RequestIdPair(0x154);
}

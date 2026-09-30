/* Constructor of the ov254 boss. Installs the handlers (+8, +0xc, +0x28, +0x2c, +0x1c message, +0x30,
 * +0x34, +0x48, +0x1d0 hit, +0x1dc), the +0x1fc bounds box, kind 4, the +0x64 pose (scale 6.65) and
 * the +0x60 high bits 5-7 / +0x1ae bit 3. Builds the +0x384 body (pose 0, animation 1 at +0x38c,
 * bones +0x408 / +0x428 / +0x42c) and the +0x388 upper rig (pose 0x1f, animation 0x20 at +0x3b0,
 * seven bones +0x40c..+0x424), the +0x434 list of nine ground anchor points, the +0x430 bone of pose
 * 0x3e and the eight +0x4e8 sub-items of data_ov254_020d592c (attached, hidden). Five colliders are
 * placed at the origin: spheres of radius 7.0, 4.2 and 1/256 and two capsules along the forward axis
 * (length 8.4, radius 1.57); each goes both into the +0x144 pool (kept in +0x3f4) and the +0x22c pool
 * (kept in +0x3e0). The four helpers (+0x468, +0x45c, +0x460, +0x464), sixteen +0x46c shards and ten
 * +0x4ac debris pieces are created and sound 0x16d loads. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { int id[8]; } IdTable8;
typedef struct { int w[6]; } Bounds;
typedef struct { VecFx32 pos; int nRadius; } Sphere;
typedef struct { VecFx32 p0; VecFx32 dir; int nLength; int nRadius; } Segment;
struct Pair { int res; int handle; };
struct Ov254 {
    char pad000[0x3e0];
    int *bodyShapes[5];     /* +0x3e0 */
    int bodyNodes[5];       /* +0x3f4 */
    char pad408[0x46c - 0x408];
    int shards[16];         /* +0x46c */
    int debris[10];         /* +0x4ac */
    char pad4d4[0x4e8 - 0x4d4];
    struct Pair items[8];   /* +0x4e8 */
};

extern void Ov254_ReleaseNodeResources(void);
extern void Ov254_TickWithChildRefresh(void);
extern void Ov254_AttachHook(void);
extern void Ov254_DetachHook(void);
extern void Ov254_HelperCHandleMessage(void);
extern void Ov254_CreateAiTask(void);
extern void Ov254_DrawPrePass(void);
extern void Ov254_MoveStep(void);
extern void Ov254_HitFilter(void);
extern void Ov254_RebindSubObjects(void);
extern void *Ov107_PackTextureHandle(char *self, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void Snd_RegisterSeqAndBind(void *dst, int a, void *b, int n);
extern void MainBlob_ResetSlotRows(int obj, void *block);
extern int InsertSortedEntryWithKey(int item, int kind, const char *name);
extern void List_Init(void *pool);
extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern int Ov107_CreateNamedResourceBinding(void *item, const char *name);
extern void Ov107_EnqueueValue(char *self, int item);
extern int Ov107_Mover_New(Segment *seg);
extern int Ov107_CloneResourceTransform(Sphere *sphere);
extern int Ov254_HelperD_New(char *self);
extern int Ov254_Pillar_New(char *self);
extern int Ov254_Marker_New(char *self);
extern int Ov254_TwinMarker_New(char *self);
extern int Ov254_Helper_New(char *self);
extern int Ov254_HelperE_New(char *self);
extern void Res_RequestIdPair(int resourceId);
extern IdTable8 data_ov254_020d592c;
extern Bounds data_ov254_020d5914;
extern const char data_ov254_020d598c[];
extern const char data_ov254_020d5994[];
extern const char data_ov254_020d5998[];
extern const char data_ov254_020d599c[];
extern const char data_ov254_020d59a4[];
extern const char data_ov254_020d59ac[];
extern const char data_ov254_020d59b8[];
extern const char data_ov254_020d59c4[];
extern const char data_ov254_020d59d0[];
extern const char data_ov254_020d59dc[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042258;

static inline void SetAnchor(char *self, int x, int z)
{
    int *p = List_InsertSorted(self + 0x434, 0xc, 100);

    p[0] = x;
    p[1] = 0;
    p[2] = z;
}

void Ov254_Construct(char *self)
{
    IdTable8 ids = data_ov254_020d592c;
    Sphere sph;
    Segment seg;
    VecFx32 zero;
    int i;
    int *slot;
    int capsule;
    int k;
    int node;

    *(Callback *)(self + 0x8) = Ov254_ReleaseNodeResources;
    *(Callback *)(self + 0xc) = Ov254_TickWithChildRefresh;
    *(Callback *)(self + 0x28) = Ov254_AttachHook;
    *(Callback *)(self + 0x2c) = Ov254_DetachHook;
    *(Callback *)(self + 0x1c) = Ov254_HelperCHandleMessage;
    *(Callback *)(self + 0x30) = Ov254_CreateAiTask;
    *(Callback *)(self + 0x34) = Ov254_DrawPrePass;
    *(Callback *)(self + 0x48) = Ov254_MoveStep;
    *(Callback *)(self + 0x1d0) = Ov254_HitFilter;
    *(Callback *)(self + 0x1dc) = Ov254_RebindSubObjects;
    *(Bounds *)(self + 0x1fc) = data_ov254_020d5914;
    *(u8 *)(self + 0x1c9) = 4;
    *(int *)(self + 0x70) = 0x1000;
    *(int *)(self + 0x70) = 0x6a65;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0;
    *(int *)(self + 0x6c) = 0;
    {
        u16 hw = *(u16 *)(self + 0x60);

        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0xe0) << 0x18) >> 0x10);
    }
    *(u16 *)(self + 0x100 + 0xae) |= 8;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    Snd_RegisterSeqAndBind(self + 0x38c, *(int *)(*(int *)(self + 0x384) + 0x88), Ov107_PackTextureHandle(self, 1), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x384), self + 0x38c);
    *(int *)(self + 0x408) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov254_020d598c);
    *(int *)(self + 0x428) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov254_020d5994);
    *(int *)(self + 0x42c) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov254_020d5998);
    *(int *)(self + 0x388) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0x1f));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x388));
    Snd_RegisterSeqAndBind(self + 0x3b0, *(int *)(*(int *)(self + 0x388) + 0x88), Ov107_PackTextureHandle(self, 0x20), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x388), self + 0x3b0);
    *(int *)(self + 0x40c) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 1, data_ov254_020d599c);
    *(int *)(self + 0x410) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 3, data_ov254_020d599c);
    *(int *)(self + 0x414) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 3, data_ov254_020d59a4);
    *(int *)(self + 0x418) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 1, data_ov254_020d59ac);
    *(int *)(self + 0x41c) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 1, data_ov254_020d59b8);
    *(int *)(self + 0x420) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 1, data_ov254_020d59c4);
    *(int *)(self + 0x424) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 1, data_ov254_020d59d0);
    List_Init(self + 0x434);
    SetAnchor(self, -0x16000, -0xd000);
    SetAnchor(self, 0x9000, 0x4000);
    SetAnchor(self, 0x16000, 0x21000);
    SetAnchor(self, 0x5000, 0x2d000);
    SetAnchor(self, -0x2000, 0x32000);
    SetAnchor(self, -0x1b000, 0x2e000);
    SetAnchor(self, -0x28000, 0x2b000);
    SetAnchor(self, -0x24000, 0xe000);
    SetAnchor(self, -0x24000, 0);
    *(int *)(self + 0x430) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(self, 0x3e), data_ov254_020d59dc);
    for (i = 0; i < 8; i++) {
        ((struct Ov254 *)self)->items[i].res = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ids.id[i]));
        Ov107_EnqueueValue(self, ((struct Ov254 *)self)->items[i].res);
        *(int *)(((struct Ov254 *)self)->items[i].res + 0x5c) |= 2;
    }
    zero = data_02041dc8;
    sph.pos = zero;
    seg.p0 = zero;
    sph.nRadius = 0x7b31;
    seg.dir = data_02042258;
    seg.nLength = 0x8664;
    seg.nRadius = 0x1933;
    for (k = 0; k < 5; k++) {
        capsule = 0;
        switch (k) {
        case 0:
            sph.nRadius = 0x6ffe;
            break;
        case 4:
            sph.nRadius = 0x10;
            break;
        case 1:
            sph.nRadius = 0x8664 >> 1;
            break;
        case 2:
            capsule = 1;
            break;
        case 3:
            capsule = 1;
            break;
        }
        slot = List_InsertSorted(self + 0x144, 4, 100);
        if (capsule != 0) {
            node = Ov107_Mover_New(&seg);
        } else {
            node = Ov107_CloneResourceTransform(&sph);
        }
        *slot = node;
        ((struct Ov254 *)self)->bodyNodes[k] = node;
        ((struct Ov254 *)self)->bodyShapes[k] = List_InsertSorted(self + 0x22c, 0x10, 100);
        if (capsule != 0) {
            *((struct Ov254 *)self)->bodyShapes[k] = Ov107_Mover_New(&seg);
        } else {
            *((struct Ov254 *)self)->bodyShapes[k] = Ov107_CloneResourceTransform(&sph);
        }
    }
    *(int *)(self + 0x468) = Ov254_HelperD_New(self);
    *(int *)(self + 0x45c) = Ov254_Pillar_New(self);
    *(int *)(self + 0x460) = Ov254_Marker_New(self);
    *(int *)(self + 0x464) = Ov254_TwinMarker_New(self);
    for (i = 0; i < 16; i++) {
        ((struct Ov254 *)self)->shards[i] = Ov254_Helper_New(self);
    }
    for (i = 0; i < 10; i++) {
        ((struct Ov254 *)self)->debris[i] = Ov254_HelperE_New(self);
    }
    Res_RequestIdPair(0x16d);
}

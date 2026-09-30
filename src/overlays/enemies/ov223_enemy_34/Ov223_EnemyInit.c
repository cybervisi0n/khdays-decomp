/* Constructor of the ov223 enemy. Installs the handlers (+8, +0xc, +0x1c message, +0x30, +0x28, +0x2c,
 * +0x34, +0x1d0, +0x1dc, +0x1e0, +0x1e4), the +0x64 pose (scale 2.21), the +0x1fc bounds box (2.0
 * around the feet, 2.0 high) and bit 5 of the +0x60 high byte; builds the +0x384 rig from pose 0
 * (020cfd80 hooks it up, subscribed to +0x9c), binds its +0x388 animation (pose 1, 12 frames),
 * resolves five bones (+0x3d8, +0x3dc, +0x3d4, +0x3e0, +0x3e4) and the +0x3fc bone of pose 0x24;
 * registers action 0/2 lowered 1.1 (rate 1.31), 2/3 lowered 2.21 (0.6), 1/2 (0.8) and 4/2 (1.31);
 * builds the eight sub-items of data_ov223_020d50c4 into the +0x424 pair table (attached, hidden);
 * places five spheres at the origin (radius 0.375, 1.63, 1.19 and twice 1.0, all scaled by 1.1),
 * each reserved in the +0x22c pool (kept in +0x3ac) and the +0x144 pool (kept in +0x3c0), raises
 * bit 1 of the second one, creates the four +0x3ec helpers (020d3e24) and loads sound 0x149. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { int id[8]; } IdTable8;
typedef struct { VecFx32 min; VecFx32 max; } Bounds;
typedef struct { VecFx32 pos; int nRadius; } Sphere;
struct Pair { int res; int handle; };
struct ShapeFlags { unsigned int lo : 8; };
struct Ov223 {
    char pad000[0x3ac];
    int *bodyShapes[5];     /* +0x3ac */
    int bodyNodes[5];       /* +0x3c0 */
    char pad3d4[0x3ec - 0x3d4];
    int helpers[4];         /* +0x3ec */
    char pad3fc[0x424 - 0x3fc];
    struct Pair items[8];   /* +0x424 */
};

extern void Ov223_Destroy(void);
extern void Ov223_TickWithChildRefresh(void);
extern void Ov223_OnEffectMessage(void);
extern void Ov223_CreateAiTask(void);
extern void Ov223_ForwardEventToChildren(void);
extern void Ov223_NotifyPartsThenBase(void);
extern void Ov223_ReleaseHeldSoundsAndTick(void);
extern void Ov223_HandleHit(void);
extern void Ov223_RebindClip(void);
extern void Ov223_RequestSubState14IfNotCurrent(void);
extern void Ov223_RequestSubState15IfIdle(void);
extern void *Ov107_PackTextureHandle(char *self, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern void Ov223_ArmModelCallback(char *self);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void Snd_RegisterSeqAndBind(void *dst, int a, void *b, int n);
extern void MainBlob_ResetSlotRows(int obj, void *block);
extern int FindResourceIndexByName(int item, const char *name);
extern int Ov107_CreateNamedResourceBinding(void *item, const char *name);
extern void Ov107_Actor_SetAttachSlot(char *self, int slot, int a, const VecFx32 *v, int c);
extern void Ov107_EnqueueValue(char *self, int item);
extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern int Ov107_CloneResourceTransform(const Sphere *sphere);
extern int Ov223_Actor_New(char *self);
extern void Res_RequestIdPair(int resourceId);
extern IdTable8 data_ov223_020d50c4;
extern const char data_ov223_020d510c[];
extern const char data_ov223_020d5118[];
extern const char data_ov223_020d5128[];
extern const char data_ov223_020d5134[];
extern const char data_ov223_020d5140[];
extern const char data_ov223_020d514c[];
extern const VecFx32 data_02041dc8;

#define FX_MUL(a, b) ((int)(((long long)(a) * (b) + 0x800) >> 12))

static inline void VEC_Set(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void Ov223_EnemyInit(char *self)
{
    IdTable8 ids = data_ov223_020d50c4;
    Bounds bounds;
    VecFx32 lift;
    Sphere sph;
    VecFx32 origin;
    int i;
    int k;
    int node;
    int *slot;

    bounds.min.x = -0x2000;
    bounds.min.y = 0;
    bounds.min.z = -0x2000;
    bounds.max.x = 0x2000;
    bounds.max.y = 0x2000;
    bounds.max.z = 0x2000;
    *(Callback *)(self + 0x8) = Ov223_Destroy;
    *(Callback *)(self + 0xc) = Ov223_TickWithChildRefresh;
    *(Callback *)(self + 0x1c) = Ov223_OnEffectMessage;
    *(Callback *)(self + 0x30) = Ov223_CreateAiTask;
    *(Callback *)(self + 0x28) = Ov223_ForwardEventToChildren;
    *(Callback *)(self + 0x2c) = Ov223_NotifyPartsThenBase;
    *(Callback *)(self + 0x34) = Ov223_ReleaseHeldSoundsAndTick;
    *(Callback *)(self + 0x1d0) = Ov223_HandleHit;
    *(Callback *)(self + 0x1dc) = Ov223_RebindClip;
    *(Callback *)(self + 0x1e0) = Ov223_RequestSubState14IfNotCurrent;
    *(Callback *)(self + 0x1e4) = Ov223_RequestSubState15IfIdle;
    *(int *)(self + 0x70) = 0x235c;
    VEC_Set((VecFx32 *)(self + 0x64), 0, *(int *)(self + 0x70), 0);
    *(Bounds *)(self + 0x1fc) = bounds;
    {
        u16 hw = *(u16 *)(self + 0x60);

        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x20) << 0x18) >> 0x10);
    }
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    Ov223_ArmModelCallback(self);
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    Snd_RegisterSeqAndBind(self + 0x388, *(int *)(*(int *)(self + 0x384) + 0x88), Ov107_PackTextureHandle(self, 1), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x384), self + 0x388);
    *(int *)(self + 0x3d8) = FindResourceIndexByName(*(int *)(self + 0x384), data_ov223_020d510c);
    *(int *)(self + 0x3dc) = FindResourceIndexByName(*(int *)(self + 0x384), data_ov223_020d5118);
    *(int *)(self + 0x3d4) = FindResourceIndexByName(*(int *)(self + 0x384), data_ov223_020d5128);
    *(int *)(self + 0x3e0) = FindResourceIndexByName(*(int *)(self + 0x384), data_ov223_020d5134);
    *(int *)(self + 0x3e4) = FindResourceIndexByName(*(int *)(self + 0x384), data_ov223_020d5140);
    *(int *)(self + 0x3fc) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(self, 0x24), data_ov223_020d514c);
    lift.x = 0;
    lift.y = -0x11ae;
    lift.z = 0;
    Ov107_Actor_SetAttachSlot(self, 0, 2, &lift, 0x1500);
    lift.x = 0;
    lift.y = -0x235c;
    lift.z = 0;
    Ov107_Actor_SetAttachSlot(self, 2, 3, &lift, 0x99a);
    Ov107_Actor_SetAttachSlot(self, 1, 2, 0, 0xccd);
    Ov107_Actor_SetAttachSlot(self, 4, 2, 0, 0x1500);
    for (i = 0; i < 8; i++) {
        if (ids.id[i] >= 0) {
            if (i < 0) {
                node = CreateSubitemInstance0xB4((void *)ids.id[i]);
            } else {
                node = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ids.id[i]));
            }
            Ov107_EnqueueValue(self, ((struct Ov223 *)self)->items[i].res = node);
            *(int *)(((struct Ov223 *)self)->items[i].res + 0x5c) |= 2;
        }
    }
    origin = data_02041dc8;
    for (k = 0; k < 5; k++) {
        sph.pos = origin;
        switch (k) {
        case 1:
            sph.nRadius = 0x1a00;
            break;
        case 0:
            sph.nRadius = 0x600;
            break;
        case 2:
            sph.nRadius = 0x1300;
            break;
        default:
            sph.nRadius = 0x1000;
            break;
        }
        sph.nRadius = FX_MUL(sph.nRadius, 0x11ae);
        ((struct Ov223 *)self)->bodyShapes[k] = List_InsertSorted(self + 0x22c, 0x10, 100);
        *((struct Ov223 *)self)->bodyShapes[k] = Ov107_CloneResourceTransform(&sph);
        slot = List_InsertSorted(self + 0x144, 4, 100);
        node = Ov107_CloneResourceTransform(&sph);
        *slot = node;
        ((struct Ov223 *)self)->bodyNodes[k] = node;
    }
    ((struct ShapeFlags *)(((struct Ov223 *)self)->bodyShapes[1] + 2))->lo |= 2;
    for (i = 0; i < 4; i++) {
        ((struct Ov223 *)self)->helpers[i] = Ov223_Actor_New(self);
    }
    Res_RequestIdPair(0x149);
}

/* Constructor of the ov276 enemy: installs the handlers (+8 teardown, +0xc draw, +0x1c message,
 * +0x30 registry entry, +0x28/+0x2c veneers, +0x34 tick, +0x1d0 hit, +0x1dc/+0x1e0/+0x1e4
 * callbacks), seeds the +0x1fc bounding box, the +0x64 pose (scale 0x10cc, y 0x10cc) and bit 3
 * of +0x1ae; builds the primary item from pool entry 0 (+0x3a8: joint callback, back-link,
 * subscribed), binds pool entry 1 as its 12-slot animation set (+0x384), resolves the two named
 * joints (+0x3b4/+0x3b8), refreshes its callbacks and resets the +0x3c0/+0x444 and two +0x3ec
 * hit shapes; keeps the named motion handle from pool entry 0x17 (+0x470); creates the six
 * sub-items listed by the overlay's +0x2b88 table (+0x488: the first from the thread's +0x88
 * pool, the rest from the actor pool; attached, bit 1, polygon ids 3..0x1e for the last five),
 * registers three reactions (2/3 with a -0x10cc lift, 1/2 and 4/2) and two placements on the
 * +0x22c/+0x144 lists (+0x3ac/+0x3b0) at the zero vector, then loads sound 0x164. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Box {
    int xmin, ymin, zmin;
    int xmax, ymax, zmax;
};

struct PoolIds {
    int id[6];
};

struct Pose {
    VecFx32 pos;
    int scale;
};

struct Lift {
    int a;
    int b;
    int c;
};

struct SubSlot {
    int item;
    int pad4;
};

struct HitShape {
    int w[11];
};

struct Ov276Actor {
    char pad000[0x64];
    int camera[4];
    char pad074[0x144 - 0x74];
    char pool144[0x3b0 - 0x144];
    int poolValue3b0;
    char pad3b4[0x3ec - 0x3b4];
    struct HitShape shapes[2];
    char pad444[0x488 - 0x444];
    struct SubSlot subs[6];
};

extern void Ov276_FullTeardownReleaseSlots(void);
extern void Ov276_DrawHook(void);
extern void Ov276_HandleMessage(void);
extern void Ov276_CreateNodeRegistryEntry(void);
extern void func_ov276_020d0194(void);
extern void func_ov276_020d01a0(void);
extern void Ov276_TickHook(void);
extern void Ov276_OnHit(void);
extern void Ov276_RebindModelForVariant(void);
extern void Ov276_RequestSubState8IfNotAlready(void);
extern void Ov276_RequestSubState9IfIdle(void);
extern void Ov276_JointCallback(void);
extern void *Ov107_PackTextureHandle(int pool, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void Snd_RegisterSeqAndBind(void *set, int model, void *pool, int count);
extern void MainBlob_ResetSlotRows(int item, void *set);
extern int FindResourceIndexByName(int item, const char *name);
extern void RefreshObjectCallbacks(int item, int a);
extern void SrtTransform_SetIdentity(void *shape);
extern int Ov107_CreateNamedResourceBinding(void *item, const char *name);
extern int *Ov107_GetActorManager(void);
extern void Ov107_EnqueueValue(int self, int item);
extern void NNS_G3dMdlSetMdlPolygonID(void *model, int a, int id);
extern void Ov107_Actor_SetAttachSlot(int self, int a, int b, struct Lift *lift, int id);
extern int *List_InsertSorted(void *list, int stride, int max);
extern int Ov107_CloneResourceTransform(void *pose);
extern void Res_RequestIdPair(int id);
extern const struct PoolIds data_ov276_020d2b88;
extern const char data_ov276_020d2c2c[];
extern const char data_ov276_020d2c3c[];
extern const char data_ov276_020d2c4c[];
extern const VecFx32 data_02041dc8;

static inline void VecSetP_(VecFx32 *v, int x, int y, int z) { v->x = x; v->y = y; v->z = z; }

void Ov276_EnemyConstruct(char *self)
{
    struct PoolIds pools;
    struct Box box;
    struct Pose pose;
    struct Lift lift;
    struct Ov276Actor *actor;
    int *p;
    int handle;       /* the actor as the int handle the ov107 calls take */
    int i;
    int polyId;
    int value;

    pools = data_ov276_020d2b88;
    box.xmin = -0x1c7c;
    box.ymin = 0;
    box.zmin = -0xa44;
    box.xmax = box.xmin + 0x38f9;
    box.ymax = box.ymin + 0x20a8;
    box.zmax = box.zmin + 0x107a;
    *(void **)(self + 0x8) = Ov276_FullTeardownReleaseSlots;
    *(void **)(self + 0xc) = Ov276_DrawHook;
    handle = (int)self;
    *(void **)(self + 0x1c) = Ov276_HandleMessage;
    *(void **)(self + 0x30) = Ov276_CreateNodeRegistryEntry;
    *(void **)(self + 0x28) = func_ov276_020d0194;
    *(void **)(self + 0x2c) = func_ov276_020d01a0;
    *(void **)(self + 0x34) = Ov276_TickHook;
    *(void **)(self + 0x1d0) = Ov276_OnHit;
    *(void **)(self + 0x1dc) = Ov276_RebindModelForVariant;
    *(void **)(self + 0x1e0) = Ov276_RequestSubState8IfNotAlready;
    *(void **)(self + 0x1e4) = Ov276_RequestSubState9IfIdle;
    *(struct Box *)(self + 0x1fc) = box;
    *(int *)(self + 0x70) = 0x10cc;
    VecSetP_((VecFx32 *)(self + 0x64), 0, *(int *)(self + 0x70), 0);
    *(u16 *)(self + 0x100 + 0xae) |= 8;
    polyId = 3;
    *(int *)(self + 0x3a8) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(handle, 0));
    *(void **)(*(int *)(self + 0x3a8) + 0x74) = Ov276_JointCallback;
    *(char **)(*(int *)(self + 0x3a8) + 0x84) = self;
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x3a8));
    Snd_RegisterSeqAndBind(self + 0x384, *(int *)(*(int *)(self + 0x3a8) + 0x88), Ov107_PackTextureHandle(handle, 1), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x3a8), self + 0x384);
    *(int *)(self + 0x3b4) = FindResourceIndexByName(*(int *)(self + 0x3a8), data_ov276_020d2c2c);
    *(int *)(self + 0x3b8) = FindResourceIndexByName(*(int *)(self + 0x3a8), data_ov276_020d2c3c);
    RefreshObjectCallbacks(*(int *)(self + 0x3a8), 0);
    SrtTransform_SetIdentity(self + 0x3c0);
    SrtTransform_SetIdentity(self + 0x444);
    actor = (struct Ov276Actor *)self;
    for (i = 0; i < 2; i++) {
        SrtTransform_SetIdentity(&actor->shapes[i]);
    }
    *(int *)(self + 0x470) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(handle, 0x17), data_ov276_020d2c4c);
    for (i = 0; i < 6; i++) {
        if (i < 1) {
            int *os = Ov107_GetActorManager();
            unsigned int kind = pools.id[i] & 0x1ff;
            unsigned int addr = (os[0x22] + 0x8000) & 0x00fffffc;
            addr = addr << 7;
            addr = addr | 0x80000000;
            value = CreateSubitemInstance0xB4((void *)(kind | addr));
        } else {
            value = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(handle, pools.id[i]));
        }
        Ov107_EnqueueValue(handle, (actor->subs[i].item = value));
        *(int *)(actor->subs[i].item + 0x5c) |= 2;
        if (i >= 1) {
            NNS_G3dMdlSetMdlPolygonID(*(void **)(*(int *)(actor->subs[i].item + 0x88) + 0x78), 0, polyId);
            polyId++;
            if (polyId >= 0x1f) {
                polyId = 3;
            }
        }
    }
    lift.c = lift.a = 0;
    lift.b = -0x10cc;
    Ov107_Actor_SetAttachSlot(handle, 2, 3, &lift, 0x6b8);
    Ov107_Actor_SetAttachSlot(handle, 1, 2, 0, 0x8f6);
    Ov107_Actor_SetAttachSlot(handle, 4, 2, 0, 0xb33);
    pose.pos = data_02041dc8;
    pose.scale = 0x10cc;
    *(int **)(self + 0x3ac) = List_InsertSorted(self + 0x22c, 0x10, 0x64);
    **(int **)(self + 0x3ac) = Ov107_CloneResourceTransform(&pose);
    p = List_InsertSorted(actor->pool144, 4, 0x64);
    value = (*p = Ov107_CloneResourceTransform(&pose));
    actor->poolValue3b0 = value;
    Res_RequestIdPair(0x164);
}

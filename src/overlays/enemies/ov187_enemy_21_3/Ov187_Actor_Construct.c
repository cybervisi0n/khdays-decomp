
#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

struct Obj;
typedef void (*ObjCallback)(struct Obj *self);

struct CameraWork {
    VecFx32 vector;
    int scalar;
};

struct ConfigContainer {
    char pad00[0x78];
    void *config78;
};

struct Subscriber {
    char pad00[0x5c];
    unsigned int flags5c;
};

struct CreatedItem {
    char pad00[0x88];
    struct ConfigContainer *container88;
};

struct PoolEntry {
    int value;
};

struct RollingCounter {
    u8 value;
};

struct Obj {
    char pad00[0x08];
    ObjCallback fn08;
    ObjCallback fn0c;
    char pad10[0x0c];
    ObjCallback fn1c;
    char pad20[0x08];
    ObjCallback fn28;
    ObjCallback fn2c;
    ObjCallback fn30;
    ObjCallback fn34;
    char pad38[0x28];
    u16 flags60;
    char pad62[0x02];
    int camera64[4];
    char pad74[0x28];
    struct Subscriber *subscriber9c;
    char padA0[0xa4];
    char pool144[0x6a];
    u16 flags1ae;
    char pad1b0[0x20];
    ObjCallback fn1d0;
    char pad1d4[0x08];
    ObjCallback fn1dc;
    ObjCallback fn1e0;
    char pad1e4[0x08];
    ObjCallback fn1ec;
    ObjCallback fn1f0;
    char pad1f4[0x04];
    ObjCallback fn1f8;
    char pad1fc[0x30];
    char pool22c[0x158];
    struct CreatedItem *subitem384;
    struct PoolEntry *poolEntry388;
    int poolValue38c;
    void **entities390;
    char spawner394[0x14];
};

extern const struct CameraWork data_ov187_020d70f0;
extern struct RollingCounter data_ov187_020d7120;

extern void Ov187_Destroy(struct Obj *self);
extern void Ov187_TickAndSyncTwoModelXforms(struct Obj *self);
extern void Ov187_CreateAiTask(struct Obj *self);
extern void Ov187_NotifySubNodesRunCallbacks(struct Obj *self);
extern void Ov187_PingAllSubNodes(struct Obj *self);
extern void Ov187_EventCreateDestroySubObject(struct Obj *self);
extern void Ov187_CancelPendingRequest(struct Obj *self);
extern void Ov187_ApplyHitReaction(struct Obj *self);
extern void Ov187_DriveExitModes(struct Obj *self);
extern void Ov187_RetuneRigKindSlots03(struct Obj *self);
extern void Ov187_NotifyAndDoubleAngle(struct Obj *self);
extern void Ov187_RunSubNodeCallbacksArg(struct Obj *self);
extern void Ov187_RunSubNodeCallbacksClose(struct Obj *self);

extern void *Ov107_PackTextureHandle(struct Obj *self, int index);
extern struct CreatedItem *CreateSubitemInstance0xB4(void *item);
extern void NNS_G3dMdlSetMdlPolygonID(void *config, unsigned int index, unsigned int variant);
extern void Ov107_Actor_SetAttachSlot(struct Obj *, int, unsigned int, int, int);
extern int Ov107_CloneResourceTransform(void *camera);
extern void Ov107_LoadSpawnRecord(int kind, void *spawner);
extern void **CallocInstance(int size);
extern void *Ov187_CreateNamedEntity(void *spawner);

/*
 * Constructor for the ov185 actor.
 *
 * Installs the thirteen handler entries, raises bit 6 of the actor flag byte,
 * seeds the camera work record from the overlay spawn table, raises the ring
 * flag and the subscriber flag, then creates the subitem, registers it, and
 * configures slots 0, 2 and 3 with a rolling sequence number that advances to
 * 0x1e and then wraps back to 3.
 *
 * It then reserves two pool slots and takes a camera handle for each, loads the
 * parameter table at 0x394 from the archive, allocates the four entry child
 * table at 0x390, creates the four child entities from that parameter table,
 * and announces resource 0x120.
 *
 * The 0x2000 is a genuine fifth stacked argument of the spawn call here, unlike
 * in the smaller sibling at 020cfa2c where the same store is a field of a local
 * frame.  The two cases are told apart by where the store lands: as an argument
 * it is call setup and sits immediately before the branch, and as a field store
 * the scheduler is free to hoist it above the surrounding work.
 */
void Ov187_Actor_Construct(struct Obj *self)
{
    struct CameraWork work;
    struct PoolEntry *slot;
    int result;
    int i;
    u16 v;

    work = data_ov187_020d70f0;
    self->fn08 = Ov187_Destroy;
    self->fn0c = Ov187_TickAndSyncTwoModelXforms;
    self->fn30 = Ov187_CreateAiTask;
    self->fn28 = Ov187_NotifySubNodesRunCallbacks;
    self->fn2c = Ov187_PingAllSubNodes;
    self->fn1c = Ov187_EventCreateDestroySubObject;
    self->fn34 = Ov187_CancelPendingRequest;
    self->fn1d0 = Ov187_ApplyHitReaction;
    self->fn1e0 = Ov187_DriveExitModes;
    self->fn1dc = Ov187_RetuneRigKindSlots03;
    self->fn1ec = Ov187_NotifyAndDoubleAngle;
    self->fn1f0 = Ov187_RunSubNodeCallbacksArg;
    self->fn1f8 = Ov187_RunSubNodeCallbacksClose;

    v = self->flags60;
    self->flags60 = (u16)((v & ~0xff00)
                          | ((((((unsigned int)v << 0x10) >> 0x18) | 0x40)
                              << 0x18) >> 0x10));
    *(struct CameraWork *)self->camera64 = work;
    self->flags1ae |= 0x10;
    self->subscriber9c->flags5c |= 4;

    self->subitem384 = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    RegisterSubscriberSlot(self->subscriber9c, self->subitem384);
    NNS_G3dMdlSetMdlPolygonID(self->subitem384->container88->config78, 0,
                  data_ov187_020d7120.value);
    NNS_G3dMdlSetMdlPolygonID(self->subitem384->container88->config78, 2,
                  data_ov187_020d7120.value);
    NNS_G3dMdlSetMdlPolygonID(self->subitem384->container88->config78, 3,
                  data_ov187_020d7120.value);

    data_ov187_020d7120.value = data_ov187_020d7120.value + 1;
    if (data_ov187_020d7120.value >= 0x1f) {
        data_ov187_020d7120.value = 3;
    }
    Ov107_Actor_SetAttachSlot(self, 2, 2, 0, 0x2000);

    self->poolEntry388 = (struct PoolEntry *)List_InsertSorted(self->pool22c, 0x10, 0x64);
    self->poolEntry388->value = Ov107_CloneResourceTransform(&work);

    slot = (struct PoolEntry *)List_InsertSorted(self->pool144, 4, 0x64);
    result = (slot->value = Ov107_CloneResourceTransform(&work));
    self->poolValue38c = result;

    Ov107_LoadSpawnRecord(2, self->spawner394);
    self->entities390 = CallocInstance(0x10);
    for (i = 0; i < 4; i++) {
        self->entities390[i] = Ov187_CreateNamedEntity(self->spawner394);
    }
    Res_RequestIdPair(0x120);
}

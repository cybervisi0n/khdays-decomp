/* Initialises the enemy actor: installs its handlers, camera and flags, creates its models and
 * attach slots, and requests its resources. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

struct Obj;
typedef void (*ObjCallback)(struct Obj *self);

struct Vec4 {
    int x;
    int y;
    int z;
    int w;
};

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

struct OutgoingFrame {
    unsigned int outgoing;
    struct CameraWork work;
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
    char pad20[0x10];
    ObjCallback fn30;
    ObjCallback fn34;
    char pad38[0x2c];
    int camera64[4];
    char pad74[0x28];
    struct Subscriber *subscriber9c;
    char padA0[0xa4];
    char pool144[0x6a];
    u16 flags1ae;
    char pad1b0[0x20];
    ObjCallback fn1d0;
    ObjCallback fn1d4;
    char pad1d8[0x04];
    ObjCallback fn1dc;
    ObjCallback fn1e0;
    char pad1e4[0x10];
    int field1f4;
    char pad1f8[0x34];
    char pool22c[0x158];
    struct CreatedItem *subitem384;
    struct PoolEntry *poolEntry388;
    int poolValue38c;
};

extern const struct CameraWork data_ov118_020d1874;
extern struct RollingCounter data_ov118_020d18a0;

extern void Ov118_OnDespawn(struct Obj *self);
extern void Ov118_TickAndSyncTwoModelXforms(struct Obj *self);
extern void Ov118_SpawnAuraOnTag5(struct Obj *self);
extern void Ov118_CallSetupThenSharedHandler(struct Obj *self);
extern void Ov118_PlayAnimPair(struct Obj *self);
extern void Ov118_RunHandlerUnlessBit0OnlyThenAdvance(struct Obj *self);
extern void Ov118_CreateRegistryEntryForActor(struct Obj *self);
extern void Ov118_TickStaggerAndFlipFacing(struct Obj *self);
extern void Ov118_ReactionRequestSubState8(struct Obj *self);

extern void *Ov107_PackTextureHandle(struct Obj *self, int index);
extern struct CreatedItem *CreateSubitemInstance0xB4(void *item);
extern void NNS_G3dMdlSetMdlPolygonID(void *config, unsigned int index,
                          unsigned int variant);
extern void Ov107_Actor_SetAttachSlot(struct Obj *, int, unsigned int, struct Vec4 *);
extern int Ov107_CloneResourceTransform(void *camera);

void Ov118_InitEffectActor(struct Obj *self)
{
    struct OutgoingFrame frame;
    frame.work = data_ov118_020d1874;
    struct PoolEntry *slot;
    int result;
    u8 capacity;
    self->fn08 = Ov118_OnDespawn;
    self->fn0c = Ov118_TickAndSyncTwoModelXforms;
    self->fn1c = Ov118_SpawnAuraOnTag5;
    self->fn30 = Ov118_CreateRegistryEntryForActor;
    self->fn34 = Ov118_RunHandlerUnlessBit0OnlyThenAdvance;
    self->fn1d4 = Ov118_CallSetupThenSharedHandler;
    self->fn1d0 = Ov118_TickStaggerAndFlipFacing;
    self->fn1e0 = Ov118_ReactionRequestSubState8;
    self->fn1dc = Ov118_PlayAnimPair;

    self->field1f4 = 0;
    *(struct CameraWork *)self->camera64 = frame.work;
    self->flags1ae |= 0x10;
    self->subscriber9c->flags5c |= 4;

    self->subitem384 = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    RegisterSubscriberSlot(self->subscriber9c, self->subitem384);
    NNS_G3dMdlSetMdlPolygonID(self->subitem384->container88->config78, 3,
                  data_ov118_020d18a0.value);

    data_ov118_020d18a0.value = (data_ov118_020d18a0.value + 1) & 0x1f;
    frame.outgoing = 0x1000;
    Ov107_Actor_SetAttachSlot(self, 2, 2, 0);

    self->poolEntry388 = (struct PoolEntry *)List_InsertSorted(self->pool22c, 0x10, 0x64);

    {
        result = Ov107_CloneResourceTransform(&frame.work);
        capacity = 0x64;
        self->poolEntry388->value = result;

        slot = (struct PoolEntry *)List_InsertSorted(self->pool144, 4, capacity);
        result = (slot->value = Ov107_CloneResourceTransform(&frame.work));
        self->poolValue38c = result;

        Res_RequestIdPair(0x120);
    }
}


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

extern const struct CameraWork data_ov187_020d710c;
extern struct RollingCounter data_ov187_020d7130;

extern void Ov187_OnDespawn(struct Obj *self);
extern void Ov187_TickAndSyncChildren(struct Obj *self);
extern void Ov187_SpawnAuraOnTag5(struct Obj *self);
extern void Ov187_CreateRegistryEntryForActor(struct Obj *self);
extern void Ov187_RunHandlerUnlessBit0OnlyThenAdvance(struct Obj *self);
extern void Ov187_CallSetupThenSharedHandler(struct Obj *self);
extern void Ov187_TickStaggerAndFlipFacing(struct Obj *self);
extern void Ov187_ReactionRequestSubState8(struct Obj *self);
extern void Ov187_PlayAnimPair(struct Obj *self);

extern void *Ov107_PackTextureHandle(struct Obj *self, int index);
extern struct CreatedItem *CreateSubitemInstance0xB4(void *item);
extern void NNS_G3dMdlSetMdlPolygonID(void *config, unsigned int index, unsigned int variant);
extern void Ov107_Actor_SetAttachSlot(struct Obj *, int, unsigned int, int);
extern int Ov107_CloneResourceTransform(void *camera);

/*
 * Constructor for the ov185 actor.
 *
 * Installs the nine handler entries, clears the counter, seeds the camera work
 * record from the overlay spawn table, raises the ring flag and the subscriber
 * flag, then creates the subitem, registers it, and configures its slot with a
 * rolling sequence number that advances modulo 32.  Finally it reserves two
 * pool slots, takes a camera handle for each, and announces the second as
 * resource 0x120.
 *
 * The 0x1000 written just before the spawn call is a field of the local frame,
 * not a fifth argument.  The frame begins at the outgoing argument slot, so the
 * callee does read the value there, but in the source it is an ordinary field
 * store and the call takes four parameters.  The store survives because the
 * frame's other field is address-taken, being handed to the camera index helper
 * twice.  Written as an argument instead, the compiler treats it as call setup,
 * delays it, and the whole surrounding window comes out permuted.
 */
void Ov187_InitEffectActor(struct Obj *self)
{
    struct OutgoingFrame frame;
    frame.work = data_ov187_020d710c;
    struct PoolEntry *slot;
    int result;
    u8 capacity;
    self->fn08 = Ov187_OnDespawn;
    self->fn0c = Ov187_TickAndSyncChildren;
    self->fn1c = Ov187_SpawnAuraOnTag5;
    self->fn30 = Ov187_CreateRegistryEntryForActor;
    self->fn34 = Ov187_RunHandlerUnlessBit0OnlyThenAdvance;
    self->fn1d4 = Ov187_CallSetupThenSharedHandler;
    self->fn1d0 = Ov187_TickStaggerAndFlipFacing;
    self->fn1e0 = Ov187_ReactionRequestSubState8;
    self->fn1dc = Ov187_PlayAnimPair;

    self->field1f4 = 0;
    *(struct CameraWork *)self->camera64 = frame.work;
    self->flags1ae |= 0x10;
    self->subscriber9c->flags5c |= 4;

    self->subitem384 = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    RegisterSubscriberSlot(self->subscriber9c, self->subitem384);
    NNS_G3dMdlSetMdlPolygonID(self->subitem384->container88->config78, 3,
                  data_ov187_020d7130.value);

    data_ov187_020d7130.value = (data_ov187_020d7130.value + 1) & 0x1f;
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

/* Initialises the enemy actor: installs its handlers, hit box and flags, creates its models and
 * attach slots, and requests its resources. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Box {
    VecFx32 min;
    VecFx32 max;
};

static inline void Vec3_Set(VecFx32 *out, int x, int y, int z)
{
    out->x = x;
    out->y = y;
    out->z = z;
}

struct Subitem {
    char pad00[0x5c];
    unsigned int flags5c;
    char pad60[0x0c];
    void (*callback6c)(void);
    char pad70[0x04];
    void (*callback74)(void);
    char pad78[0x0c];
    void *owner84;
};

struct Obj {
    char pad00[0x08];
    void (*fn08)(void);
    void (*fn0c)(void);
    char pad10[0x0c];
    void (*fn1c)(void);
    char pad20[0x08];
    void (*fn28)(void);
    void (*fn2c)(void);
    void (*fn30)(void);
    void (*fn34)(void);
    char pad38[0x28];
    u16 flags60;
    char pad62[0x02];
    int camera[4];
    char pad74[0x28];
    void *subscriberList9c;
    char padA0[0xa4];
    char pool144[0x8c];
    void (*fn1d0)(void);
    char pad1d4[0x08];
    void (*fn1dc)(void);
    void (*fn1e0)(void);
    void (*fn1e4)(void);
    char pad1e8[0x14];
    struct Box box;
    char pad214[0x18];
    char pool22c[0x158];
    struct Subitem *subitem384;
    struct Subitem *subitem388;
    int *poolEntry38c;
    int poolValue390;
    char pad394[0x04];
    int child398[3];
    char pad3a4[0x18];
    int angle3bc;
    int quaternion3c0[4];
    int resourceId3d0;
    int resourceId3d4;
    char pad3d8[0x18];
    struct Subitem *subitem3f0;
};

extern void Ov199_ReleaseSubObjectsThenNotify(void);
extern void Ov199_ResetSetupAndPropagate(void);
extern void Ov199_IssueSpawnCommand(void);
extern void Ov199_SpawnActorRegistryEntry(void);
extern void Ov199_AttachThreeSubNodesThenFinalize(void);
extern void Ov199_AttachThreeSubNodesThenFinalize_2(void);
extern void Ov199_ReleaseAttachmentsOnStop(void);
extern void Ov199_ResolveHitReaction(void);
extern void Ov199_Model_ReapplyTrack0(void);
extern void Ov199_RequestSubState7IfNotCurrent(void);
extern void Ov199_RequestSubState8IfIdle(void);
extern void Ov199_QueryAndCopyVecIfHit(void);
extern void Ov199_TickSwingArc(void);
extern int Ov199_AllocLinkChild3a4(struct Obj *self);

extern char data_ov199_020d614c[];
extern char data_ov199_020d6154[];
extern VecFx32 data_02042264;

extern void *Ov107_PackTextureHandle(struct Obj *self, int index);
extern struct Subitem *CreateSubitemInstance0xB4(void *item);
extern void RegisterSubscriberSlot(void *list, struct Subitem *item);
extern int FindResourceIndexByName(struct Subitem *item, char *name);
extern void RefreshObjectCallbacks(struct Subitem *item, int value);
extern void QuatFromAxisAngle(int *out, VecFx32 *axis, int angle);
extern void Ov107_Actor_SetAttachSlot(struct Obj *self, int index, int a, int b, int scale);
extern void Ov107_EnqueueValue(struct Obj *self, struct Subitem *item);
extern int *List_InsertSorted(void *pool, int size, int count);
extern int Ov107_CloneResourceTransform(void *camera);
extern void Res_RequestIdPair(int id);

void Ov199_InitActor(struct Obj *self)
{
    struct Box box;
    struct Subitem *item;
    int *slot;
    int value;
    int i;
    unsigned int flags;

    self->fn08 = Ov199_ReleaseSubObjectsThenNotify;
    self->fn0c = Ov199_ResetSetupAndPropagate;
    self->fn1c = Ov199_IssueSpawnCommand;
    self->fn30 = Ov199_SpawnActorRegistryEntry;
    self->fn28 = Ov199_AttachThreeSubNodesThenFinalize;
    self->fn2c = Ov199_AttachThreeSubNodesThenFinalize_2;
    self->fn34 = Ov199_ReleaseAttachmentsOnStop;
    self->fn1d0 = Ov199_ResolveHitReaction;
    self->fn1dc = Ov199_Model_ReapplyTrack0;
    self->fn1e0 = Ov199_RequestSubState7IfNotCurrent;
    self->fn1e4 = Ov199_RequestSubState8IfIdle;

    box.min.x = -0x1800;
    box.min.z = -0x1800;
    box.min.y = 0;
    box.max.x = 0x1800;
    box.max.y = 0x1800;
    box.max.z = 0x1800;

    flags = self->flags60;
    self->flags60 = flags & ~0xff00 |
        (((((flags << 0x10) >> 0x18) | 0x40) << 0x18) >> 0x10);

    self->camera[3] = 0x1a00;
    self->camera[0] = 0;
    self->camera[1] = 0x1a00;
    self->camera[2] = 0;

    self->box = box;

    self->subitem384 = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    self->subitem384->callback74 = Ov199_QueryAndCopyVecIfHit;
    self->subitem384->owner84 = self;
    RegisterSubscriberSlot(self->subscriberList9c, self->subitem384);
    self->resourceId3d4 = FindResourceIndexByName(self->subitem384, data_ov199_020d614c);

    (self->subitem388 = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 1)))->callback6c = Ov199_TickSwingArc;
    self->subitem388->owner84 = self;
    RefreshObjectCallbacks(self->subitem388, 0);
    self->resourceId3d0 = FindResourceIndexByName(self->subitem388, data_ov199_020d6154);

    self->angle3bc = 0x10c1;
    QuatFromAxisAngle(self->quaternion3c0, &data_02042264, self->angle3bc);

    Ov107_Actor_SetAttachSlot(self, 0, 1, 0, 0x3000);
    Ov107_Actor_SetAttachSlot(self, 2, 1, 0, 0x3000);
    Ov107_Actor_SetAttachSlot(self, 1, 1, 0, 0x3000);
    Ov107_Actor_SetAttachSlot(self, 4, 1, 0, 0x3000);

    item = self->subitem3f0 = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 4));
    Ov107_EnqueueValue(self, item);
    item->flags5c |= 2;

    self->poolEntry38c = List_InsertSorted(self->pool22c, 0x10, 0x64);
    *self->poolEntry38c = Ov107_CloneResourceTransform(self->camera);

    slot = List_InsertSorted(self->pool144, 4, 0x64);
    value = (*slot = Ov107_CloneResourceTransform(self->camera));
    self->poolValue390 = value;

    for (i = 0; i < 3; i++) {
        self->child398[i] = Ov199_AllocLinkChild3a4(self);
    }

    Res_RequestIdPair(0x130);
}

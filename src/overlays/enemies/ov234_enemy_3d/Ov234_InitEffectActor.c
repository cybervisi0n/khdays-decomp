/* Initialises the enemy actor: installs its handlers and hit box, creates its model and attach
 * slots, and requests its resources. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Ov234Box {
    VecFx32 min;
    VecFx32 max;
};

struct Ov234Work {
    VecFx32 vec;
    int scale;
};

struct Ov234TextureTable {
    int offsets[1];
};

struct Ov234InitFrame {
    volatile int textureScratch;
    struct Ov234Box box;
};

struct Ov234Actor {
    char pad000[0x08];
    void (*fn008)(void);
    void (*fn00c)(void);
    char pad010[0x0c];
    void (*fn01c)(void);
    char pad020[0x08];
    void (*fn028)(void);
    void (*fn02c)(void);
    void (*fn030)(void);
    char pad034[0x2c];
    u16 flags060;
    char pad062[0x02];
    int camera[4];
    char pad074[0x28];
    void *resource09c;
    char pad0a0[0xa4];
    char pool144[0x6a];
    u16 flags1ae;
    char pad1b0[0x19];
    u8 state1c9;
    char pad1ca[0x06];
    void (*fn1d0)(void);
    char pad1d4[0x08];
    void (*fn1dc)(void);
    char pad1e0[0x1c];
    struct Ov234Box box1fc;
    char pad214[0x18];
    char pool22c[0x158];
    char *handle384;
    int *slot388;
    int handle38c;
    char pad390[0x04];
    char work394[0x24];
    int value3b8;
    char *handle3bc;
};

extern void Ov234_Destroy(void);
extern void Ov234_PropagateBlockToLinkedNodes(void);
extern void Ov234_PlaceActorWithTransform(void);
extern void Ov234_Model_ReapplyTrack0(void);
extern void Ov234_CreateNodeRegistryEntry(void);
extern void Ov234_ResolveHitReaction(void);
extern void func_ov234_020cc25c(void);
extern void func_ov234_020cc268(void);

extern const struct Ov234TextureTable data_ov234_020cd100;
extern unsigned Ov107_PackTextureHandle();
extern char *CreateSubitemInstance0xB4(unsigned);
extern void RegisterSubscriberSlot(void *, char *);
extern void MainBlob_ResetSlotRows(char *, void *);
extern void Srt_SetTranslationXYZ(void *, int, int, int);
extern void Srt_SetScaleUniform(void *, int);
extern void Ov107_EnqueueValue(struct Ov234Actor *, int);
extern int *List_InsertSorted(void *, int, int);
extern int Ov107_CloneResourceTransform(void *);
extern void Res_RequestIdPair(int nId);

void Ov234_InitEffectActor(struct Ov234Actor *arg0)
{
    struct Ov234Actor *self;
    int minY = 0x17;
    int minX = 0xfffff116;
    int minZ = 0xfffff7a8;
    struct Ov234TextureTable textureTable = data_ov234_020cd100;
    struct Ov234Work work;
    struct Ov234Box box;
    int *slot;

#pragma opt_dead_assignments off
    box.min.x = 1;
#pragma opt_dead_assignments on
    self = arg0;

    box.min.x = minX;
    box.min.y = minY;
    box.min.z = minZ;
    box.max.x = box.min.x + 0x1dd3;
    box.max.y = box.min.y + 0x1c27;
    box.max.z = box.min.z + 0xd64;

    self->fn008 = Ov234_Destroy;
    self->fn00c = Ov234_PropagateBlockToLinkedNodes;
    self->fn01c = Ov234_PlaceActorWithTransform;
    self->fn030 = Ov234_CreateNodeRegistryEntry;
    self->fn028 = func_ov234_020cc25c;
    self->fn02c = func_ov234_020cc268;
    self->fn1d0 = Ov234_ResolveHitReaction;
    self->fn1dc = Ov234_Model_ReapplyTrack0;

    self->state1c9 = 2;
    self->flags1ae |= 4;
    {
        unsigned value = self->flags060;
        self->flags060 = (u16)((value & ~0xff00) |
            (((((value << 0x10) >> 0x18) | 0xa0) << 0x18) >> 0x10));
    }

    {
        self->camera[3] = 0x700;
        self->box1fc = box;

        self->handle384 =
            CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
        RegisterSubscriberSlot(self->resource09c, self->handle384);
        MainBlob_ResetSlotRows(self->handle384, self->work394);
        Srt_SetTranslationXYZ(self->handle384 + 4, 0, 0x380, 0);
        Srt_SetScaleUniform((char *)self + 0xa0, 0xccd);

        self->handle3bc = CreateSubitemInstance0xB4(
            Ov107_PackTextureHandle(self, textureTable.offsets[0]));
        Ov107_EnqueueValue(self, (int)self->handle3bc);
        *(int *)(self->handle3bc + 0x5c) |= 2;
    }

    work.vec.x = 0;
    work.vec.y = 0;
    work.vec.z = 0;
    work.scale = 0x1200;

    self->slot388 = List_InsertSorted(self->pool22c, 0x10, 0x64);
    *self->slot388 = Ov107_CloneResourceTransform(&work);

    slot = List_InsertSorted(self->pool144, 4, 0x64);
    *slot = Ov107_CloneResourceTransform(&self->camera[0]);
    self->handle38c = *slot;

    self->value3b8 = 0;
    Res_RequestIdPair(0x178);
}

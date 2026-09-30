/* Constructor of the ov213 enemy's companion (x2 with ov273): installs the handlers (+8, +0xc
 * draw, +0x1c message, +0x30, +0x1dc), raises flags 0x1d in +0x1ae and 0x64 in the +0x60 high
 * byte, sets the +0x1c9 group to 2, clears +0x54/+0x58 and places the +0x64 pose at the origin
 * with scale 0.25. Builds the +0x388 and +0x38c rigs from poses 0x1c and 0x3c of the +0x384 pool
 * (both subscribed to +0x9c), a one-slot +0x3a8 table holding the sub-item of the pool's shared
 * pose (data_ov213_020d2f28; registered, bit 1 of +0x5c), and reserves the +0x144 collision handle
 * (+0x390) from a capsule at the origin along -z (radius 1.82, height 0.31). The shared pose
 * id is read first and kept in the frame across the rig construction. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 pos; VecFx32 up; int radius; int height; } Capsule;

extern unsigned Ov107_PackTextureHandle(int pool, int kind);
extern int CreateSubitemInstance0xB4(unsigned res);
extern void RegisterSubscriberSlot(int list, int obj);
extern void *CallocInstance(int size);
extern void Ov107_EnqueueValue(char *self, int obj);
extern int *List_InsertSorted(void *list, int stride, int max);
extern int Ov107_Mover_New(Capsule *capsule);
extern int data_ov213_020d2f28;
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042258;
extern void Ov213_Companion_Destroy(void);
extern void Ov213_PushPose(void);
extern void Ov213_CmdSpawnChildAtOffsetB(void);
extern void Ov213_CreateRegistryEntryAndLink(void);
extern void Ov213_RearmHitSlots(void);

void Ov213_CompanionConstruct(char *self)
{
    volatile int shared;
    VecFx32 zero;
    Capsule capsule;
    u16 v;
    int *p;
    int h;

    *(void **)(self + 8) = (void *)Ov213_Companion_Destroy;
    *(void **)(self + 0xc) = (void *)Ov213_PushPose;
    *(void **)(self + 0x1c) = (void *)Ov213_CmdSpawnChildAtOffsetB;
    *(void **)(self + 0x30) = (void *)Ov213_CreateRegistryEntryAndLink;
    *(void **)(self + 0x1dc) = (void *)Ov213_RearmHitSlots;
    *(u16 *)(self + 0x1ae) |= 0x1d;
    shared = data_ov213_020d2f28;
    v = *(u16 *)(self + 0x60);
    *(u16 *)(self + 0x60) = (u16)((v & ~0xff00) | (((((unsigned int)v << 0x10) >> 0x18 | 0x64) << 0x18) >> 0x10));
    *(unsigned char *)(self + 0x1c9) = 2;
    *(int *)(self + 0x54) = 0;
    *(int *)(self + 0x58) = 0;
    zero = data_02041dc8;
    *(VecFx32 *)(self + 0x64) = zero;
    /* default scale first: the overwritten store is dropped after scheduling but spends the
     * block's scheduling budget, which keeps the ROM's capsule stores ahead of the fca8 call */
    *(int *)(self + 0x70) = 0x1000;
    *(int *)(self + 0x70) = 0x400;
    *(int *)(self + 0x388) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(*(int *)(self + 0x384), 0x1c));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x388));
    *(int *)(self + 0x38c) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(*(int *)(self + 0x384), 0x3c));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x38c));
    *(void **)(self + 0x3a8) = CallocInstance(8);
    **(int **)(self + 0x3a8) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(*(int *)(self + 0x384), shared));
    Ov107_EnqueueValue(self, **(int **)(self + 0x3a8));
    *(int *)(**(int **)(self + 0x3a8) + 0x5c) |= 2;
    capsule.pos = zero;
    capsule.up = data_02042258;
    capsule.radius = 0x1d1e;
    capsule.height = 0x500;
    p = List_InsertSorted(self + 0x144, 4, 0x64);
    h = (*p = Ov107_Mover_New(&capsule));
    *(int *)(self + 0x390) = h;
}

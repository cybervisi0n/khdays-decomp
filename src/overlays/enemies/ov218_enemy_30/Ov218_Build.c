/* Build the ov218 actor: its brain (020ce224), update (020ce25c), message (020ce2bc), spawn (020ce430),
 * teardown (020ce36c), damage (020ce3fc) and animation (020ce3a8) callbacks are installed, bit 6 of
 * the +0x60 high byte and bits 2/9 of +0x1ae are set, the body gets a 0.375 radius sphere (+0x64)
 * and the scene's +0x5c bit 2 is set; the pose is scaled 1.3, model 3 of the +0x390 set becomes the
 * +0x384 rig (subscribed to the +0x9c scene) and the two data_ov218_020cf314 models go into the
 * +0x39c pairs, attached (020c9074) and hidden. Two collision cylinders from the sphere are registered
 * in the +0x22c (16) and +0x144 (4) pools; the second is kept in +0x38c. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { u8 a, b; } Pair2;
struct EffectPair { int res; int handle; };
struct Ov218Models { char pad[0x39c]; struct EffectPair pair[2]; };

extern u8 data_ov218_020cf314[];
extern void Ov218_Destroy(void);
extern void Ov218_PropagateBlockToLinkedNodes(void);
extern void Ov218_OnMessage(void);
extern void Ov218_SpawnActorRegistryEntry(void);
extern void Ov218_ReleaseHeld3a8(void);
extern void Ov218_StoreTargetVector(void);
extern void Ov218_ReplayAnim(void);
extern void Srt_SetScaleUniform(void *srt, int scale);
extern void *Ov107_PackTextureHandle(int set, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void Ov107_EnqueueValue(char *self, int model);
extern int *List_InsertSorted(void *pool, int count, int size);
extern int Ov107_CloneResourceTransform(void *sphere);

static inline void VecSet(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void Ov218_Build(char *self)
{
    u8 ids[2];
    int set;
    int model;
    int i;
    int *cyl;

    ids[1] = data_ov218_020cf314[1];
    ids[0] = data_ov218_020cf314[0];
    set = *(int *)(self + 0x390);
    *(void **)(self + 8) = Ov218_Destroy;
    *(void **)(self + 0xc) = Ov218_PropagateBlockToLinkedNodes;
    *(void **)(self + 0x1c) = Ov218_OnMessage;
    *(void **)(self + 0x30) = Ov218_SpawnActorRegistryEntry;
    *(void **)(self + 0x34) = Ov218_ReleaseHeld3a8;
    *(void **)(self + 0x1d0) = Ov218_StoreTargetVector;
    *(void **)(self + 0x1dc) = Ov218_ReplayAnim;
    {
        u16 hw = *(u16 *)(self + 0x60);
        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x40) << 0x18) >> 0x10);
    }
    *(u16 *)(self + 0x1ae) |= 0x204;
    *(int *)(self + 0x70) = 0x600;
    *(int *)(self + 0x54) = 0;
    *(int *)(self + 0x58) = 0;
    *(int *)(*(int *)(self + 0x9c) + 0x5c) |= 4;
    VecSet((VecFx32 *)(self + 0x64), 0, *(int *)(self + 0x70), 0);
    Srt_SetScaleUniform(self + 0xa0, 0x14cd);
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(set, 3));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    for (i = 0; i < 2; i++) {
        model = ((struct Ov218Models *)self)->pair[i].res = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(set, ids[i]));
        Ov107_EnqueueValue(self, model);
        *(int *)(model + 0x5c) |= 2;
    }
    *(int **)(self + 0x388) = List_InsertSorted(self + 0x22c, 0x10, 0x64);
    **(int **)(self + 0x388) = Ov107_CloneResourceTransform(self + 0x64);
    cyl = List_InsertSorted(self + 0x144, 4, 0x64);
    *(int *)(self + 0x38c) = *cyl = Ov107_CloneResourceTransform(self + 0x64);
}

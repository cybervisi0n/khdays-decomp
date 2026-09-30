/* Constructor of an ov254 marker object: installs its handlers (+8, +0x30 update, +0x1dc), sets
 * bits 1-3, 5 and 6 of the +0x60 high byte and bits 2-4 of +0x1ae, the +0x64 pose (0, 1, 0, tiny
 * scale), builds the +0x384 item from pose 0x45 of the +0x388 pool and subscribes it to +0x9c. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);

extern void *Ov107_PackTextureHandle(int pool, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern const VecFx32 data_02041dc8;
extern void Ov254_OnDespawn_3(void);
extern void Ov254_Marker_CreateAiTask(void);
extern void Ov254_Marker_ApplyAnims(void);

static inline void VecSet(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void Ov254_MarkerConstruct(char *self)
{
    int pool = *(int *)(self + 0x388);
    VecFx32 *pose;

    *(Callback *)(self + 0x8) = Ov254_OnDespawn_3;
    *(Callback *)(self + 0x30) = Ov254_Marker_CreateAiTask;
    *(Callback *)(self + 0x1dc) = Ov254_Marker_ApplyAnims;
    {
        u16 hw = *(u16 *)(self + 0x60);
        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x6e) << 0x18) >> 0x10);
    }
    *(u16 *)(self + 0x100 + 0xae) |= 0x1c;
    pose = (VecFx32 *)(self + 0x64);
    *pose = data_02041dc8;
    VecSet(pose, 0, *(int *)(self + 0x70) = 1, 0);
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(pool, 0x45));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
}

/* Constructor of an ov254 helper: installs its handlers (+8, +0x1c message, +0x30 update, +0x1dc),
 * sets bits 1-3 and 6 of the +0x60 high byte and bits 2 and 4 of +0x1ae, zeroes the +0x64 pose
 * with a tiny scale, clears +0x54 / +0x58, builds the +0x384 item (pose 0x41 of the +0x38c pool,
 * subscribed to +0x9c) and the hidden +0x390 item (pose 0x42, registered). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);

extern void *Ov107_PackTextureHandle(int pool, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void Ov107_EnqueueValue(char *self, int item);
extern const VecFx32 data_02041dc8;
extern void Ov254_Destroy(void);
extern void Ov254_HelperBHandleMessage(void);
extern void Ov254_Helper_CreateAiTask(void);
extern void Ov254_RebindChannels(void);

void Ov254_HelperConstruct(char *self)
{
    int pool = *(int *)(self + 0x38c);

    *(Callback *)(self + 0x8) = Ov254_Destroy;
    *(Callback *)(self + 0x1c) = Ov254_HelperBHandleMessage;
    *(Callback *)(self + 0x30) = Ov254_Helper_CreateAiTask;
    *(Callback *)(self + 0x1dc) = Ov254_RebindChannels;
    {
        u16 hw = *(u16 *)(self + 0x60);
        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x4e) << 0x18) >> 0x10);
    }
    *(u16 *)(self + 0x100 + 0xae) |= 0x14;
    *(VecFx32 *)(self + 0x64) = data_02041dc8;
    *(int *)(self + 0x70) = 1;
    *(int *)(self + 0x54) = 0;
    *(int *)(self + 0x58) = 0;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(pool, 0x41));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    *(int *)(self + 0x390) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(pool, 0x42));
    Ov107_EnqueueValue(self, *(int *)(self + 0x390));
    *(int *)(*(int *)(self + 0x390) + 0x5c) |= 2;
}

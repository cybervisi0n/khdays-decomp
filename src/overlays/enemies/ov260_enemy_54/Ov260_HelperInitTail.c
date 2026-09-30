/* Constructor tail of an ov260 helper: installs its handlers (+8 020d2510, +0xc veneer 020d252c,
 * +0x30 020d25e0, +0x1dc 020d2538), sets bits 1-3 and 6 of the +0x60 high byte and bits 2 and 4 of
 * +0x1ae, +0x70 = 0x800, +0x64 rests at the origin, +0x54 / +0x58 clear, and its model (+0x384)
 * loads from the +0x38c owner's kit entry 0x31 and registers with the +0x9c scene. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern int CreateSubitemInstance0xB4(int item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void Ov260_OnDespawn(void);
extern void func_ov260_020d252c(void);
extern void Ov260_SpawnActorRegistryEntry(void);
extern void Ov260_Model_ReapplyTracks(void);
extern const VecFx32 data_02041dc8;

void Ov260_HelperInitTail(char *self)
{
    char *owner = *(char **)(self + 0x38c);

    *(void **)(self + 8) = Ov260_OnDespawn;
    *(void **)(self + 0xc) = func_ov260_020d252c;
    *(void **)(self + 0x30) = Ov260_SpawnActorRegistryEntry;
    *(void **)(self + 0x1dc) = Ov260_Model_ReapplyTracks;
    {
        u16 hw = *(u16 *)(self + 0x60);
        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x4e) << 0x18) >> 0x10);
    }
    *(u16 *)(self + 0x1ae) |= 0x14;
    *(int *)(self + 0x70) = 0x800;
    *(VecFx32 *)(self + 0x64) = data_02041dc8;
    *(int *)(self + 0x54) = 0;
    *(int *)(self + 0x58) = 0;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(owner, 0x31));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
}

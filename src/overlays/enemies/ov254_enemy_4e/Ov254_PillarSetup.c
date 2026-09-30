/* Setup of the ov254 enemy's pillar: installs the +8 tick (020d2254), +0xc (020d2270), +0x30 (020d23b8),
 * +0x1d0 hit (020d238c) and +0x1dc (020d22c0) handlers, sets bits 1-3 and 5 of the +0x60 high byte, bit
 * 2 of +0x1ae and the top two bits of the +0 flags, the unit +0x64 pose and flag 2 of the +0x9c body;
 * the main model (+0x384, item 0x4d of the +0x390 pool) is subscribed with its animation stopped, and
 * the +0x388 contact is an axis-aligned box centred 12.0 below the origin (half extents 7.0 x 12.0 x
 * 7.0).
 * Codegen: the +0x70 scale and the +0x388 contact slot go through local pointers and the +0x9c body
 * through LoadPtr; the plain spellings colour the constant and copy registers differently. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { VecFx32 pos; VecFx32 axis[3]; VecFx32 half; } Box;
struct bf { unsigned b : 8; };

/* Reads the word stored at an address. */
static inline int LoadPtr(int addr)
{
    return *(int *)addr;
}

extern void Ov254_OnDespawn(void);
extern void Ov254_Pillar_TickFollowOwner(void);
extern void Ov254_Pillar_CreateAiTask(void);
extern void Ov254_Pillar_OnHit(void);
extern void Ov254_Model_ReapplyTracks(void);
extern void *Ov107_PackTextureHandle(int pool, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void RefreshObjectCallbacks(int item, int a);
extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern int Ov107_HitShape_NewBox(Box *box);
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042270;
extern const VecFx32 data_02042264;
extern const VecFx32 data_02042258;

void Ov254_PillarSetup(char *self)
{
    int pool = *(int *)(self + 0x390);
    char *pItemSlot;
    int *pScale;
    Box box;

    *(Callback *)(self + 0x8) = Ov254_OnDespawn;
    *(Callback *)(self + 0xc) = Ov254_Pillar_TickFollowOwner;
    *(Callback *)(self + 0x30) = Ov254_Pillar_CreateAiTask;
    *(Callback *)(self + 0x1d0) = Ov254_Pillar_OnHit;
    pScale = (int *)(self + 0x70);
    *(Callback *)(self + 0x1dc) = Ov254_Model_ReapplyTracks;
    {
        u16 hw = *(u16 *)(self + 0x60);

        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x2e) << 0x18) >> 0x10);
    }
    *(u16 *)(self + 0x100 + 0xae) |= 4;
    *(u16 *)self |= 0xc000;
    *pScale = 1;
    {
        int scale = *pScale;

        *(int *)(self + 0x64) = 0;
        *(int *)(self + 0x68) = scale;
        *(int *)(self + 0x6c) = 0;
    }
    *(int *)(LoadPtr((int)self + 0x9c) + 0x5c) |= 4;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(pool, 0x4d));
    RegisterSubscriberSlot(LoadPtr((int)self + 0x9c), *(int *)(self + 0x384));
    RefreshObjectCallbacks(*(int *)(self + 0x384), 0);
    box.pos = data_02041dc8;
    box.axis[0] = data_02042270;
    box.axis[1] = data_02042264;
    box.axis[2] = data_02042258;
    box.half.x = box.half.z = 0x7000;
    pItemSlot = self + 0x388;
    box.half.y = 0xc000;
    box.pos.y -= 0xc000;
    *(int **)(self + 0x388) = List_InsertSorted(self + 0x22c, 0x10, 0x64);
    **(int **)pItemSlot = Ov107_HitShape_NewBox(&box);
    ((struct bf *)(*(int *)pItemSlot + 8))->b |= 2;
}

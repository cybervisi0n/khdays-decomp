/* Constructor of the ov237 shot: installs the handlers (+8, +0xc, +0x1c message, +0x30, +0x1d0 hit
 * filter), sets bits 1, 2 and 6 of the +0x60 high byte and bit 2 of +0x1ae, a 0.375 radius at rest,
 * builds model 0x3c of the owner's +0x390 set as the +0x384 rig (subscribed to +0x9c, tracks 0, 2, 4
 * and 1 looping, posed), sets bit 3 of +0x1ae and places the body on the +0x22c pool (+0x388, flag 1
 * of its +8 byte); no target yet (+0x38c). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
typedef struct { VecFx32 pos; int scale; } Placement;
typedef struct { unsigned f : 8; } B8;

extern void Ov237_OnDespawn(void);
extern void Ov237_TickAndSyncModelXform(void);
extern void func_ov237_020d0ca0(void);
extern void Ov237_CreateRegistryEntryAndLink(void);
extern void Ov237_OnHit(void);
extern void *Ov107_PackTextureHandle(int set, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void SetSubitemState(int rig, int channel, int a, int b);
extern void RefreshObjectCallbacks(int rig, int a);
extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern int Ov107_CloneResourceTransform(const Placement *placement);

void Ov237_ShotConstruct(char *self)
{
    int set = *(int *)(self + 0x390);

    *(Callback *)(self + 0x8) = Ov237_OnDespawn;
    *(Callback *)(self + 0xc) = Ov237_TickAndSyncModelXform;
    *(Callback *)(self + 0x1c) = func_ov237_020d0ca0;
    *(Callback *)(self + 0x30) = Ov237_CreateRegistryEntryAndLink;
    *(Callback *)(self + 0x1d0) = Ov237_OnHit;
    {
        u16 hw = *(u16 *)(self + 0x60);

        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x46) << 0x18) >> 0x10);
    }
    *(u16 *)(self + 0x1ae) |= 4;
    /* overwritten default: spends the scheduling budget (keeps the +0x1ae store in IR order) */
    *(int *)(self + 0x70) = 0x1000;
    *(int *)(self + 0x70) = 0x600;
    *(int *)(self + 0x54) = 0;
    *(int *)(self + 0x58) = 0;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(set, 0x3c));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    SetSubitemState(*(int *)(self + 0x384), 0, 0, 1);
    SetSubitemState(*(int *)(self + 0x384), 2, 0, 1);
    SetSubitemState(*(int *)(self + 0x384), 4, 0, 1);
    SetSubitemState(*(int *)(self + 0x384), 1, 0, 1);
    RefreshObjectCallbacks(*(int *)(self + 0x384), 0);
    *(u16 *)(self + 0x1ae) |= 8;
    *(int *)(self + 0x388) = (int)List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x388) = Ov107_CloneResourceTransform((Placement *)(self + 0x64));
    ((B8 *)(*(int *)(self + 0x388) + 8))->f |= 2;
    *(int *)(self + 0x38c) = 0;
}

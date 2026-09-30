/* Build the ov146 actor: its brain (020ce514), update (020ce538), spawn (020ce7e0), teardown
 * (020ce608), knock-back (020ce6e4) and animation (020ce564) callbacks are installed, bits 2-3 of the
 * +0x60 high byte clear and bit 1 is set, +0x1ae bit 2 clears and +0x1b0 gains 0x888; the body is 1.0 x
 * 0.5 with a 0.75 radius (+0x64 sphere lifted by it). Model 0xb of the +0x3b4 set becomes the +0x384
 * rig, subscribed to the +0x9c scene, its data_ov146_020cf534 sub-part goes to +0x3b8, the rig pose
 * resets and record 0xc binds to the +0x388 slot. Two collision cylinders from the +0x64 sphere at the
 * origin are registered in the +0x22c (16) and +0x144 (4) pools; the second is kept in +0x3b0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { VecFx32 center; int nRadius; } Sphere;
typedef struct { char data[0x24]; } AnimSlot;

extern void Ov146_Destroy(void);
extern void Ov146_TickUnlessFrozen(void);
extern void Ov146_Rider_CreateAiTask(void);
extern void Ov146_PropagateBlockChainThenNotify_2(void);
extern void Ov146_OnKnockback(void);
extern void Ov146_RebindAnimSlot(void);
extern void *Ov107_PackTextureHandle(int set, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern int InsertSortedEntryWithKey(int rig, int kind, void *desc);
extern void RefreshObjectCallbacks(int rig, int a);
extern void Snd_RegisterSeqAndBind(AnimSlot *slot, int bank, void *record, int d);
extern void MainBlob_ResetSlotRows(int rig, AnimSlot *slot);
extern int *List_InsertSorted(void *pool, int count, int size);
extern int Ov107_CloneResourceTransform(Sphere *sphere);
extern char data_ov146_020cf534[];
extern const VecFx32 data_02041dc8;

static inline void VecSet(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void Ov146_ActorConstruct(char *self)
{
    int set = *(int *)(self + 0x3b4);
    Sphere body;
    int *cyl;

    *(void **)(self + 8) = Ov146_Destroy;
    *(void **)(self + 0xc) = Ov146_TickUnlessFrozen;
    *(void **)(self + 0x30) = Ov146_Rider_CreateAiTask;
    *(void **)(self + 0x34) = Ov146_PropagateBlockChainThenNotify_2;
    *(void **)(self + 0x1d0) = Ov146_OnKnockback;
    *(void **)(self + 0x1dc) = Ov146_RebindAnimSlot;
    {
        u16 hw = *(u16 *)(self + 0x60);
        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            (((unsigned int)(u16)((((unsigned int)hw << 0x10) >> 0x18) & ~0xc) << 0x18) >> 0x10);
    }
    {
        u16 hw = *(u16 *)(self + 0x60);
        /* the clear is written once more and overwritten at once: a dead store that uses up the
         * scheduler's budget, so the rest of the block keeps the ROM's order */
        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            (((unsigned int)(u16)((((unsigned int)hw << 0x10) >> 0x18) & ~0xc) << 0x18) >> 0x10);
        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 2) << 0x18) >> 0x10);
    }
    *(u16 *)(self + 0x1ae) &= ~4;
    *(u16 *)(self + 0x1b0) |= 0x888;
    *(int *)(self + 0x70) = 0xc00;
    *(int *)(self + 0x54) = 0x1000;
    *(int *)(self + 0x58) = 0x800;
    VecSet((VecFx32 *)(self + 0x64), 0, *(int *)(self + 0x70), 0);
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(set, 0xb));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    *(int *)(self + 0x3b8) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov146_020cf534);
    RefreshObjectCallbacks(*(int *)(self + 0x384), 0);
    Snd_RegisterSeqAndBind((AnimSlot *)(self + 0x388), *(int *)(*(int *)(self + 0x384) + 0x88), Ov107_PackTextureHandle(set, 0xc), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x384), (AnimSlot *)(self + 0x388));
    body = *(Sphere *)(self + 0x64);
    body.center = data_02041dc8;
    *(int **)(self + 0x3ac) = List_InsertSorted(self + 0x22c, 0x10, 0x64);
    **(int **)(self + 0x3ac) = Ov107_CloneResourceTransform(&body);
    cyl = List_InsertSorted(self + 0x144, 4, 0x64);
    *(int *)(self + 0x3b0) = *cyl = Ov107_CloneResourceTransform(&body);
}

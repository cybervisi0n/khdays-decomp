/* Ov245_CarriedConstruct -- constructor of the ov245 carried object. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef void (*Callback)(void);
struct w8 { unsigned int lo : 8, rest : 24; };

struct Ov245Obj {
    int pad0[2];
    Callback tick;        /* 0x08 */
    Callback draw;        /* 0x0c */
    int pad10[3];
    Callback message;     /* 0x1c */
    int pad20;
    Callback hook24;      /* 0x24 */
    int pad28[2];
    Callback cb30;        /* 0x30 */
    int pad34[11];
    u16 flags60;          /* 0x60 */
    u16 pad62;
    VecFx32 pose;            /* 0x64 */
    int scale;            /* 0x70 */
    int pad74[10];
    int pOwner;           /* 0x9c */
    char padA0[0x1ae - 0xa0];
    u16 flags1ae;         /* 0x1ae */
    char pad1b0[0x1d0 - 0x1b0];
    Callback hit;         /* 0x1d0 */
    char pad1d4[0x384 - 0x1d4];
    int pItem;            /* 0x384 */
    int *pPlacement;      /* 0x388 */
    int p38c;             /* 0x38c */
    int pool;             /* 0x390 */
    int pSub;             /* 0x394 */
};

extern void Ov245_Destroy(void);
extern void Ov245_TickAndSyncModelXform(void);
extern void Ov245_SpawnSlotEffectMsg(void);
extern void Ov245_CreateAiTask(void);
extern void Ov245_FilterMessage(void);
extern void Ov245_SubHitFilter2(void);
extern void *Ov107_PackTextureHandle(int pool, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void SetSubitemState(int item, int channel, int a, int flag);
extern void RefreshObjectCallbacks(int item, int a);
extern void Ov107_EnqueueValue(struct Ov245Obj *self, int item);
extern int *List_InsertSorted(void *list, int stride, int max);
extern int Ov107_CloneResourceTransform(void *pose);
extern const VecFx32 data_02041dc8;

void Ov245_CarriedConstruct(struct Ov245Obj *self) {
    int pool = self->pool;

    /* written three times: the repeats are dead stores dropped after scheduling that use up its
     * budget, so the rest of the constructor keeps the ROM's order (as in Ov261_EnemyConstruct) */
    self->tick = Ov245_Destroy;
    self->tick = Ov245_Destroy;
    self->tick = Ov245_Destroy;
    self->draw = Ov245_TickAndSyncModelXform;
    self->message = Ov245_SpawnSlotEffectMsg;
    self->cb30 = Ov245_CreateAiTask;
    self->hook24 = Ov245_FilterMessage;
    self->hit = Ov245_SubHitFilter2;
    {
        u16 hw = self->flags60;
        self->flags60 = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x56) << 0x18) >> 0x10);
    }
    self->flags1ae |= 0xc;
    self->scale = 0x2000;
    self->pose = data_02041dc8;
    *(int *)(self->pOwner + 0x5c) |= 4;
    self->pItem = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(pool, 0x16));
    RegisterSubscriberSlot(self->pOwner, self->pItem);
    SetSubitemState(self->pItem, 0, 0, 1);
    RefreshObjectCallbacks(self->pItem, 0);
    self->pSub = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(pool, 0x1b));
    Ov107_EnqueueValue(self, self->pSub);
    *(int *)(self->pSub + 0x5c) |= 2;
    self->pPlacement = List_InsertSorted((char *)self + 0x22c, 0x10, 0x64);
    *self->pPlacement = Ov107_CloneResourceTransform(&self->pose);
    ((struct w8 *)(self->pPlacement + 2))->lo |= 2;
    self->p38c = 0;
}

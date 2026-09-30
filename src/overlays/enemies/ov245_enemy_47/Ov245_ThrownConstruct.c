/* Ov245_ThrownConstruct = Ov245_ThrownConstruct -- constructor of the ov245 thrown object: installs the handlers (+8 tick,
 * +0xc draw, +0x1c message, +0x30 callback, +0x24 hook, +0x1dc finish), seeds the +0x64 pose
 * (y and scale 2.6) and +0x54/+0x58 (0, 0x100), builds the primary item from pool entry 0x12
 * of the +0x394 pool (+0x384, subscribed, motion halted), the +0x398 sub-item from entry 0x24
 * (attached, bit 1 of +0x5c), and from a pose 1.5 up at scale 2.6 a +0x22c placement (+0x388,
 * bit 1 of its +8 low byte) and a +0x144 placement (+0x38c); +0x390 starts empty. */

#include "nitro/fx_types.h"

typedef struct { VecFx32 pos; int scale; } Pose;
typedef void (*Callback)(void);
struct w8 { unsigned int lo : 8, rest : 24; };

extern void Ov245_Destroy_2(void);
extern void Ov245_TickAndSyncTwoModelXforms(void);
extern void Ov245_SpawnSlotChildMsg(void);
extern void Ov245_Thrown_CreateAiTask(void);
extern void Ov245_FilterMessage(void);
extern void Ov245_Thrown_ApplyAnims(void);
extern void *Ov107_PackTextureHandle(int pool, int index);
extern int CreateSubitemInstance0xB4(void *item);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void RefreshObjectCallbacks(int item, int a);
struct Ov245Thrown;
extern void Ov107_EnqueueValue(struct Ov245Thrown *self, int item);
extern int *List_InsertSorted(void *list, int stride, int max);
extern int Ov107_CloneResourceTransform(void *pose);

static inline void VecSetP_(VecFx32 *v, int x, int y, int z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

struct Ov245Thrown {
    int pad0[2];
    Callback tick;        /* 0x08 */
    Callback draw;        /* 0x0c */
    int pad10[3];
    Callback message;     /* 0x1c */
    int pad20;
    Callback hook24;      /* 0x24 */
    int pad28[2];
    Callback cb30;        /* 0x30 */
    int pad34[8];
    int f54;              /* 0x54 */
    int f58;              /* 0x58 */
    int pad5c[2];
    VecFx32 pose;           /* 0x64 */
    int scale;            /* 0x70 */
    int pad74[10];
    int pOwner;           /* 0x9c */
    char padA0[0x144 - 0xa0];
    char list144[0x1dc - 0x144];   /* 0x144 */
    Callback cb1dc;       /* 0x1dc */
    char pad1e0[0x22c - 0x1e0];
    char list22c[0x384 - 0x22c];   /* 0x22c */
    int pItem;            /* 0x384 */
    int *pShape;          /* 0x388 */
    int placement;        /* 0x38c */
    int f390;             /* 0x390 */
    int pool;             /* 0x394 */
    int pSub;             /* 0x398 */
};

void Ov245_ThrownConstruct(struct Ov245Thrown *self) {
    int pool = self->pool;
    Pose pose;
    int *slot;

    /* written twice: the dead copy is dropped after scheduling but spends its budget, which
     * keeps the ROM's register choice for the handler stores (as in Ov245_CarriedConstruct) */
    self->tick = Ov245_Destroy_2;
    self->tick = Ov245_Destroy_2;
    self->draw = Ov245_TickAndSyncTwoModelXforms;
    self->message = Ov245_SpawnSlotChildMsg;
    self->cb30 = Ov245_Thrown_CreateAiTask;
    self->hook24 = Ov245_FilterMessage;
    self->cb1dc = Ov245_Thrown_ApplyAnims;
    self->scale = 0x2991;
    VecSetP_(&self->pose, 0, 0x2991, 0);   /* the constant again, not a re-read of scale */
    self->f54 = 0;
    self->f58 = 0x100;
    self->pItem = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(pool, 0x12));
    RegisterSubscriberSlot(self->pOwner, self->pItem);
    RefreshObjectCallbacks(self->pItem, 0);
    self->pSub = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(pool, 0x24));
    Ov107_EnqueueValue(self, self->pSub);
    *(int *)(self->pSub + 0x5c) |= 2;
    pose.pos.x = 0;
    pose.pos.y = 0x1800;
    pose.pos.z = 0;
    pose.scale = 0x2991;
    self->pShape = List_InsertSorted(self->list22c, 0x10, 0x64);
    *self->pShape = Ov107_CloneResourceTransform(&pose);
    ((struct w8 *)((int)self->pShape + 8))->lo |= 2;
    slot = List_InsertSorted(self->list144, 4, 0x64);
    self->placement = *slot = Ov107_CloneResourceTransform(&pose);
    self->f390 = 0;
}

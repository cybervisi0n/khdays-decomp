/* Ov126_ConstructSubitem: sub-item constructor of the ov125 enemy (four handlers, speed 0x200,
 * capsule 0x1000/0x200 on the data_02041dc8 / data_02042240 axes). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct {
    VecFx32 vPos;
    VecFx32 vUp;
    int nRadius;
    int nHeight;
} Capsule;

typedef struct {
    int value;
    int pad_0004;
    u32 flags : 8;
} Ov125PoolEntry;

typedef struct {
    char pad_0000[0x08];
    void (*callback_0008)(void);
    void (*callback_000c)(void);
    char pad_0010[0x20];
    void (*callback_0030)(void);
    char pad_0034[0x2c];
    u16 flags_0060;
    char pad_0062[0x0e];
    int field_0070;
    char pad_0074[0x28];
    void *subscriber_009c;
    char pad_00a0[0x10e];
    u16 flags_01ae;
    char pad_01b0[0x20];
    void (*callback_01d0)(void);
    char pad_01d4[0x58];
    char pool_022c[0x158];
    void *subitem_0384;
    Ov125PoolEntry *poolEntry_0388;
    int owner_038c;
} Ov125Object;

extern void *Ov107_PackTextureHandle(int owner, int index);
extern void *CreateSubitemInstance0xB4(void *item);
extern int Ov107_Mover_New(Capsule *req);
extern void Ov126_OnDespawn(void);
extern void Ov126_TickAndSyncModelXform(void);
extern void Ov126_CreateAiTask_2(void);
extern void Ov126_HandleBounce(void);
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042240;

void Ov126_ConstructSubitem(Ov125Object *self) {
    Capsule req;
    u16 v;

    self->callback_0008 = Ov126_OnDespawn;
    self->callback_000c = Ov126_TickAndSyncModelXform;
    self->callback_0030 = Ov126_CreateAiTask_2;
    self->callback_01d0 = Ov126_HandleBounce;
    self->field_0070 = 0x200;
    self->flags_01ae |= 0x14;
    v = self->flags_0060;
    self->flags_0060 =
        (u16)((v & ~0xff00) | (((((u32)v << 0x10) >> 0x18 | 0x40) << 0x18) >> 0x10));
    self->subitem_0384 = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self->owner_038c, 2));
    RegisterSubscriberSlot(self->subscriber_009c, self->subitem_0384);
    SetSubitemState(self->subitem_0384, 0, 0, 1);
    RefreshObjectCallbacks(self->subitem_0384, 0);
    req.vPos = data_02041dc8;
    req.vUp = data_02042240;
    req.nRadius = 0x1000;
    req.nHeight = 0x200;
    self->poolEntry_0388 = (Ov125PoolEntry *)List_InsertSorted(self->pool_022c, 0x10, 100);
    self->poolEntry_0388->value = Ov107_Mover_New(&req);
    self->poolEntry_0388->flags |= 2;
}

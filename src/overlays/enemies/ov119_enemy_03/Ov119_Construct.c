/* Constructor of the ov119 enemy (x3 with ov272/ov279). Installs the handlers (+8, +0x1c message,
 * +0x30, +0x34 tick, +0x1d0 hit filter, +0x1dc, +0x1e0), raises bit 6 of the +0x60 high byte, sets
 * the +0x64 pose (scale 0.56), builds the +0x384 rig from pose 0 (subscribed to +0x9c, callback
 * Ov119_RefreshAndPublishTransform with the enemy as its +0x84 owner) and resolves its five bones (+0x390 in set
 * 3; +0x398, +0x394, +0x39c, +0x3a0 in set 1); builds the five sub-items of data_ov119_020cf94c
 * into the +0x3a8 pair table (registered, bit 1 of +0x5c), registers actions 0, 1, 2 and 4 with
 * mode 1 (rate 1.13), reserves the +0x22c capsule (+0x388) and one +0x144 capsule (+0x38c, its
 * body at +0x2cc) of length 1.0 and radius 0.56 along +Y, and loads sound 0x121. */

#include "nitro/fx_types.h"

typedef struct { void *node; int pad; } Slot;
typedef struct { int w[5]; } KindTable;
typedef struct { VecFx32 pos; VecFx32 axis; int length; int radius; } Capsule;

extern void *Ov107_PackTextureHandle(void *self, int slot);
extern void *CreateSubitemInstance0xB4(void *res);
extern void RegisterSubscriberSlot(void *list, void *node);
extern int InsertSortedEntryWithKey(void *obj, int set, const char *name);
extern void *CallocInstance(int size);
extern void Ov107_EnqueueValue(void *self, void *obj);
extern void Ov107_Actor_SetAttachSlot(void *self, int a, int b, const VecFx32 *v, int e);
extern void *List_InsertSorted(void *list, int size, int count);
extern void *Ov107_Mover_New(const Capsule *capsule);
extern void Res_RequestIdPair(int id);

extern KindTable data_ov119_020cf94c;
extern const char data_ov119_020cf98c[];
extern const char data_ov119_020cf998[];
extern const char data_ov119_020cf9a4[];
extern const char data_ov119_020cf9b0[];
extern const char data_ov119_020cf9bc[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042264;

extern void Ov119_Destroy(void);
extern void Ov119_OnEffectMessage(void);
extern void Ov119_Tick(void);
extern void Ov119_CreateAiTask(void);
extern void Ov119_OnHit(void);
extern void Ov119_RequestSubState11IfIdle(void);
extern void Ov119_PlayAnimWithParts(void);
extern void Ov119_RefreshAndPublishTransform(void);

void Ov119_Construct(char *self)
{
    KindTable kinds;
    Capsule cap;
    int i;

    kinds = data_ov119_020cf94c;
    *(void **)(self + 0x8) = (void *)Ov119_Destroy;
    *(void **)(self + 0x1c) = (void *)Ov119_OnEffectMessage;
    *(void **)(self + 0x34) = (void *)Ov119_Tick;
    *(void **)(self + 0x30) = (void *)Ov119_CreateAiTask;
    *(void **)(self + 0x1d0) = (void *)Ov119_OnHit;
    *(void **)(self + 0x1e0) = (void *)Ov119_RequestSubState11IfIdle;
    *(void **)(self + 0x1dc) = (void *)Ov119_PlayAnimWithParts;
    {
        unsigned int v = *(unsigned short *)(self + 0x60);
        *(unsigned short *)(self + 0x60) = (unsigned short)((v & ~0xff00) | ((((v << 0x10) >> 0x18 | 0x40) << 0x18) >> 0x10));
    }
    *(int *)(self + 0x70) = 0x900;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x900;
    *(int *)(self + 0x6c) = 0;

    *(void **)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    RegisterSubscriberSlot(*(void **)(self + 0x9c), *(void **)(self + 0x384));
    *(void **)(*(char **)(self + 0x384) + 0x74) = (void *)Ov119_RefreshAndPublishTransform;
    *(char **)(*(char **)(self + 0x384) + 0x84) = self;
    *(int *)(self + 0x390) = InsertSortedEntryWithKey(*(void **)(self + 0x384), 3, data_ov119_020cf98c);
    *(int *)(self + 0x398) = InsertSortedEntryWithKey(*(void **)(self + 0x384), 1, data_ov119_020cf998);
    *(int *)(self + 0x394) = InsertSortedEntryWithKey(*(void **)(self + 0x384), 1, data_ov119_020cf9a4);
    *(int *)(self + 0x39c) = InsertSortedEntryWithKey(*(void **)(self + 0x384), 1, data_ov119_020cf9b0);
    *(int *)(self + 0x3a0) = InsertSortedEntryWithKey(*(void **)(self + 0x384), 1, data_ov119_020cf9bc);
    *(void **)(self + 0x3a8) = CallocInstance(0x28);
    for (i = 0; i < 5; i++) {
        (*(Slot **)(self + 0x3a8))[i].node = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, kinds.w[i]));
        Ov107_EnqueueValue(self, (*(Slot **)(self + 0x3a8))[i].node);
        *(int *)((char *)(*(Slot **)(self + 0x3a8))[i].node + 0x5c) |= 2;
    }
    Ov107_Actor_SetAttachSlot(self, 0, 1, 0, 0x1200);
    Ov107_Actor_SetAttachSlot(self, 1, 1, 0, 0x1200);
    Ov107_Actor_SetAttachSlot(self, 2, 1, 0, 0x1200);
    Ov107_Actor_SetAttachSlot(self, 4, 1, 0, 0x1200);

    cap.pos = data_02041dc8;
    cap.axis = data_02042264;
    cap.length = 0x1000;
    cap.radius = 0x900;
    *(void **)(self + 0x388) = List_InsertSorted(self + 0x22c, 0x10, 100);
    **(void ***)(self + 0x388) = Ov107_Mover_New(&cap);
    {
        void **slot = (void **)List_InsertSorted(self + 0x144, 4, 100);
        void *node = Ov107_Mover_New(&cap);
        *slot = node;
        *(void **)(self + 0x38c) = node;
        *(char **)(self + 0x2cc) = (char *)node + 4;
    }
    Res_RequestIdPair(0x121);
}

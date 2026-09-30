/* Constructor of the ov158 enemy (and its byte-identical twin): installs the handlers (+8 tick
 * 020cc248, +0xc 020cc290, +0x1c message 020cc398, +0x30 020cc728, +0x28 020cc53c, +0x2c
 * 020cc564, +0x34 020cc58c, +0x1d0 on-hit 020cc784, +0x1e0 release 020cc99c, +0x1dc finish
 * 020cc2dc), seeds the +0x64 pose (scale 0x1c00, y 0x1c00), builds the primary item from pool
 * entry 0 (subscribed), resolves its two mode-3 joints (+0x398 data_ov159_020d4fec, +0x394
 * data_ov159_020d4ff4), keeps the data_ov159_020d4ff8 motion handle of pool entry 9 (+0x39c),
 * the eight sub-items of the data_ov159_020d4f80 kinds in a fresh 64-byte slot table (+0x390,
 * attached, bit 1 on their +0x5c), configures actions 0/1/2/4 (mode 1, rate 0x3000), creates a
 * placement from the +0x64 pose on the +0x22c list (+0x388) and a capsule (zero position, world
 * Y axis, radius 0x1000, height 0x1c00) on the +0x144 list (+0x38c); +0x3a0 is built by
 * Ov159_Actor_New and sound 0x150 is loaded. */

#include "nitro/fx_types.h"

struct Ov158Capsule {
    VecFx32 vPos;
    VecFx32 vUp;
    int nRadius;
    int nHeight;
};

struct Ov158Kinds { int w[8]; };

struct Ov158SubitemSlot {
    void *subitem;
    int pad;
};

extern struct Ov158Kinds data_ov159_020d4f80;
extern VecFx32 data_02041dc8;
extern VecFx32 data_02042264;
extern const char data_ov159_020d4fec[];
extern const char data_ov159_020d4ff4[];
extern const char data_ov159_020d4ff8[];

extern void Ov159_Destroy(void);
extern void Ov159_RebindClip(void);
extern void Ov159_HandleSpawnMessage(void);
extern void Ov159_stCreateRegistryEntry(void);
extern void Ov159_ForwardEventToChild(void);
extern void Ov159_NotifyPartThenBase(void);
extern void Ov159_TickHandler(void);
extern void Ov159_HandleHit(void);
extern void Ov159_RequestState8(void);
extern void Ov159_RetuneRigRearmEmitter(void);
extern void *Ov159_Actor_New(int *self);

extern void *Ov107_PackTextureHandle();
extern void *CreateSubitemInstance0xB4();
extern void RegisterSubscriberSlot();
extern char *InsertSortedEntryWithKey();
extern void *Ov107_CreateNamedResourceBinding();
extern void *CallocInstance();
extern void Ov107_EnqueueValue();
extern void Ov107_Actor_SetAttachSlot();
extern void *List_InsertSorted();
extern int Ov107_CloneResourceTransform();
extern int Ov107_Mover_New(struct Ov158Capsule *req);
extern void Res_RequestIdPair(int nId);

void Ov159_Construct(int param)
{
    struct Ov158Kinds kinds;
    struct Ov158Capsule req;
    int i;

    kinds = data_ov159_020d4f80;

    *(void **)(param + 0x08) = Ov159_Destroy;
    *(void **)(param + 0x0c) = Ov159_RebindClip;
    *(void **)(param + 0x1c) = Ov159_HandleSpawnMessage;
    *(void **)(param + 0x30) = Ov159_stCreateRegistryEntry;
    *(void **)(param + 0x28) = Ov159_ForwardEventToChild;
    *(void **)(param + 0x2c) = Ov159_NotifyPartThenBase;
    *(void **)(param + 0x34) = Ov159_TickHandler;
    *(void **)(param + 0x1d0) = Ov159_HandleHit;
    *(void **)(param + 0x1e0) = Ov159_RequestState8;
    *(void **)(param + 0x1dc) = Ov159_RetuneRigRearmEmitter;

    *(int *)(param + 0x70) = 0x1c00;
    *(int *)(param + 0x64) = 0;
    *(int *)(param + 0x68) = 0x1c00;
    *(int *)(param + 0x6c) = 0;

    {
        int *self = (int *)param;

        ((void **)self)[0xe1] = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
        RegisterSubscriberSlot(self[0x27], ((void **)self)[0xe1]);
        ((void **)self)[0xe6] = InsertSortedEntryWithKey(self[0xe1], 3, data_ov159_020d4fec);
        ((void **)self)[0xe5] = InsertSortedEntryWithKey(self[0xe1], 3, data_ov159_020d4ff4);
        ((void **)self)[0xe7] = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(self, 9), data_ov159_020d4ff8);
        ((void **)self)[0xe4] = CallocInstance(0x40);

        for (i = 0; i < 8; i++) {
            ((struct Ov158SubitemSlot *)((void **)self)[0xe4])[i].subitem =
                CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, kinds.w[i]));
            Ov107_EnqueueValue(self,
                ((struct Ov158SubitemSlot *)((void **)self)[0xe4])[i].subitem);
            *(int *)((char *)((struct Ov158SubitemSlot *)
                ((void **)self)[0xe4])[i].subitem + 0x5c) |= 2;
        }

        Ov107_Actor_SetAttachSlot(self, 0, 1, 0, 0x3000);
        Ov107_Actor_SetAttachSlot(self, 1, 1, 0, 0x3000);
        Ov107_Actor_SetAttachSlot(self, 2, 1, 0, 0x3000);
        Ov107_Actor_SetAttachSlot(self, 4, 1, 0, 0x3000);

        ((void **)self)[0xe2] = List_InsertSorted(self + 0x8b, 0x10, 100);
        *((int **)self)[0xe2] = Ov107_CloneResourceTransform(self + 0x19);

        req.vPos = data_02041dc8;
        req.vUp = data_02042264;
        req.nRadius = 0x1000;
        req.nHeight = 0x1c00;
        {
            int *p = List_InsertSorted(self + 0x51, 4, 100);
            self[0xe3] = *p = Ov107_Mover_New(&req);
        }
        ((void **)self)[0xe8] = Ov159_Actor_New(self);
        Res_RequestIdPair(0x150);
    }
}

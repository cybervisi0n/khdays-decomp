/* Ov216_Construct: ported from a matched sibling family (same shape, constants and offsets adjusted). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { void *node; int pad; } Slot;
typedef struct { int w[5]; } KindTable;
typedef struct { int w[6]; } ParamBlock;
typedef struct { VecFx32 v; int w; } SpawnSeed;

extern void *Ov107_PackTextureHandle(void *self, int slot);
extern void *CreateSubitemInstance0xB4(void *res);
extern void RegisterSubscriberSlot(void *list, void *node);
extern void Snd_RegisterSeqAndBind(void *dst, void *a, void *b, int n);
extern void MainBlob_ResetSlotRows(void *obj, void *block);
extern int FindResourceIndexByName(void *obj, const char *name);
extern void RefreshObjectCallbacks(void *obj, int a);
extern void Ov107_Actor_SetAttachSlot(void *self, int a, int b, const VecFx32 *v, int e);
extern void *Ov107_GetActorManager(void);
extern void Ov107_EnqueueValue(void *self, void *obj);
extern void *JointModel_New(void *res, int n);
extern void *Ov216_createRegistryEntryStoreField(void *self);
extern void *List_InsertSorted(void *list, int size, int count);
extern void *Ov107_CloneResourceTransform(const SpawnSeed *seed);
extern void Res_RequestIdPair(int id);

extern KindTable data_ov216_020cebe4;
extern const char data_ov216_020cec4c[];
extern const char data_ov216_020cec58[];
extern const VecFx32 data_02041dc8;

extern void Ov216_SyncSubitemPoseToJoint(void);
extern void Ov216_initSubitemPathTarget(void);
extern void Ov216_destroyObjectSlots(void);
extern void Ov216_resetChildAndDispatch(void);
extern void Ov216_HandleSpawnMessage(void);
extern void Ov216_stCreateRegistryEntry(void);
extern void Ov216_stSyncChildPauseFlag(void);
extern void Ov216_ApplyHitEvent(void);
extern void Ov216_spawnFromTable(void);
extern void Ov216_RequestSubState7IfNotCurrent(void);
extern void Ov216_RequestSubState8IfIdle(void);

void Ov216_Construct(char *self)
{
    KindTable kinds;
    ParamBlock params;
    SpawnSeed seed;
    VecFx32 velocity;
    int i;

    kinds = data_ov216_020cebe4;
    params.w[0] = -0x190b;
    params.w[1] = -6;
    params.w[2] = -0x1fc4;
    params.w[3] = params.w[0] + 0x3217;
    params.w[4] = params.w[1] + 0x2cc3;
    params.w[5] = params.w[2] + 0x2597;
    *(void **)(self + 0x8) = (void *)Ov216_destroyObjectSlots;
    *(void **)(self + 0xc) = (void *)Ov216_resetChildAndDispatch;
    *(void **)(self + 0x1c) = (void *)Ov216_HandleSpawnMessage;
    *(void **)(self + 0x30) = (void *)Ov216_stCreateRegistryEntry;
    *(void **)(self + 0x34) = (void *)Ov216_stSyncChildPauseFlag;
    *(void **)(self + 0x1d0) = (void *)Ov216_ApplyHitEvent;
    *(void **)(self + 0x1dc) = (void *)Ov216_spawnFromTable;
    *(void **)(self + 0x1e0) = (void *)Ov216_RequestSubState7IfNotCurrent;
    *(void **)(self + 0x1e4) = (void *)Ov216_RequestSubState8IfIdle;
    *(int *)(self + 0x70) = 0x1800;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x1800;
    *(int *)(self + 0x6c) = 0;
    *(ParamBlock *)(self + 0x1fc) = params;
    *(char *)(self + 0x1c9) = 1;
    {
        unsigned int v = *(u16 *)(self + 0x60);
        unsigned int low = v & ~0xff00;
        v <<= 16;
        v >>= 24;
        v |= 0x20;
        v <<= 24;
        v >>= 16;
        v |= low;
        *(u16 *)(self + 0x60) = (u16)v;
        *(u16 *)(self + 0x1ae) |= 8;
    }

    {
        void *rig = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
        *(void **)(self + 0x384) = rig;
        *(void **)((char *)rig + 0x74) = (void *)Ov216_SyncSubitemPoseToJoint;
    }
    *(char **)(*(char **)(self + 0x384) + 0x84) = self;
    RegisterSubscriberSlot(*(void **)(self + 0x9c), *(void **)(self + 0x384));

    {
        void *anim = Ov107_PackTextureHandle(self, 1);
        Snd_RegisterSeqAndBind(self + 0x388, *(void **)(*(char **)(self + 0x384) + 0x88), anim, 0xc);
        MainBlob_ResetSlotRows(*(void **)(self + 0x384), self + 0x388);
        *(int *)(self + 0x438) = FindResourceIndexByName(*(void **)(self + 0x384), data_ov216_020cec4c);

        {
            *(void **)(self + 0x420) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0xa));
            *(void **)(*(char **)(self + 0x420) + 0x6c) = (void *)Ov216_initSubitemPathTarget;
            *(char **)(*(char **)(self + 0x420) + 0x84) = self;
            RefreshObjectCallbacks(*(void **)(self + 0x420), 0);
            *(int *)(self + 0x434) = FindResourceIndexByName(*(void **)(self + 0x420), data_ov216_020cec58);
        }
    }

    velocity.x = 0;
    velocity.y = 0x1800;
    velocity.z = 0;
    Ov107_Actor_SetAttachSlot(self, 0, 2, &velocity, 0xd9a);
    velocity.x = 0;
    velocity.z = 0;
    velocity.y = -0x1800;
    Ov107_Actor_SetAttachSlot(self, 2, 3, &velocity, 0x99a);
    Ov107_Actor_SetAttachSlot(self, 1, 2, 0, 0xccd);

    for (i = 0; i < 5; i++) {
        void *node;
        if (i <= 0) {
            void *os = Ov107_GetActorManager();
            unsigned int kind = kinds.w[i] & 0x1ff;
            unsigned int addr = (*(int *)((char *)os + 0x88) + 0x8000) & 0x00fffffc;
            addr = addr << 7;
            addr = addr | 0x80000000;
            node = CreateSubitemInstance0xB4((void *)(kind | addr));
        } else {
            node = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, kinds.w[i]));
        }
        Ov107_EnqueueValue(self, (((Slot *)(self + 0x440))[i].node = node));
        *(int *)((char *)((Slot *)(self + 0x440))[i].node + 0x5c) |= 2;
    }

    *(void **)(self + 0x3c4) = JointModel_New(Ov107_PackTextureHandle(self, 0xe), 0x22);
    Ov107_EnqueueValue(self, *(void **)(self + 0x3c4));
    *(int *)(*(char **)(self + 0x3c4) + 0x5c) |= 2;
    *(void **)(self + 0x3c0) = Ov216_createRegistryEntryStoreField(self);

    seed = *(SpawnSeed *)(self + 0x64);
    seed.v = data_02041dc8;
    *(void **)(self + 0x3ac) = List_InsertSorted(self + 0x22c, 0x10, 100);
    **(void ***)(self + 0x3ac) = Ov107_CloneResourceTransform(&seed);
    {
        void **slot = (void **)List_InsertSorted(self + 0x144, 4, 100);
        void *node = Ov107_CloneResourceTransform(&seed);
        *slot = node;
        *(void **)(self + 0x3b0) = node;
    }
    Res_RequestIdPair(0x144);
}


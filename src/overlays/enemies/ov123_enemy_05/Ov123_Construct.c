/* Constructor of the ov123 enemy (and its byte-identical twin): raises bit 8 of the +0 flags,
 * installs the twelve handlers (+8 tick, +0xc veneer, +0x1c message, +0x28/+0x2c/+0x30/+0x34/
 * +0x38 callbacks, +0x10 draw, +0x1e0 release, +0x1d0 on-hit, +0x1dc finish), seeds the +0x64
 * pose (scale 0x800, y 0x800) and bit 4 of the +0x1ae flags, builds the primary item from pool
 * entry 0 (subscribed), resolves the named bone (+0x390), the two sub-items of the +0xe298 pool
 * pair (+0x398, attached, bit 1 on their +0x5c), configures actions 0/1/2/4 with the +0xe2a0
 * offset and the 0x3000/0x1800/0x1800/0x1800 rates, then creates two capsule placements from
 * one request (position (0, 0x800, 0), world Y axis, radius and height 0x800): +0x38c on the
 * +0x144 list, +0x388 on the +0x22c list; +0x394 is built by cd4cc and sound 0x115 is loaded. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Ov153Capsule {
    VecFx32 vPos;
    VecFx32 vUp;
    int nRadius;
    int nHeight;
};

struct Ov153SubitemSlot {
    void *subitem;
    int pad;
};

extern const int data_ov123_020ce298[2];
extern VecFx32 data_ov123_020ce2a0;
extern const VecFx32 data_02042264;
extern void Ov123_Model_SetTracks0And3(void);
extern const char data_ov123_020ce30c[];

extern void Ov123_ReleaseSubObjectsListThenNotify(void);
extern void func_ov123_020cc294(void);   /* the game's tail-call veneer to the ov107 draw hook, named after the byte-identical SDK thunk */
extern void Ov123_HandleMessage(void);
extern void Ov123_CreateRegistryEntryAndLink(void);
extern void Ov123_ForwardRegionEventToParts(void);
extern void Ov123_NotifyPartsThenBase(void);
extern void Ov123_RefreshPose(void);
extern void Ov123_Tick(void);
extern void Ov123_Slot38_SetFlag3C8(void);
extern void Ov123_TryClaimReactionSlot(void);
extern void Ov123_OnHit(void);
extern void *Ov123_Actor_New(int *self);

extern void *Ov107_PackTextureHandle();
extern void *CreateSubitemInstance0xB4();
extern void RegisterSubscriberSlot();
extern void *InsertSortedEntryWithKey();
extern void *CallocInstance();
extern void Ov107_EnqueueValue();
extern void Ov107_Actor_SetAttachSlot();
extern void *List_InsertSorted();
extern int Ov107_Mover_New(struct Ov153Capsule *req);
extern void Res_RequestIdPair(int nId);

void Ov123_Construct(int param)
{
    VecFx32 offset;
    struct Ov153Capsule req;
    int kinds[2];
    int i;
    int resource;

    kinds[0] = data_ov123_020ce298[0];
    kinds[1] = data_ov123_020ce298[1];
    offset = data_ov123_020ce2a0;

    *(u16 *)param |= 0x100;
    *(void **)(param + 0x08) = Ov123_ReleaseSubObjectsListThenNotify;
    *(void **)(param + 0x0c) = func_ov123_020cc294;
    *(void **)(param + 0x1c) = Ov123_HandleMessage;
    *(void **)(param + 0x30) = Ov123_CreateRegistryEntryAndLink;
    *(void **)(param + 0x28) = Ov123_ForwardRegionEventToParts;
    *(void **)(param + 0x2c) = Ov123_NotifyPartsThenBase;
    *(void **)(param + 0x10) = Ov123_RefreshPose;
    *(void **)(param + 0x34) = Ov123_Tick;
    *(void **)(param + 0x38) = Ov123_Slot38_SetFlag3C8;
    *(void **)(param + 0x1e0) = Ov123_TryClaimReactionSlot;
    *(void **)(param + 0x1d0) = Ov123_OnHit;
    *(void **)(param + 0x1dc) = Ov123_Model_SetTracks0And3;

    *(int *)(param + 0x70) = 0x800;
    *(int *)(param + 0x64) = 0;
    *(int *)(param + 0x68) = 0x800;
    *(int *)(param + 0x6c) = 0;
    *(u16 *)(param + 0x100 + 0xae) |= 0x10;

    {
        int *self = (int *)param;

        ((void **)self)[0xe1] = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
        RegisterSubscriberSlot(self[0x27], ((void **)self)[0xe1]);
        ((void **)self)[0xe4] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov123_020ce30c);
        ((void **)self)[0xe6] = CallocInstance(0x10);

        for (i = 0; i < 2; i++) {
            ((struct Ov153SubitemSlot *)((void **)self)[0xe6])[i].subitem =
                CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, kinds[i]));
            Ov107_EnqueueValue(self,
                ((struct Ov153SubitemSlot *)((void **)self)[0xe6])[i].subitem);
            *(int *)((char *)((struct Ov153SubitemSlot *)
                ((void **)self)[0xe6])[i].subitem + 0x5c) |= 2;
        }

        Ov107_Actor_SetAttachSlot(self, 0, 1, &offset, 0x3000);
        Ov107_Actor_SetAttachSlot(self, 1, 1, &offset, 0x1800);
        Ov107_Actor_SetAttachSlot(self, 2, 1, &offset, 0x1800);
        Ov107_Actor_SetAttachSlot(self, 4, 1, &offset, 0x1800);

        req.vPos.x = 0;
        req.vPos.z = 0;
        req.vPos.y = 0x800;
        req.vUp = data_02042264;
        req.nRadius = 0x800;
        req.nHeight = 0x800;
        {
            int *p = List_InsertSorted(self + 0x51, 4, 100);
            resource = Ov107_Mover_New(&req);
            *p = resource;
            self[0xe3] = resource;
        }
        ((void **)self)[0xe2] = List_InsertSorted(self + 0x8b, 0x10, 100);
        *((int **)self)[0xe2] = Ov107_Mover_New(&req);
        ((void **)self)[0xe5] = Ov123_Actor_New(self);
        Res_RequestIdPair(0x115);
    }
}

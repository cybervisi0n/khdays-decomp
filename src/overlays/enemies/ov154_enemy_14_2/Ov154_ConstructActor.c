/* Big constructor of the ov153 enemy (x3: ov153/154/155): raises bit 8 of the +0 flags, installs
 * the eleven handlers (+8 tick, +0xc/+0x10 draw pair, +0x1c message, +0x28/+0x2c/+0x30/+0x34
 * hit callbacks, +0x1d0 on-hit, +0x1dc finish, +0x1e0 release), seeds the +0x64 pose (scale
 * 0x1400, y 0x1400) and bit 4 of the +0x1ae flags, builds the primary item from pool entry 0,
 * resolves the "headcon" bone (+0x38c), the two sub-items of kinds 1/2 (+0x394, attached, bit 1
 * on their +0x5c), configures actions 0/1/2/4 with the (0, 0xa00, 0) offset and the
 * 0x3e66/0x1f33/0x1f33/0x1f33 rates, then creates two capsule placements from one request
 * (position (0, 0xa00, 0), world Y axis, radius and height 0xa00): +0x390 on the +0x144 list,
 * +0x388 on the +0x22c list; +0x398 is built by Ov154_Actor_New and sound 0x13c is loaded. */

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

extern const int data_ov154_020d1c4c[2];
extern VecFx32 data_ov154_020d1c54;
extern const VecFx32 data_02042264;
extern const char data_ov154_020d1ccc[];

extern void Ov154_Destroy(void);
extern void func_ov154_020cfed0(void);   /* the game's tail-call veneer to the ov107 draw hook, named after the byte-identical SDK thunk */
extern void Ov154_HandleMessage(void);
extern void Ov154_CreateRegistryEntryAndLink_2(void);
extern void Ov154_ForwardRegionEventToParts(void);
extern void Ov154_NotifyPartsThenBase(void);
extern void Ov154_RefreshPose(void);
extern void Ov154_Tick(void);
extern void Ov154_TryBeginSubState4(void);
extern void Ov154_OnHit(void);
extern void Ov154_Model_SetTracks0And3(void);
extern void *Ov154_Actor_New(int *self);

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

void Ov154_ConstructActor(int param)
{
    struct Ov153Capsule req;
    VecFx32 offset;
    int kinds[2];
    int i;
    int resource;

    kinds[0] = data_ov154_020d1c4c[0];
    kinds[1] = data_ov154_020d1c4c[1];
    offset = data_ov154_020d1c54;

    *(u16 *)param |= 0x100;
    *(void **)(param + 0x08) = Ov154_Destroy;
    *(void **)(param + 0x0c) = func_ov154_020cfed0;
    *(void **)(param + 0x1c) = Ov154_HandleMessage;
    *(void **)(param + 0x30) = Ov154_CreateRegistryEntryAndLink_2;
    *(void **)(param + 0x28) = Ov154_ForwardRegionEventToParts;
    *(void **)(param + 0x2c) = Ov154_NotifyPartsThenBase;
    *(void **)(param + 0x10) = Ov154_RefreshPose;
    *(void **)(param + 0x34) = Ov154_Tick;
    *(void **)(param + 0x1e0) = Ov154_TryBeginSubState4;
    *(void **)(param + 0x1d0) = Ov154_OnHit;
    *(void **)(param + 0x1dc) = Ov154_Model_SetTracks0And3;

    *(int *)(param + 0x70) = 0x1400;
    *(int *)(param + 0x64) = 0;
    *(int *)(param + 0x68) = 0x1400;
    *(int *)(param + 0x6c) = 0;
    *(u16 *)(param + 0x100 + 0xae) |= 0x10;

    {
        int *self = (int *)param;

        ((void **)self)[0xe1] = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
        RegisterSubscriberSlot(self[0x27], ((void **)self)[0xe1]);
        ((void **)self)[0xe3] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov154_020d1ccc);
        ((void **)self)[0xe5] = CallocInstance(0x10);

        for (i = 0; i < 2; i++) {
            ((struct Ov153SubitemSlot *)((void **)self)[0xe5])[i].subitem =
                CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, kinds[i]));
            Ov107_EnqueueValue(self,
                ((struct Ov153SubitemSlot *)((void **)self)[0xe5])[i].subitem);
            *(int *)((char *)((struct Ov153SubitemSlot *)
                ((void **)self)[0xe5])[i].subitem + 0x5c) |= 2;
        }

        Ov107_Actor_SetAttachSlot(self, 0, 1, &offset, 0x3e66);
        Ov107_Actor_SetAttachSlot(self, 1, 1, &offset, 0x1f33);
        Ov107_Actor_SetAttachSlot(self, 2, 1, &offset, 0x1f33);
        Ov107_Actor_SetAttachSlot(self, 4, 1, &offset, 0x1f33);

        req.vUp = data_02042264;
        req.nRadius = 0xa00;
        req.nHeight = 0xa00;
        req.vPos.y = 0xa00;
        req.vPos.x = 0;
        req.vPos.z = 0;
        {
            int *p = List_InsertSorted(self + 0x51, 4, 100);
            resource = Ov107_Mover_New(&req);
            *p = resource;
            self[0xe4] = resource;
        }
        ((void **)self)[0xe2] = List_InsertSorted(self + 0x8b, 0x10, 100);
        *((int **)self)[0xe2] = Ov107_Mover_New(&req);
        ((void **)self)[0xe6] = Ov154_Actor_New(self);
        Res_RequestIdPair(0x13c);
    }
}

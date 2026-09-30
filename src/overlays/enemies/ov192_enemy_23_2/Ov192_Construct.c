/* Big constructor of the ov192 enemy (x3: ov192/192/193): raises bit 8 of the +0 flags, installs
 * the eleven handlers (+8 tick, +0xc/+0x10 draw pair, +0x1c message, +0x28/+0x2c/+0x30/+0x34
 * hit callbacks, +0x1d0 on-hit, +0x1dc finish, +0x1e0 release), seeds the +0x64 pose
 * (scale 0xc00, y 0xc00) and bit 4 of the +0x1ae flags, builds the primary item from pool entry
 * 0, resolves the "head02" and "headcon" bones (+0x398/+0x39c), the four sub-items of kinds
 * 1/2/3/6 (+0x3a0, attached, bit 1 on their +0x5c), configures actions 0/1/2/4 with the (0, 0x400,
 * 0) offset and the 0xd99/0x6cc/0x2ecc/0x2ecc rates, then creates four placements: two on the
 * +0x144 list (+0x394 at the origin, scale 0xc00; +0x390 at y 0xc00, scale 0xc00) and two on the
 * +0x22c list (+0x38c at the origin, scale 0xa00; +0x388 at y 0x800, scale 0x800, bit 1 on its
 * +8 flags), fills the +0x3a4 table with four Ov192_Actor_New records and loads sound 0x133. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Ov192Pose {
    VecFx32 position;
    int scale;
};

struct Ov192Kinds {
    int a;
    int b;
    int c;
    int d;
};

struct Ov192SubitemSlot {
    void *subitem;
    int pad;
};

struct bf {
    unsigned b : 8;
};

extern struct Ov192Kinds data_ov192_020d4b80;
extern VecFx32 data_ov192_020d4b74;
extern const VecFx32 data_02041dc8;
extern const char data_ov192_020d4c0c[];
extern const char data_ov192_020d4c14[];

extern void Ov192_DestroyArrayObjectsAndBuffersThenNotify(void);
extern void func_ov192_020d2040(void);
extern void Ov192_RefreshPose(void);
extern void Ov192_HandleSpawnMessage(void);
extern void Ov192_ForwardRegionEventToParts(void);
extern void Ov192_NotifyPartsThenBase(void);
extern void Ov192_CreateRegistryEntryAndLink(void);
extern void Ov192_Tick(void);
extern void Ov192_OnHit(void);
extern void Ov192_Model_SetTracks0And3(void);
extern void Ov192_TryBeginSubState7(void);
extern void *Ov192_Actor_New(int *self);

extern void *Ov107_PackTextureHandle();
extern void *CreateSubitemInstance0xB4();
extern void RegisterSubscriberSlot();
extern void *InsertSortedEntryWithKey();
extern void *CallocInstance();
extern void Ov107_EnqueueValue();
extern void Ov107_Actor_SetAttachSlot();
extern void *List_InsertSorted();
extern int Ov107_CloneResourceTransform();
extern void Res_RequestIdPair(int nId);

void Ov192_Construct(int param)
{
    struct Ov192Kinds kinds;
    struct Ov192Pose pose;
    VecFx32 offset;
    int i;
    int resource;

    kinds = data_ov192_020d4b80;
    offset = data_ov192_020d4b74;

    *(u16 *)param |= 0x100;
    *(void **)(param + 0x08) = Ov192_DestroyArrayObjectsAndBuffersThenNotify;
    *(void **)(param + 0x1c) = Ov192_HandleSpawnMessage;
    *(void **)(param + 0x30) = Ov192_CreateRegistryEntryAndLink;
    *(void **)(param + 0x28) = Ov192_ForwardRegionEventToParts;
    *(void **)(param + 0x2c) = Ov192_NotifyPartsThenBase;
    *(void **)(param + 0x0c) = func_ov192_020d2040;
    *(void **)(param + 0x10) = Ov192_RefreshPose;
    *(void **)(param + 0x34) = Ov192_Tick;
    *(void **)(param + 0x1e0) = Ov192_TryBeginSubState7;
    *(void **)(param + 0x1d0) = Ov192_OnHit;
    *(void **)(param + 0x1dc) = Ov192_Model_SetTracks0And3;

    *(int *)(param + 0x70) = 0xc00;
    *(int *)(param + 0x64) = 0;
    *(int *)(param + 0x68) = 0xc00;
    *(int *)(param + 0x6c) = 0;
    *(u16 *)(param + 0x100 + 0xae) |= 0x10;

    {
        int *self = (int *)param;

        ((void **)self)[0xe1] = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
        RegisterSubscriberSlot(self[0x27], ((void **)self)[0xe1]);
        ((void **)self)[0xe6] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov192_020d4c0c);
        ((void **)self)[0xe7] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov192_020d4c14);
        ((void **)self)[0xe8] = CallocInstance(0x20);

        for (i = 0; i < 4; i++) {
            ((struct Ov192SubitemSlot *)((void **)self)[0xe8])[i].subitem =
                CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ((int *)&kinds)[i]));
            Ov107_EnqueueValue(self,
                ((struct Ov192SubitemSlot *)((void **)self)[0xe8])[i].subitem);
            *(int *)((char *)((struct Ov192SubitemSlot *)
                ((void **)self)[0xe8])[i].subitem + 0x5c) |= 2;
        }

        Ov107_Actor_SetAttachSlot(self, 0, 1, &offset, 0x5d99);
        Ov107_Actor_SetAttachSlot(self, 1, 1, &offset, 0x2ecc);
        Ov107_Actor_SetAttachSlot(self, 2, 1, &offset, 0x2ecc);
        Ov107_Actor_SetAttachSlot(self, 4, 1, &offset, 0x2ecc);

        pose.position = data_02041dc8;
        pose.scale = 0xc00;
        {
            int *p = List_InsertSorted(self + 0x51, 4, 100);
            resource = Ov107_CloneResourceTransform(&pose);
            *p = resource;
            self[0xe5] = resource;
        }

        pose.position = data_02041dc8;
        pose.scale = 0xa00;
        {
            int **placementSlot = &((int **)self)[0xe3];
            int *placement;
            placement = List_InsertSorted(self + 0x8b, 0x10, 100);
            *placementSlot = placement;
            **placementSlot = Ov107_CloneResourceTransform(&pose);
        }

        pose.scale = 0xc00;
        pose.position.x = 0;
        pose.position.y = 0xc00;
        pose.position.z = 0;
        {
            int *p = List_InsertSorted(self + 0x51, 4, 100);
            resource = Ov107_CloneResourceTransform(&pose);
            *p = resource;
            self[0xe4] = resource;
        }

        pose.scale = 0x800;
        pose.position.x = 0;
        pose.position.y = 0x800;
        pose.position.z = 0;
        ((void **)self)[0xe2] = List_InsertSorted(self + 0x8b, 0x10, 100);
        *((int **)self)[0xe2] = Ov107_CloneResourceTransform(&pose);
        ((struct bf *)(((char **)self)[0xe2] + 8))->b |= 2;

        ((void **)self)[0xe9] = CallocInstance(0x10);
        for (i = 0; i < 4; i++) {
            ((void **)((void **)self)[0xe9])[i] = Ov192_Actor_New(self);
        }
        Res_RequestIdPair(0x133);
    }
}

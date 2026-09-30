/* Constructor of the ov194 enemy (x3: ov194/195/196): installs the handlers (+8 tick, +0xc
 * draw, +0x1c message, +0x30/+0x34 hit callbacks, +0x1d0 on-hit, +0x1e0 release, +0x1dc
 * finish), seeds the +0x64 pose (scale 0x1400, y 0x1400), builds the primary item from pool
 * entry 0 (subscribed), resolves its "Bone_head" (+0x394, mode 1) and "tag_00" (+0x398, mode
 * 3) joints, keeps the "move" motion handle (+0x3d0), the three sub-items of kinds 2/3/4 in a
 * fresh 24-byte slot table (+0x3d4, attached, bit 1 on their +0x5c), configures actions 0/1/2/4
 * (modes 1/1/1/1, rate 0x2851), resolves the "B" (+0x39c) and "guru0" (+0x3a0) joints of the
 * first sub-item, and creates two placements from a zero position with scale 0x10cc: +0x38c on
 * the +0x22c list and +0x390 on the +0x144 list, whose +0x20 block is kept in +0x2cc; sound
 * 0x134 is loaded. (The ov120 initializer shape: the +0x2cc store needs the tail in its own
 * block with its own `int *self` -- see Ov120_InitializeActor.) */

#include "nitro/fx_types.h"

struct Ov194Pose {
    VecFx32 position;
    int scale;
};

struct Ov194SubitemSlot {
    void *subitem;
    int pad;
};

extern VecFx32 data_ov195_020d2b8c;
extern VecFx32 data_02041dc8;
extern const char data_ov195_020d2c2c[];
extern const char data_ov195_020d2c38[];
extern const char data_ov195_020d2c40[];
extern const char data_ov195_020d2c48[];
extern const char data_ov195_020d2c4c[];

extern void Ov195_ReleaseSubObjectsAndListThenNotify(void);
extern void Ov195_Draw(void);
extern void Ov195_HandleMessage(void);
extern void Ov195_stCreateRegistryEntry(void);
extern void Ov195_ReleaseByStateAndPublishPose(void);
extern void Ov195_OnHit(void);
extern void Ov195_RequestSubState9IfIdle(void);
extern void Ov195_Model_SetTrack0(void);

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
extern void Res_RequestIdPair(int nId);

void Ov195_Construct(int param)
{
    VecFx32 kinds;
    struct Ov194Pose pose;
    int i;

    kinds = data_ov195_020d2b8c;

    *(void **)(param + 0x08) = Ov195_ReleaseSubObjectsAndListThenNotify;
    *(void **)(param + 0x0c) = Ov195_Draw;
    *(void **)(param + 0x1c) = Ov195_HandleMessage;
    *(void **)(param + 0x30) = Ov195_stCreateRegistryEntry;
    *(void **)(param + 0x34) = Ov195_ReleaseByStateAndPublishPose;
    *(void **)(param + 0x1d0) = Ov195_OnHit;
    *(void **)(param + 0x1e0) = Ov195_RequestSubState9IfIdle;
    *(void **)(param + 0x1dc) = Ov195_Model_SetTrack0;

    *(int *)(param + 0x70) = 0x1400;
    *(int *)(param + 0x64) = 0;
    *(int *)(param + 0x68) = 0x1400;
    *(int *)(param + 0x6c) = 0;

    {
        int *self = (int *)param;

        ((void **)self)[0xe1] = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
        RegisterSubscriberSlot(self[0x27], ((void **)self)[0xe1]);
        ((void **)self)[0xe5] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov195_020d2c2c);
        ((void **)self)[0xe6] = InsertSortedEntryWithKey(self[0xe1], 3, data_ov195_020d2c38);
        ((void **)self)[0xf4] = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(self, 1), data_ov195_020d2c40);
        ((void **)self)[0xf5] = CallocInstance(0x18);

        for (i = 0; i < 3; i++) {
            ((struct Ov194SubitemSlot *)((void **)self)[0xf5])[i].subitem =
                CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ((int *)&kinds)[i]));
            Ov107_EnqueueValue(self,
                ((struct Ov194SubitemSlot *)((void **)self)[0xf5])[i].subitem);
            *(int *)((char *)((struct Ov194SubitemSlot *)
                ((void **)self)[0xf5])[i].subitem + 0x5c) |= 2;
        }

        Ov107_Actor_SetAttachSlot(self, 0, 1, 0, 0x2851);
        Ov107_Actor_SetAttachSlot(self, 1, 1, 0, 0x2851);
        Ov107_Actor_SetAttachSlot(self, 2, 1, 0, 0x2851);
        Ov107_Actor_SetAttachSlot(self, 4, 1, 0, 0x2851);

        ((void **)self)[0xe7] = InsertSortedEntryWithKey(((struct Ov194SubitemSlot *)((void **)self)[0xf5])[0].subitem, 1, data_ov195_020d2c48);
        ((void **)self)[0xe8] = InsertSortedEntryWithKey(((struct Ov194SubitemSlot *)((void **)self)[0xf5])[0].subitem, 1, data_ov195_020d2c4c);

        pose.position = data_02041dc8;
        pose.scale = 0x10cc;

        ((void **)self)[0xe3] = List_InsertSorted(self + 0x8b, 0x10, 100);
        *((int **)self)[0xe3] = Ov107_CloneResourceTransform(&pose);
    }
    {
        int *self = (int *)param;
        int *p = List_InsertSorted(self + 0x51, 4, 100);
        int resource = Ov107_CloneResourceTransform(&pose);
        *p = resource;
        self[0xe4] = resource;
        ((char **)self)[0xb3] = (char *)resource + 0x20;
        Res_RequestIdPair(0x134);
    }
}

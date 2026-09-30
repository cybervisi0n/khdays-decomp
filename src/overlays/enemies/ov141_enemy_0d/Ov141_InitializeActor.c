/* ov141 actor initializer: install the callback table, seed the camera pose,
 * build the actor's rig, resolve two bone attachments, create the action
 * resource and five node subitems, configure four actions, seed the two pose
 * handles and open the actor's own sub-object.
 *
 * The two bones resolved through InsertSortedEntryWithKey are "7" (kind 3) and "root"
 * (kind 1); each returns a 0x30-byte attachment record. Field 0x2cc keeps the
 * body of the "root" record (its +0x14 block).
 *
 * The tail lives in its own block with its own `int *self` declaration. That is
 * not cosmetic: at function scope mwcc reserves r0/r1 for the trailing
 * Ov107_PackTextureHandle(self, 1) argument setup, the +0x14 offset lands in r2 and
 * the store sinks below the two movs. The block frees r0 and the original's
 * `add r0,r0,#0x14 ; str r0,[r6,#0x2cc]` comes back.
 */

#include "nitro/fx_types.h"

struct Ov141Pose {
    VecFx32 position;
    int scale;
};

struct Ov141SubitemSlot {
    void *subitem;
    int pad;
};

struct Ov141KindTable {
    int kind[5];
};

extern struct Ov141KindTable data_ov141_020ce9bc;
extern VecFx32 data_02041dc8;
extern const char data_ov141_020cea4c[];
extern const char data_ov141_020cea50[];
extern char data_ov141_020cea58[];

extern void Ov141_ReleaseSubObjectsAndListThenNotify(void);
extern void Ov141_TickAndSyncMarkerSrt(void);
extern void Ov141_HandleCommand(void);
extern void Ov141_CreateRegistryEntryAndLink(void);
extern void Ov141_ForwardEventToChild(void);
extern void Ov141_NotifyPartThenBase(void);
extern void Ov141_ReleaseByStateAndSyncSrt(void);
extern void Ov141_ApplyHit(void);
extern void Ov141_RequestSubState8IfIdle(void);
extern void Ov141_Model_SetTrack0(void);

extern void *Ov107_PackTextureHandle();
extern void *CreateSubitemInstance0xB4();
extern void RegisterSubscriberSlot();
extern void *InsertSortedEntryWithKey();
extern void *Ov107_CreateNamedResourceBinding();
extern void *CallocInstance();
extern void Ov107_EnqueueValue();
extern void Ov107_Actor_SetAttachSlot();
extern void *List_InsertSorted();
extern int Ov107_CloneResourceTransform();
extern void *Ov141_AllocLinkChild3a0();
extern void Res_RequestIdPair(int nId);

void Ov141_InitializeActor(int param)
{
    struct Ov141KindTable kinds;
    struct Ov141Pose pose;
    int i;
    int resource;

    kinds = data_ov141_020ce9bc;

    *(void **)(param + 0x08) = Ov141_ReleaseSubObjectsAndListThenNotify;
    *(void **)(param + 0x0c) = Ov141_TickAndSyncMarkerSrt;
    *(void **)(param + 0x1c) = Ov141_HandleCommand;
    *(void **)(param + 0x30) = Ov141_CreateRegistryEntryAndLink;
    *(void **)(param + 0x28) = Ov141_ForwardEventToChild;
    *(void **)(param + 0x2c) = Ov141_NotifyPartThenBase;
    *(void **)(param + 0x34) = Ov141_ReleaseByStateAndSyncSrt;
    *(void **)(param + 0x1d0) = Ov141_ApplyHit;
    *(void **)(param + 0x1e0) = Ov141_RequestSubState8IfIdle;
    *(void **)(param + 0x1dc) = Ov141_Model_SetTrack0;

    *(int *)(param + 0x70) = 0x800;
    *(int *)(param + 0x64) = 0;
    *(int *)(param + 0x68) = 0x800;
    *(int *)(param + 0x6c) = 0;

    {
        int *self = (int *)param;

        ((void **)self)[0xe1] = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
        RegisterSubscriberSlot(self[0x27], ((void **)self)[0xe1]);
        ((void **)self)[0xe5] = InsertSortedEntryWithKey(self[0xe1], 3, data_ov141_020cea4c);
        ((void **)self)[0xe6] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov141_020cea50);
        ((char **)self)[0xb3] = (char *)((void **)self)[0xe6] + 0x14;
        ((void **)self)[0xf3] = Ov107_CreateNamedResourceBinding(
            Ov107_PackTextureHandle(self, 1), data_ov141_020cea58);
        ((void **)self)[0xe4] = CallocInstance(0x28);

        for (i = 0; i < 5; i++) {
            ((struct Ov141SubitemSlot *)((void **)self)[0xe4])[i].subitem =
                CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, kinds.kind[i]));
            Ov107_EnqueueValue(self,
                ((struct Ov141SubitemSlot *)((void **)self)[0xe4])[i].subitem);
            *(int *)((char *)((struct Ov141SubitemSlot *)
                ((void **)self)[0xe4])[i].subitem + 0x5c) |= 2;
        }

        Ov107_Actor_SetAttachSlot(self, 0, 1, 0, 0x1f33);
        Ov107_Actor_SetAttachSlot(self, 1, 1, 0, 0x1f33);
        Ov107_Actor_SetAttachSlot(self, 2, 1, 0, 0x1f33);
        Ov107_Actor_SetAttachSlot(self, 4, 1, 0, 0x1f33);

        pose.position = data_02041dc8;
        pose.scale = 0x800;

        ((void **)self)[0xe2] = List_InsertSorted(self + 0x8b, 0x10, 100);
        *((int **)self)[0xe2] = Ov107_CloneResourceTransform(&pose);
        {
            int *p = List_InsertSorted(self + 0x51, 4, 100);
            resource = Ov107_CloneResourceTransform(&pose);
            *p = resource;
            self[0xe3] = resource;
        }
        ((void **)self)[0xf2] = Ov141_AllocLinkChild3a0(self);
        Res_RequestIdPair(0x11e);
    }
}

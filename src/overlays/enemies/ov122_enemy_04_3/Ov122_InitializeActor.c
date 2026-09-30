/* ov122 actor initializer: install the callback table, seed the camera pose,
 * build the actor's primary subitem, resolve four bone attachments, create the
 * action resource and three node subitems, configure four actions and seed the
 * two pose handles.
 *
 * The four bone names resolved through InsertSortedEntryWithKey are Bone_R_fing,
 * Bip01_R_Foot, Bip01_L_Foot and Bip01; each returns a 0x30-byte attachment
 * record.  Field 0x2cc keeps the body of the Bip01 record (its +0x14 block),
 * which is the only place in ov122 that writes it.
 *
 * Shared with ov120 and ov121: the same function with fourteen overlay-local
 * symbols substituted, the eight callbacks it installs and the six data
 * records it seeds from.
 *
 * CODEGEN NOTE -- the +0x14 store is a SCOPE problem, not a scheduling one.
 * Written at function scope, mwcc reserves r0/r1 for the trailing
 * Ov107_PackTextureHandle(self, 1) argument setup, so the offset lands in r2 and
 * the store sinks below the two movs.  Putting the whole tail in its own block
 * with its own `int *self` declaration (the idiom the matched ov281 homolog
 * uses) frees r0, and mwcc emits the ROM's `add r0,r0,#0x14 ; str r0,[r6,#0x2cc]`
 * with the argument movs after it.  Twenty-five source forms of the store
 * itself -- temporaries, post-increment, assignment expressions, volatile,
 * register hints, inline helper, declaration-order permutations -- are all
 * bit-identical and none of them move it. */

#include "nitro/fx_types.h"

struct Ov120Pose {
    VecFx32 position;
    int scale;
};

struct Ov120SubitemSlot {
    void *subitem;
    int pad;
};

struct Ov120Bone {
    unsigned char pad[0x14];
};

extern VecFx32 data_ov122_020d1b24;
extern VecFx32 data_02041dc8;
extern const unsigned short data_ov122_020d1b6c[];
extern const unsigned short data_ov122_020d1b78[];
extern const unsigned short data_ov122_020d1b88[];
extern const unsigned short data_ov122_020d1b98[];
extern char data_ov122_020d1ba0[];

extern void Ov122_Destroy(void);
extern void Ov122_ReleaseAndDestroy(void);
extern void Ov122_Actor_HandleEvent(void);
extern void Ov122_SpawnActorRegistryEntry(void);
extern void Ov122_SyncAttachedPose(void);
extern void Ov122_Actor_OnHit(void);
extern void Ov122_RequestSubState9IfIdle(void);
extern void Ov122_Model_SetTrack0(void);

extern void *Ov107_PackTextureHandle();
extern void *CreateSubitemInstance0xB4();
extern void RegisterSubscriberSlot();
extern struct Ov120Bone *InsertSortedEntryWithKey();
extern void *Ov107_CreateNamedResourceBinding();
extern void *CallocInstance();
extern void Ov107_EnqueueValue();
extern void Ov107_Actor_SetAttachSlot();
extern void *List_InsertSorted();
extern int Ov107_CloneResourceTransform();
extern void Res_RequestIdPair(int nId);

void Ov122_InitializeActor(int param)
{
    VecFx32 kinds;
    struct Ov120Pose pose;
    int i;
    int resource;

    kinds = data_ov122_020d1b24;

    *(void **)(param + 0x08) = Ov122_Destroy;
    *(void **)(param + 0x0c) = Ov122_ReleaseAndDestroy;
    *(void **)(param + 0x1c) = Ov122_Actor_HandleEvent;
    *(void **)(param + 0x30) = Ov122_SpawnActorRegistryEntry;
    *(void **)(param + 0x34) = Ov122_SyncAttachedPose;
    *(void **)(param + 0x1d0) = Ov122_Actor_OnHit;
    *(void **)(param + 0x1e0) = Ov122_RequestSubState9IfIdle;
    *(void **)(param + 0x1dc) = Ov122_Model_SetTrack0;

    *(int *)(param + 0x70) = 0x800;
    *(int *)(param + 0x64) = 0;
    *(int *)(param + 0x68) = 0x800;
    *(int *)(param + 0x6c) = 0;

    {
        int *self = (int *)param;

        ((void **)self)[0xe1] = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
        RegisterSubscriberSlot(self[0x27], ((void **)self)[0xe1]);
        ((void **)self)[0xe4] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov122_020d1b6c);
        ((void **)self)[0xe5] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov122_020d1b78);
        ((void **)self)[0xe6] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov122_020d1b88);
        ((void **)self)[0xe7] = InsertSortedEntryWithKey(self[0xe1], 1, data_ov122_020d1b98);
        ((char **)self)[0xb3] = (char *)((void **)self)[0xe7] + 0x14;
        ((void **)self)[0xe8] = Ov107_CreateNamedResourceBinding(
            Ov107_PackTextureHandle(self, 1), data_ov122_020d1ba0);
        ((void **)self)[0xe9] = CallocInstance(0x18);

        for (i = 0; i < 3; i++) {
            ((struct Ov120SubitemSlot *)((void **)self)[0xe9])[i].subitem =
                CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, ((int *)&kinds)[i]));
            Ov107_EnqueueValue(self,
                ((struct Ov120SubitemSlot *)((void **)self)[0xe9])[i].subitem);
            *(int *)((char *)((struct Ov120SubitemSlot *)
                ((void **)self)[0xe9])[i].subitem + 0x5c) |= 2;
        }

        Ov107_Actor_SetAttachSlot(self, 0, 1, 0, 0x1800);
        Ov107_Actor_SetAttachSlot(self, 1, 1, 0, 0x1800);
        Ov107_Actor_SetAttachSlot(self, 2, 1, 0, 0x1800);
        Ov107_Actor_SetAttachSlot(self, 4, 1, 0, 0x1800);

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
        Res_RequestIdPair(0x11a);
    }
}

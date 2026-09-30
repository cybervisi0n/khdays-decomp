/* Constructor of the ov210 enemy (x3 with ov211/ov282). Installs the handlers, clears +0x1f4,
 * sets the +0x64 pose (scale 1.25), raises bits 3 and 4 of +0x1ae, builds the +0x384 rig from
 * pose 0 (subscribed to +0x9c, 12-channel animation set 1 bound at +0x388) and raises bit 1 of the
 * parent's +0x5c; resolves five bones (+0x3bc in set 3, +0x3c0/+0x3c4/+0x3c8/+0x3cc in set 1) and
 * the "move" handle of set 0x1b (+0x3b8); builds the eight sub-items of data_ov210_020d4664 into
 * the +0x3d0 pair table (registered, bit 1 of +0x5c); registers reaction 2/3 (0x500); allocates the
 * +0x3d4 slot for the ov210 3cc8 child; reserves the +0x22c and +0x144 collision handles
 * (+0x3b0/+0x3b4) from a zero seed pointing up with scale 1.0 and radius 1.25, and loads sound
 * 0x117. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int w[8]; } PoseTable;
typedef struct { VecFx32 pos; VecFx32 up; int scale; int radius; } Seed;
struct Pair { int res; int handle; };

extern void *Ov107_PackTextureHandle(char *self, int kind);
extern int CreateSubitemInstance0xB4(void *res);
extern void RegisterSubscriberSlot(int list, int obj);
extern void Snd_RegisterSeqAndBind(void *set, int model, void *anim, int n);
extern void MainBlob_ResetSlotRows(int obj, void *set);
extern int InsertSortedEntryWithKey(int obj, int set, const char *name);
extern int Ov107_CreateNamedResourceBinding(void *res, const char *name);
extern void *CallocInstance(int size);
extern void Ov107_EnqueueValue(char *self, int obj);
extern void Ov107_Actor_SetAttachSlot(char *self, int a, int b, void *lift, int id);
extern int Ov210_ShieldPart_New(char *self);
extern int *List_InsertSorted(void *list, int stride, int max);
extern int Ov107_Mover_New(Seed *seed);
extern void Res_RequestIdPair(int id);
extern const PoseTable data_ov210_020d4664;
extern const char data_ov210_020d478c[];
extern const char data_ov210_020d4798[];
extern const char data_ov210_020d47a0[];
extern const char data_ov210_020d47ac[];
extern const char data_ov210_020d47bc[];
extern const char data_ov210_020d47cc[];
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042264;
extern void Ov210_Destroy(void);
extern void Ov210_TickWithChildRefresh(void);
extern void Ov210_ForwardRegionEventToShield(void);
extern void Ov210_NotifyShieldThenBase(void);
extern void Ov210_HandleMessage(void);
extern void Ov210_CreateAiTask(void);
extern void Ov210_Update(void);
extern void Ov210_HandleHit(void);
extern void Ov210_UpdateNodeReservationState(void);
extern void Ov210_MessageMapSetupSubObject(void);

void Ov210_Construct(char *self)
{
    PoseTable poses;
    Seed seed;
    int i;
    int *p;
    int v;

    poses = data_ov210_020d4664;
    *(void **)(self + 8) = (void *)Ov210_Destroy;
    *(void **)(self + 0xc) = (void *)Ov210_TickWithChildRefresh;
    *(void **)(self + 0x28) = (void *)Ov210_ForwardRegionEventToShield;
    *(void **)(self + 0x2c) = (void *)Ov210_NotifyShieldThenBase;
    *(void **)(self + 0x1c) = (void *)Ov210_HandleMessage;
    *(void **)(self + 0x30) = (void *)Ov210_CreateAiTask;
    *(void **)(self + 0x34) = (void *)Ov210_Update;
    *(void **)(self + 0x1d0) = (void *)Ov210_HandleHit;
    *(void **)(self + 0x1e0) = (void *)Ov210_UpdateNodeReservationState;
    *(void **)(self + 0x1dc) = (void *)Ov210_MessageMapSetupSubObject;
    *(int *)(self + 0x1f4) = 0;
    *(int *)(self + 0x70) = 0x1400;
    *(int *)(self + 0x64) = 0;
    *(int *)(self + 0x68) = 0x1400;
    *(int *)(self + 0x6c) = 0;
    *(u16 *)(self + 0x1ae) |= 0x18;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    Snd_RegisterSeqAndBind(self + 0x388, *(int *)(*(int *)(self + 0x384) + 0x88), Ov107_PackTextureHandle(self, 1), 0xc);
    MainBlob_ResetSlotRows(*(int *)(self + 0x384), self + 0x388);
    *(int *)(*(int *)(self + 0x9c) + 0x5c) |= 2;
    *(int *)(self + 0x3bc) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 3, data_ov210_020d478c);
    *(int *)(self + 0x3c0) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov210_020d4798);
    *(int *)(self + 0x3c4) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov210_020d47a0);
    *(int *)(self + 0x3c8) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov210_020d47ac);
    *(int *)(self + 0x3cc) = InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov210_020d47bc);
    *(int *)(self + 0x3b8) = Ov107_CreateNamedResourceBinding(Ov107_PackTextureHandle(self, 0x1b), data_ov210_020d47cc);
    *(void **)(self + 0x3d0) = CallocInstance(0x40);
    for (i = 0; i < 8; i++) {
        (*(struct Pair **)(self + 0x3d0))[i].res = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, poses.w[i]));
        Ov107_EnqueueValue(self, (*(struct Pair **)(self + 0x3d0))[i].res);
        *(int *)((*(struct Pair **)(self + 0x3d0))[i].res + 0x5c) |= 2;
    }
    Ov107_Actor_SetAttachSlot(self, 2, 3, 0, 0x500);
    *(void **)(self + 0x3d4) = CallocInstance(4);
    **(int **)(self + 0x3d4) = Ov210_ShieldPart_New(self);
    seed.pos = data_02041dc8;
    seed.up = data_02042264;
    seed.scale = 0x1000;
    seed.radius = 0x1400;
    *(int **)(self + 0x3b0) = List_InsertSorted(self + 0x22c, 0x10, 0x64);
    **(int **)(self + 0x3b0) = Ov107_Mover_New(&seed);
    p = List_InsertSorted(self + 0x144, 4, 0x64);
    v = (*p = Ov107_Mover_New(&seed));
    *(int *)(self + 0x3b4) = v;
    Res_RequestIdPair(0x117);
}

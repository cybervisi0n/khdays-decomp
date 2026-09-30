/* Constructor of an ov259 helper: installs its handlers (+8 update 020d1aa4, +0xc 020d1ad4, +0x1c
 * 020d1be0, +0x30 020d1c68, +0x34 020d1bec, +0x1d0 020d1c60), sets bits 1, 5 and 6 of the +0x60 high
 * byte and bits 3-4 of +0x1ae, +0x70 = 0x800, +0x64 rests at the origin, +0x54 / +0x58 / +0x398 clear.
 * Its body (+0x384) and shell (+0x388) models load from the owner's +0x394 kit (0x3c / 0x3e, or
 * 0x3d / 0x3f in the alternate costume, global 0204c240 bit 2); the owner's +0x410 takes the shell's
 * "tag00_1" node, both register with the +0x9c scene, the shell hides and the body shows at full
 * scale. A hit capsule (length 0x1a00, radius 0x400) goes into a +0x22c pool slot at +0x38c (bit 1
 * set) and a second one into a +0x144 slot, also kept at +0x390. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/enemy_common.h"

typedef struct { unsigned f : 8; } B8;
typedef struct { VecFx32 pos; VecFx32 axis; int length; int radius; } Capsule;

extern int CreateSubitemInstance0xB4(int item);
extern int InsertSortedEntryWithKey(int item, int a, const char *name);
extern int RegisterSubscriberSlot(int subscriber, int item);
extern void Srt_SetScaleUniform(int srt, int scale);
extern int *List_InsertSorted(void *pool, int elementSize, int capacity);
extern void *Ov107_Mover_New(const Capsule *capsule);
extern void Ov259_ReleaseSubObjects(void);
extern void Ov259_HelperUpdate(void);
extern void func_ov259_020d1be0(void);
extern void Ov259_Helper_CreateAiTask(void);
extern void func_ov259_020d1bec(void);
extern void Ov259_OnHitIgnore(void);
extern const VecFx32 data_02041dc8;
extern const VecFx32 data_02042258;
extern u8 data_0204c240;
extern char data_ov259_020d2fdc[];

void Ov259_HelperInit(char *self)
{
    char *owner = *(char **)(self + 0x394);
    VecFx32 origin;
    Capsule cap;

    *(void **)(self + 8) = Ov259_ReleaseSubObjects;
    *(void **)(self + 0xc) = Ov259_HelperUpdate;
    *(void **)(self + 0x1c) = func_ov259_020d1be0;
    *(void **)(self + 0x30) = Ov259_Helper_CreateAiTask;
    *(void **)(self + 0x34) = func_ov259_020d1bec;
    *(void **)(self + 0x1d0) = Ov259_OnHitIgnore;
    {
        u16 hw = *(u16 *)(self + 0x60);
        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x62) << 0x18) >> 0x10);
    }
    *(u16 *)(self + 0x1ae) |= 8;
    {
        u16 hw = *(u16 *)(self + 0x60);
        *(u16 *)(self + 0x60) = (hw & ~0xff00) |
            ((((((unsigned int)hw << 0x10) >> 0x18) | 0x20) << 0x18) >> 0x10);
    }
    *(u16 *)(self + 0x1ae) |= 0x10;
    *(int *)(self + 0x70) = 0x800;
    origin = data_02041dc8;
    *(VecFx32 *)(self + 0x64) = data_02041dc8;
    *(int *)(self + 0x54) = 0;
    *(int *)(self + 0x58) = 0;
    *(int *)(self + 0x398) = 0;
    *(int *)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(owner, (data_0204c240 & 4) ? 0x3d : 0x3c));
    *(int *)(self + 0x388) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(owner, (data_0204c240 & 4) ? 0x3f : 0x3e));
    *(int *)(*(char **)(self + 0x394) + 0x410) = InsertSortedEntryWithKey(*(int *)(self + 0x388), 3, data_ov259_020d2fdc);
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x384));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(int *)(self + 0x388));
    Srt_SetScaleUniform(*(int *)(self + 0x388) + 4, 0);
    Srt_SetScaleUniform(*(int *)(self + 0x384) + 4, 0x1000);
    cap.pos = origin;
    cap.axis = data_02042258;
    cap.length = 0x1a00;
    cap.radius = 0x400;
    *(int **)(self + 0x38c) = List_InsertSorted(self + 0x22c, 0x10, 100);
    **(void ***)(self + 0x38c) = Ov107_Mover_New(&cap);
    ((B8 *)(*(char **)(self + 0x38c) + 8))->f |= 2;
    {
        void **slot = (void **)List_InsertSorted(self + 0x144, 4, 100);

        *(void **)(self + 0x390) = *slot = Ov107_Mover_New(&cap);
    }
}

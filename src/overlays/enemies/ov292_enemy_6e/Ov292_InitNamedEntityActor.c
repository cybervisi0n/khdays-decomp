/* ov292 actor constructor: install the nine handler entries, set the bounding
 * box, seed the placement and open the sub-item.
 *
 * Same family as the matched ov189 and ov187 constructors and written in their
 * idioms: the six words at +0x1fc are a bounding box -- a pair of Vec3 holding
 * the minimum and the maximum, here a two-unit box standing on the ground --
 * the sub-item is created as CreateSubitemInstance0xB4(Ov107_PackTextureHandle(...)) in one
 * expression, and the flag byte at +0x61 is raised with the family's explicit
 * insert rather than a bitfield, because a 16-bit bitfield would add a second
 * truncation the ROM does not have.
 *
 * Two things in here are load-bearing for the codegen. The zero vector must be
 * const: only then may the compiler hoist its three-word load above the store
 * to the flag halfword, which is what makes the function need a third
 * callee-saved register and gives the ROM's push list. And the box has to be
 * filled before the handler entries go in, exactly as the ov189 sibling does
 * it; filled afterwards the whole prologue schedules one slot late.
 */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

struct Placement { VecFx32 vec; int scale; };
struct Box { VecFx32 min, max; };

extern const VecFx32 data_02041dc8;
extern unsigned short data_ov292_020d48cc[];

extern void Ov292_ReleaseSubObjectAndListThenNotify(void);
extern void func_ov292_020d3ab4(void);
extern void func_ov292_020d3ac0(void);
extern void Ov292_CreateNodeRegistryEntry(void);
extern void Ov292_PropagateBlockChainThenNotify(void);
extern void Ov292_ReloadPointList(void);
extern void Ov292_HandleHitEvent(void);
extern void Ov292_Model_SetTrack0(void);
extern void Ov292_RequestSubState7IfNotCurrent(void);

extern void *CreateSubitemInstance0xB4();
extern void RegisterSubscriberSlot();
extern void Srt_SetTranslationXYZ();
extern void *InsertSortedEntryWithKey();
extern void Ov107_Actor_SetAttachSlot();
extern void *List_InsertSorted();
extern int Ov107_CloneResourceTransform();
extern void Res_RequestIdPair(int nId);
extern void List_Init();

void Ov292_InitNamedEntityActor(char *self)
{
    VecFx32 zero;
    struct Box box;
    struct Placement place;

    box.min.x = -0x2000;
    box.min.y = 0;
    box.min.z = -0x2000;
    box.max.x = 0x2000;
    box.max.y = 0x2000;
    box.max.z = 0x2000;
    *(void **)(self + 0x08) = Ov292_ReleaseSubObjectAndListThenNotify;
    *(void **)(self + 0x0c) = func_ov292_020d3ab4;
    *(void **)(self + 0x1c) = func_ov292_020d3ac0;
    *(void **)(self + 0x30) = Ov292_CreateNodeRegistryEntry;
    *(void **)(self + 0x34) = Ov292_PropagateBlockChainThenNotify;
    *(void **)(self + 0x38) = Ov292_ReloadPointList;
    *(void **)(self + 0x1d0) = Ov292_HandleHitEvent;
    *(void **)(self + 0x1dc) = Ov292_Model_SetTrack0;
    *(void **)(self + 0x1e0) = Ov292_RequestSubState7IfNotCurrent;

    *(struct Box *)(self + 0x1fc) = box;
    *(unsigned short *)(self + 0x1ae) |= 8;
    {
        unsigned short *p = (unsigned short *)(self + 0x60);
        unsigned int u = *p;
        *p = (unsigned short)((u & ~0xff00) | ((((u << 0x10) >> 0x18 | 0x40) << 0x18) >> 0x10));
    }
    *(int *)(self + 0x70) = 0x1800;
    zero = data_02041dc8;
    *(VecFx32 *)(self + 0x64) = zero;

    *(void **)(self + 0x384) = CreateSubitemInstance0xB4(Ov107_PackTextureHandle(self, 0));
    RegisterSubscriberSlot(*(int *)(self + 0x9c), *(void **)(self + 0x384));
    Srt_SetTranslationXYZ(*(int *)(self + 0x384) + 4, 0, -0x1800, 0);
    *(void **)(self + 0x38c) =
        InsertSortedEntryWithKey(*(int *)(self + 0x384), 1, data_ov292_020d48cc);

    Ov107_Actor_SetAttachSlot(self, 0, 1, 0, 0x6000);
    Ov107_Actor_SetAttachSlot(self, 2, 1, 0, 0x6000);
    Ov107_Actor_SetAttachSlot(self, 1, 1, 0, 0x6000);
    Ov107_Actor_SetAttachSlot(self, 4, 1, 0, 0x6000);

    place = *(struct Placement *)(self + 0x64);
    place.vec = zero;

    *(void **)(self + 0x388) = List_InsertSorted(self + 0x22c, 0x10, 100);
    **(int **)(self + 0x388) = Ov107_CloneResourceTransform(&place);
    {
        int *slot = List_InsertSorted(self + 0x144, 4, 100);
        int handle = Ov107_CloneResourceTransform(&place);
        *slot = handle;
        *(int *)(self + 0x390) = handle;
    }
    Res_RequestIdPair(0x175);
    List_Init(self + 0x394);
}

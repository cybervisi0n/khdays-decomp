
/* What a named place resolves to: its point and the heading that goes with
   it. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct Ov002PlaceResult {
    char pad000[8];
    VecFx32 place;
    int nExtra;
} Ov002PlaceResult;

extern int ScriptVm_ReadOperandInt(void *pCtx, int nArgs);
extern int ScriptVm_ReadOperandFx32(void *pCtx, int nArgs);
extern char *ByteCode_ResolveOperand(void *pCtx, int nArgs);
extern int func_02020400(int a, int b);
extern Ov002PlaceResult *EntityMgr_FindCollEntry(int nSlot, const char *pKey);
extern void Ov002_ApplyRosterSlotToNode(int nIndex, void *pPlace, int nAngle);

/* Script VM command: put one roster slot at a place.
 *
 * Operand slots are eight bytes each and the leading halfword is the kind tag.
 * When the second operand is left out the command carries the place itself --
 * three fixed point coordinates and a heading in degrees, turned into a
 * rotation through the same 0x168 divisor the other place commands use.  When
 * it is present it names a place instead, and both the point and the heading
 * come from whatever that name resolves to.
 *
 * Always returns 1.
 */
int Ov002_ScriptPlaceSlot(void *pCtx, int nArgs)
{
    VecFx32 vPlace;
    int nIndex;
    int nAngle;
    Ov002PlaceResult *pFound;

    nIndex = ScriptVm_ReadOperandInt(pCtx, nArgs);
    if (*(short *)((char *)nArgs + 8) == 0) {
        vPlace.x = ScriptVm_ReadOperandFx32(pCtx, nArgs + 0x10);
        vPlace.y = ScriptVm_ReadOperandFx32(pCtx, nArgs + 0x18);
        vPlace.z = ScriptVm_ReadOperandFx32(pCtx, nArgs + 0x20);
        nArgs += 0x28;
        nAngle = (u16)func_02020400(ScriptVm_ReadOperandInt(pCtx, nArgs) << 0x10,
                                    0x168);
    } else {
        nArgs += 8;
        pFound = EntityMgr_FindCollEntry(0, ByteCode_ResolveOperand(pCtx, nArgs));
        vPlace = pFound->place;
        nAngle = pFound->nExtra;
    }

    Ov002_ApplyRosterSlotToNode(nIndex, &vPlace, nAngle);
    return 1;
}

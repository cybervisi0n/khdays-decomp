/* Ov015_ScriptOpSpawnTrigger -- Ov015_ScriptOpSpawnTrigger: script op that reads a slot, a kind,
 * an index, the raw word at pc + 0x1c (low half the GameState field, next byte its bit),
 * a parameter word, the centre (three fx32), a shape, the extent (three fx32) and the
 * name (a resolved operand), then spawns a trigger piece (0208061c) on the slot's class
 * table (ov002 02076468).  Always consumes the op (1). */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern int   ScriptVm_ReadOperandInt(int vm, u16 *pc);            /* ScriptVm_ReadOperandInt */
extern int   ScriptVm_ReadOperandFx32(int vm, u16 *pc);            /* ScriptVm_ReadOperandFx32 */
extern void *ByteCode_ResolveOperand(int vm, u16 *pc);            /* ByteCode_ResolveOperand */
extern void *Ov002_GetModuleSlot(int nSlot);            /* class table of a slot */
extern void *Ov015_SpawnTrigger(void *pClass, int nSlot, int nKind, int nField, u8 nBit, VecFx32 *pCentre, int nParam, s8 nShape, VecFx32 *pExtent, const char *pName);

int Ov015_ScriptOpSpawnTrigger(int vm, u16 *pc)
{
    VecFx32 centre;
    VecFx32 extent;
    int nSlot;
    u32 nKind;
    u32 nIndex;
    u32 nFieldBit;
    int nParam;
    u32 nShape;
    const char *pName;
    u16 nBitHalf;

    nSlot = ScriptVm_ReadOperandInt(vm, pc);
    nKind = ScriptVm_ReadOperandInt(vm, pc + 4);
    nIndex = ScriptVm_ReadOperandInt(vm, pc + 8);
    nFieldBit = *(u32 *)(pc + 0xe);
    nParam = ScriptVm_ReadOperandInt(vm, pc + 0x10);
    centre.x = ScriptVm_ReadOperandFx32(vm, pc + 0x14);
    centre.y = ScriptVm_ReadOperandFx32(vm, pc + 0x18);
    centre.z = ScriptVm_ReadOperandFx32(vm, pc + 0x1c);
    nShape = ScriptVm_ReadOperandInt(vm, pc + 0x20);
    extent.x = ScriptVm_ReadOperandFx32(vm, pc + 0x24);
    extent.y = ScriptVm_ReadOperandFx32(vm, pc + 0x28);
    extent.z = ScriptVm_ReadOperandFx32(vm, pc + 0x2c);
    pName = ByteCode_ResolveOperand(vm, pc + 0x30);
    nBitHalf = nFieldBit >> 16;
    Ov015_SpawnTrigger(Ov002_GetModuleSlot(nSlot), nKind & 0xffff, nIndex & 0xffff, nFieldBit & 0xffff, nBitHalf, &centre, nParam, (s8)nShape, &extent, pName);
    return 1;
}

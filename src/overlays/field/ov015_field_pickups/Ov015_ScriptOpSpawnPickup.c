/* Ov015_ScriptOpSpawnPickup -- Ov015_ScriptOpSpawnPickup: script op that reads a slot, a kind,
 * an index, two packed GameState field / bit words (pc + 0x1c and pc + 0x24), the
 * position (three fx32), an angle in degrees (turned into a 16-bit facing by
 * angle * 0x10000 / 360), a link-key count and that many link keys (from pc + 0x50, one
 * operand each), then spawns a pickup piece (020801c8) on the slot's class table (ov002
 * 02076468).  Always consumes the op (1).
 * Codegen: the operand pointer is advanced past the fixed operands before the key count is
 * read (through a pointer taken first) and then walks the keys; the packed words are split
 * into u16 / u8 locals as they are read; the vectors are declared before the scalars. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern int   ScriptVm_ReadOperandInt(int vm, u16 *pc);            /* ScriptVm_ReadOperandInt */
extern int   ScriptVm_ReadOperandFx32(int vm, u16 *pc);            /* ScriptVm_ReadOperandFx32 */
extern long long func_02020400(int nNumerator, int nDenominator); /* _s32_div_f */
extern void *Ov002_GetModuleSlot(int nSlot);            /* class table of a slot */
extern void *Ov015_SpawnPickup(void *pClass, int nSlot, int nKind, u16 nField, u8 nBit, u16 nField2, u8 nBit2, VecFx32 *pPos, short nFacing, int nKeys, short *aKey);

int Ov015_ScriptOpSpawnPickup(int vm, u16 *pc)
{
    VecFx32 position;
    short aKey[2];
    short nFacing;
    u16 nField;
    u8 nBit;
    u16 nField2;
    u8 nBit2;
    int nSlot;
    u32 nKind;
    u32 nIndex;
    int nKeys;
    u32 nRaw;
    int i;
    u16 *pKey;
    short *pDst;

    nSlot = ScriptVm_ReadOperandInt(vm, pc);
    nKind = ScriptVm_ReadOperandInt(vm, pc + 4);
    nIndex = ScriptVm_ReadOperandInt(vm, pc + 8);
    nRaw = *(u32 *)(pc + 0xe);
    nField = nRaw;
    nBit = (u16)(nRaw >> 16);
    nRaw = *(u32 *)(pc + 0x12);
    nField2 = nRaw;
    nBit2 = (u16)(nRaw >> 16);
    position.x = ScriptVm_ReadOperandFx32(vm, pc + 0x14);
    position.y = ScriptVm_ReadOperandFx32(vm, pc + 0x18);
    position.z = ScriptVm_ReadOperandFx32(vm, pc + 0x1c);
    nFacing = (short)func_02020400(ScriptVm_ReadOperandInt(vm, pc + 0x20) << 16, 360);
    pKey = pc + 0x24;
    pc += 0x28;
    nKeys = ScriptVm_ReadOperandInt(vm, pKey);
    i = 0;
    if (nKeys > 0) {
        pDst = aKey;
        do {
            *pDst = ScriptVm_ReadOperandInt(vm, pc);
            i++;
            pc += 4;
            pDst++;
        } while (i < nKeys);
    }
    Ov015_SpawnPickup(Ov002_GetModuleSlot(nSlot), nKind & 0xffff, nIndex & 0xffff, nField, nBit, nField2, nBit2, &position, nFacing, nKeys, aKey);
    return 1;
}

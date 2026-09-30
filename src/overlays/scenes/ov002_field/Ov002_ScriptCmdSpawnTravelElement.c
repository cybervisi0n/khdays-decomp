
/* One operand of a script command: a tag saying how the value is fetched and
 * the word that carries either the value itself or the reference to it. */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct {
    short kind;
    short pad;
    int   value;
} OperandSlot;

extern void *Ov002_GetModuleSlot(int nModule);
extern int func_02020400(int nNumerator, int nDenominator);
extern void Ov002_SpawnTravelElement(void *pClass, u16 wA, u16 wB, u16 wC,
                                u8 bHigh, VecFx32 *pPos, short nHeading,
                                short nLast);

/* Script command: spawn a travel element.
 *
 * Nine operands - a module slot, two keys, a packed word, three fixed point
 * coordinates, a heading in degrees and a trailing key. The fourth operand's
 * word is read straight out of the descriptor instead of through an accessor,
 * and it supplies two of the spawn arguments: its low halfword and the byte
 * above it.
 *
 * That byte is narrowed twice on purpose. The original truncates to a halfword
 * and only then to a byte, so a single (u8) cast is two instructions short.
 *
 * The heading is degrees shifted into fixed point and divided by 360.
 *
 * Always returns 1.
 */
int Ov002_ScriptCmdSpawnTravelElement(void *pContext, OperandSlot *pArgs)
{
    VecFx32 vPos;
    int nModule;
    int nA;
    int nB;
    int nPacked;
    int nDegrees;
    int nLast;
    void *pClass;
    int nHeading;

    nModule = ScriptVm_ReadOperandInt(pContext, &pArgs[0]);
    nA = ScriptVm_ReadOperandInt(pContext, &pArgs[1]);
    nB = ScriptVm_ReadOperandInt(pContext, &pArgs[2]);
    nPacked = pArgs[3].value;
    vPos.x = ScriptVm_ReadOperandFx32(pContext, &pArgs[4]);
    vPos.y = ScriptVm_ReadOperandFx32(pContext, &pArgs[5]);
    vPos.z = ScriptVm_ReadOperandFx32(pContext, &pArgs[6]);
    nDegrees = ScriptVm_ReadOperandInt(pContext, &pArgs[7]);
    pArgs += 8;
    nLast = ScriptVm_ReadOperandInt(pContext, pArgs);

    pClass = Ov002_GetModuleSlot(nModule);
    nHeading = func_02020400(nDegrees << 0x10, 0x168);
    Ov002_SpawnTravelElement(pClass, (u16)nA, (u16)nB, (u16)nPacked,
                        (u8)(u16)((unsigned int)nPacked >> 0x10), &vPos,
                        (short)nHeading, (short)nLast);
    return 1;
}


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

/* The packet published as session command 0x10.
 *
 * The two bitfields share the byte at +1 and are written at different points -
 * the style before the position, the slot after it - so each write is a
 * read-modify-write of that same byte, masking 0xfc and 3 respectively. */
typedef struct {
    u8 nKind;                       /* +0x00, not written by the publisher */
    u8 nSlot : 2;                   /* +0x01 bits 0-1 */
    u8 nStyle : 6;                  /* +0x01 bits 2-7 */
    u8 nEntry;                      /* +0x02 */
    u8 nKeyIndex;                   /* +0x03 */
    u8 nSeed;                       /* +0x04 */
    u8 pad05[3];
    VecFx32 vPos;                   /* +0x08 */
} Ov002SessionSpawnCmd;             /* 0x14 */

extern int Ov002_TakeEntryOfKind1(void);
extern int Ov002_FindKeyEntryIndex(int nKey);
extern void Ov002_BuildSessionCommand(int nKind, void *pCmd);

/* Script command: read a spawn request out of the command's operands and, on
 * the session host only, publish it as session command 0x10.
 *
 * Seven operands: one read purely for its side effect, a key to look up, a
 * style, three fixed point coordinates and a slot. A client reads all of them
 * too and then drops the request, so both sides consume the same operands.
 *
 * Always returns 1.
 */
int Ov002_ScriptCmdPublishSpawnRequest(void *pContext, OperandSlot *pArgs)
{
    VecFx32 vPos;
    Ov002SessionSpawnCmd cmd;
    int nKey;
    int nStyle;
    int nSlot;

    ScriptVm_ReadOperandInt(pContext, &pArgs[0]);
    nKey = ScriptVm_ReadOperandInt(pContext, &pArgs[1]);
    nStyle = ScriptVm_ReadOperandInt(pContext, &pArgs[2]);
    vPos.x = ScriptVm_ReadOperandFx32(pContext, &pArgs[3]);
    vPos.y = ScriptVm_ReadOperandFx32(pContext, &pArgs[4]);
    vPos.z = ScriptVm_ReadOperandFx32(pContext, &pArgs[5]);
    pArgs += 6;
    nSlot = ScriptVm_ReadOperandInt(pContext, pArgs);

    if (Session_GetLocalPlayerIndex() == 0) {
        cmd.nEntry = (u8)Ov002_TakeEntryOfKind1();
        cmd.nKeyIndex = (u8)Ov002_FindKeyEntryIndex((short)nKey);
        cmd.nStyle = (u8)nStyle;
        cmd.vPos = vPos;
        cmd.nSlot = (u8)nSlot;
        cmd.nSeed = (u8)Session_RandNextScaled(0x100);
        Ov002_BuildSessionCommand(0x10, &cmd);
    }
    return 1;
}


#include "nitro/types.h"
#include "nitro/fx_types.h"

extern int Ov002_LookupChannelEntry(void *pName);
extern void RegisterSeqAndInit(char *pNode, int nRes, int a, int b);
extern void Ov002_ElementStartTrack(char *pElement, short *pAnim, int nTrack,
                                int nParamA, int nParamB, int bEffect);
extern void Res_RequestIdPair(int nId);

/* Bring a line element's second model up.
 *
 * The element is marked active first. With no model named by the owner there is
 * nothing to build, but the effect below still runs. Otherwise the node's
 * position is kept across the bind, put back afterwards, the angle is carried
 * over and the model is marked placed before its track starts with the values
 * the element already holds.
 */
void Ov002_ElementBuildSecondModel(char *pElement)
{
    char *pOwner;
    VecFx32 vPos;

    pOwner = *(char **)(pElement + 8);
    *(u16 *)(pElement + 0x12) |= 4;

    if (*(signed char *)(pOwner + 0x58) != 0) {
        vPos = *(VecFx32 *)(pElement + 0xe0);

        RegisterSeqAndInit(pElement + 0x1b0,
                      Ov002_LookupChannelEntry(pOwner + 0x58), 1, 4);

        *(VecFx32 *)(pElement + 0x254) = vPos;
        *(u16 *)(pElement + 0x22c) = *(u16 *)(pElement + 0x18);
        *(u16 *)(pElement + 0x1b0) |= 0x20;

        Ov002_ElementStartTrack(pElement, (short *)(pElement + 0x1b0),
                            *(u8 *)(pElement + 0x2c0),
                            *(int *)(pElement + 0x2bc),
                            *(int *)(pElement + 0x2b8), 0);
    }

    if (*(short *)(pOwner + 0x74) >= 0) {
        Res_RequestIdPair(*(short *)(pOwner + 0x74));
    }
}

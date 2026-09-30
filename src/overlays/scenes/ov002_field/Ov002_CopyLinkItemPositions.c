
#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

extern char *data_ov002_0207fa10;
extern u8 data_0204c240;                /* g_modeAndDayClock; bit 2 gates this */

extern int Ov002_IsSessionOpen(void);           /* is the tally live */
extern int Ov022_GetEntryField66(int nPeer);      /* peer -> kind, or negative */

/* Copies out where each of the local peer's link items sits and returns how
 * many there were.
 *
 * Only the boot mode that raises bit 2 has them, and only while the tally is
 * live.  The context keeps eight item slots per kind and a signed count per
 * kind alongside them; the count is re-read every pass, so an item that goes
 * away mid-copy shortens the walk.  A peer with no kind, or a kind with no
 * items, copies nothing and answers zero.
 */
int Ov002_CopyLinkItemPositions(VecFx32 *aOut)
{
    char *pCtx;
    s8 *pCount;
    char *pKind;
    int nKind;
    int i;
    char *pSlot;

    pCtx = data_ov002_0207fa10;
    if ((data_0204c240 & 4) != 0 && Ov002_IsSessionOpen() != 0) {
        nKind = Ov022_GetEntryField66(QueryActiveStateOrDelegate());
        if (nKind >= 0) {
            pCount = (s8 *)(pCtx + 0xfc + nKind);
            i = 0;
            if (*(s8 *)(pCtx + 0xfc + nKind) > 0) {
                pSlot = pCtx + nKind * 0x20;
                pKind = pCtx + nKind;
                do {
                    *aOut = *(VecFx32 *)(*(char **)(pSlot + 0x7c) + 8);
                    i++;
                    pSlot += 4;
                    aOut++;
                } while (i < *(s8 *)(pKind + 0xfc));
            }
            return *pCount;
        }
    }
    return 0;
}

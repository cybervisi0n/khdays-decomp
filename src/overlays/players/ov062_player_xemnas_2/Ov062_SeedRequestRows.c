/* Seeds the two request rows of the node (+0x12c, stride 0x240) from the local tables:
 * each row's +0x214 vector, +0x230 (zero), +0x234 speed -- scaled by 1.5 at 20 fps --
 * and +0x220 value come from data_ov062_020b7fa4 / 576c / 577c by row index, and the +0x23c
 * counter is cleared. */

#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { VecFx32 v[4]; } VecTable4;
typedef struct { int n[4]; } IntTable4;

extern const VecTable4 data_ov062_020b7fa4;
extern const IntTable4 data_ov062_020b7f6c;
extern const IntTable4 data_ov062_020b7f7c;

void Ov062_SeedRequestRows(char *node)
{
    VecTable4 tblVec;
    IntTable4 tblZero = {0};
    IntTable4 tblA;
    IntTable4 tblB;
    int i;
    char *pRow;

    tblVec = data_ov062_020b7fa4;
    tblA = data_ov062_020b7f6c;
    tblB = data_ov062_020b7f7c;
    pRow = node + 0x12c;
    for (i = 0; i < 2; i++) {
        *(VecFx32 *)(pRow + 0x214) = tblVec.v[i];
        *(int *)(pRow + 0x230) = tblZero.n[i];
        *(int *)(pRow + 0x234) = tblA.n[i];
        *(int *)(pRow + 0x220) = tblB.n[i];
        if (GetFrameRateMode() == 1) {
            *(int *)(pRow + 0x234) = (int)(((long long)*(int *)(pRow + 0x234) * 0x1800 + 0x800) >> 12);
        }
        *(int *)(pRow + 0x23c) = 0;
        pRow += 0x240;
    }
}

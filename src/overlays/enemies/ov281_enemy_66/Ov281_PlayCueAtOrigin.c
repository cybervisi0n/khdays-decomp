/* State step: posts a pose, plays an effect cue at the actor's origin and installs the next step.
 */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"

extern void func_ov107_020c0b90(int a, int b, VecFx32 v, int d);
extern void SetIndexedSlot(int self, int idx, void *cb);
extern void Ov281_PoseClearFields18ThenAdvance(void);
extern VecFx32 data_02041dc8;

void Ov281_PlayCueAtOrigin(int self) {
    int *s = *(int **)(self + 4);
    Ov107_PostTagUpdate((Actor *)s[0], 0xb, 0);
    func_ov107_020c0b90(s[0], 5, data_02041dc8, 0);
    SetIndexedSlot(self, *(signed char *)(self + 0x20), (void *)&Ov281_PoseClearFields18ThenAdvance);
}

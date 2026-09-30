/* Ov245_Stop -- stop: if bit 1 of +0x40 is set and a +0xc callback is installed it is
 * told 0, the +0x384 item's motion is halted (020c7ac), the +0x3a0 vector is reset to zero and,
 * in state 1, the +0x214 slot is closed by 020d4dec. */

#include "nitro/fx_types.h"

struct Flags40 { int bit0 : 1, bit1 : 1; };

extern void RefreshObjectCallbacks(int item, int a);
extern void Ov245_Mounted_Launch(int slot);
extern const VecFx32 data_02041dc8;

void Ov245_Stop(int self) {
    if (((struct Flags40 *)(self + 0x40))->bit1 && *(void (**)(int, int))(self + 0xc) != 0) {
        (*(void (**)(int, int))(self + 0xc))(self, 0);
    }
    RefreshObjectCallbacks(*(int *)(self + 0x384), 0);
    *(VecFx32 *)(self + 0x3a0) = data_02041dc8;
    if (*(int *)(self + 0x50) == 1) {
        Ov245_Mounted_Launch(*(int *)(self + 0x214));
    }
}

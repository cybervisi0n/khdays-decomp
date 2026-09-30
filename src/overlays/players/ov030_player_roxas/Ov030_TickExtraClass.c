/* Runs the scene's extra-class timer, then refreshes the state slots.
 *
 * The timer only runs while the actor is drivable and not linked, and only
 * while the extra class exists. It starts at 0x3000 when the class reports a
 * non-negative result for the zero vector -- and only if the actor carries flag
 * bit 22, which lives in the 64-bit word at +0x464. Each frame it counts down
 * by the shared tick; reaching zero raises the actor's panel bit and parks the
 * timer back at its idle value.
 */

#include "nitro/fx_types.h"

extern int func_ov022_0209fc78(int self, int a);
extern int Ov022_IsIndexedRecordBit0Set(int self, int a);
extern int Ov022_DispatchSpawnRecord(int object, const VecFx32 *at, int mode);
extern void func_ov022_0208954c(int object, int result, int source);
extern int Ov022_GetGlobal34(void);
extern void Ov030_UpdateMotionController(int slots, int id);
extern int data_ov030_020b5a00;
extern VecFx32 data_02041dc8;

void Ov030_TickExtraClass(int self) {
    int base = *(int *)&data_ov030_020b5a00;

    if (func_ov022_0209fc78(self, 1) != 0
        && Ov022_IsIndexedRecordBit0Set(self, 1) == 0
        && *(int *)(base + 0x2cac) != 0) {
        if (*(int *)(base + 0x2ca8) == (int)0x80000000
            && (*(unsigned long long *)((char *)self + 0x464) & 0x400000) != 0) {
            int r = Ov022_DispatchSpawnRecord(*(int *)(base + 0x2cac), &data_02041dc8, 0);

            if (r >= 0) {
                *(int *)(base + 0x2ca8) = 0x3000;
                func_ov022_0208954c(*(int *)(base + 0x2cac), r, self + 0x558);
            }
        }
        if (*(int *)(base + 0x2ca8) != (int)0x80000000) {
            *(int *)(base + 0x2ca8) -= Ov022_GetGlobal34();
            if (*(int *)(base + 0x2ca8) <= 0) {
                if (*(signed char *)(self + 0xf0d) != 0) {
                    *(unsigned char *)(self + 0xf0c) |= 1;
                }
                *(int *)(base + 0x2ca8) = (int)0x80000000;
            }
        }
    }

    Ov030_UpdateMotionController(base + 0x2cb0, *(short *)(self + 0x2aba));
}

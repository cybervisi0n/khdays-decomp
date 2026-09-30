/* Update of the ov146 actor: in phase 1, while the effect shows (+0x38c), every live, unshielded entity
 * of the owner's scene list (other than the owner and its +0x3b8 partner) within 48.0 of the owner is
 * marked (+0x1c5 bit 4); when the effect state changes to off the marks clear. The +0x38c state is
 * remembered in +0x390 and the base update runs. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { u16 lo : 8; u16 hi : 8; } flags16;

extern int *List_First(void *list);
extern int *List_Next(void *list);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void Ov107_ProcessObjectTick(void *obj, int arg2);

void Ov146_Update(char *self, int arg)
{
    if (*(int *)(self + 0x50) == 1) {
        int grid = *(int *)(*(int *)(self + 0x388) + 4);
        int *it;
        char *e;
        VecFx32 d;

        if (*(int *)(self + 0x38c) != 0) {
            it = List_First((void *)(grid + 0x80));
            e = it == 0 ? 0 : (char *)*it;
            while (e != 0) {
                char *owner = *(char **)(self + 0x388);

                if (e != owner && e != *(char **)(owner + 0x3b8) && (((flags16 *)(e + 0x60))->lo & 1) &&
                    !(*(u16 *)(e + 0x1ac) & 6)) {
                    VEC_Subtract((VecFx32 *)(owner + 0x74), (VecFx32 *)(e + 0x74), &d);
                    if (VEC_Normalize(&d, &d) <= 0x30000) {
                        *(u8 *)(e + 0x1c5) |= 0x10;
                    }
                }
                it = List_Next((void *)(grid + 0x80));
                e = it == 0 ? 0 : (char *)*it;
            }
        }
        if (*(int *)(self + 0x38c) != *(int *)(self + 0x390)) {
            if (*(int *)(self + 0x38c) == 0) {
                it = List_First((void *)(grid + 0x80));
                e = it == 0 ? 0 : (char *)*it;
                while (e != 0) {
                    char *owner = *(char **)(self + 0x388);

                    if (e != owner && e != *(char **)(owner + 0x3b8)) {
                        *(u8 *)(e + 0x1c5) &= ~0x10;
                    }
                    it = List_Next((void *)(grid + 0x80));
                    e = it == 0 ? 0 : (char *)*it;
                }
            }
            *(int *)(self + 0x390) = *(int *)(self + 0x38c);
        }
    }
    Ov107_ProcessObjectTick(self, arg);
}

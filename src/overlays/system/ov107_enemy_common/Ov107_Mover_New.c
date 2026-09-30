/* Allocates a 0x98-byte record, marks it live (mode nibble = 1, flag nibble bit0 set),
 * duplicates the caller's 32-byte position/orientation block into both the "current" (+0x58)
 * and "base" (+0x78) slots, runs the sub-object constructor at +0x10, then derives the
 * record's velocity (+4) from the caller's rate field (+0x18, halved) applied over the
 * caller's target delta (+0xc), and folds the caller's own position into it. Returns the
 * new record. */

#include "nitro/fx_types.h"

typedef struct { int w[8]; } Blk32;
typedef struct { unsigned char lo : 4; unsigned char hi : 4; } NibblePair;

extern void *CallocInstance(unsigned int size);
extern void SrtTransform_SetIdentity(void *o);
extern void ScaleVec3Fx12(int factor, int *src, int *dst);
extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *out);

void *Ov107_Mover_New(VecFx32 *pReq) {
    char *alloc = (char *)CallocInstance(0x98);
    NibblePair *nb = (NibblePair *)alloc;
    nb->lo = 1;
    nb->hi |= 1;
    *(Blk32 *)(alloc + 0x58) = *(Blk32 *)pReq;
    *(Blk32 *)(alloc + 0x78) = *(Blk32 *)pReq;
    SrtTransform_SetIdentity(alloc + 0x10);
    ScaleVec3Fx12(*(int *)((char *)pReq + 0x18) >> 1, (int *)((char *)pReq + 0xc), (int *)(alloc + 4));
    VEC_Add(pReq, (VecFx32 *)(alloc + 4), (VecFx32 *)(alloc + 4));
    return alloc;
}

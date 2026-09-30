/* cd104 */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Nibbles { u8 lo : 4; u8 hi : 4; };

extern int Ov107_CollectSphereOverlaps(int owner, void *sphere, int *hits);
extern int Ov107_CollectCapsuleOverlaps(int owner, void *box, int *hits);
extern int Ov107_CollectSegmentOverlaps(int owner, void *seg, int *hits);
extern int Ov107_CollectEntitiesTouchingDisc(int owner, void *cyl, int *hits);
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, int kind, VecFx32 *push, int z);
extern void Ov107_BuildAndSendUpdate(int owner, int id, int mode, void *at);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);

int Ov258_AttackHitTest(int *node, void *sphere, void *box, void *seg, void *cyl, VecFx32 *push, int bMask,
                        u16 effect, int kind)
{
    int *state = (int *)node[1];
    int hits[4];
    long n;
    long i;

    if (sphere != 0) {
        n = Ov107_CollectSphereOverlaps(*state, sphere, hits);
    } else if (box != 0) {
        n = Ov107_CollectCapsuleOverlaps(*state, box, hits);
    } else if (seg != 0) {
        n = Ov107_CollectSegmentOverlaps(*state, seg, hits);
    } else {
        n = Ov107_CollectEntitiesTouchingDisc(*state, cyl, hits);
    }
    if (n != 0) {
        for (i = 0; i < n; i++) {
            u8 bit = 1 << *(u16 *)(hits[i] + 2);

            if (bMask != 0 && (bit & ((struct Nibbles *)((u8 *)state + 0x52))->lo) != 0) {
                continue;
            }
            if (kind == 1) {
                push->y += 0x1000;
                push->z += 0x6000;
            }
            if (push->y > 0x3000) {
                push->y = 0x3000;
            }
            if (Ov107_InvokeHitCallback(hits[i], *state, *state, (u8)kind, push, 0) == 0) {
                continue;
            }
            if (bMask != 0) {
                ((struct Nibbles *)((u8 *)state + 0x52))->lo |= bit;
            }
            if (kind == 1 || kind == 4) {
                Ov107_BuildAndSendUpdate(*state, *(short *)((u8 *)state + 0x58), 0x10, (void *)(hits[i] + 0x190));
            }
            func_ov107_020c0b90(*state, effect + 6, *(VecFx32 *)(hits[i] + 0x190), 0);
            return 1;
        }
    }
    return 0;
}

/* Attack hit test of the ov237 actor: the targets touching `sphere` (else `box`, else `segment`) are
 * tried in turn; each first follows the +0x3d8-relative push (020cdb50) and, unless `once` already
 * marked it in the +0x57 mask, is pushed (020ca918, kind `kind`). The first target hit spawns effect
 * `effect` 1.25 above it, plays hit sound 0x12d (variant 5 for kind 0, 0xc for kinds 2 / 4) and
 * returns 1; 0 when nothing is hit. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

extern int Ov107_CollectSphereOverlaps(int owner, void *sphere, int *hits);
extern int Ov107_CollectCapsuleOverlaps(int owner, void *box, int *hits);
extern int Ov107_CollectSegmentOverlaps(int owner, void *segment, int *hits);
extern VecFx32 Ov237_RotateByActorHeading(int *node, VecFx32 *target);
/* Defined taking kind as int: declared narrower here, which is what makes mwcc truncate the
 * argument at the call as the ROM does (declared as defined, the code comes out different). */
extern int Ov107_InvokeHitCallback(int hit, int owner, int item, u8 kind, VecFx32 *push, int z);
extern void func_ov107_020c0b90(int owner, int mode, VecFx32 at, int flag);
extern void Ov107_BuildAndSendUpdate(int actor, int bank, int variant, int at);

int Ov237_AttackHitTest(int *node, void *sphere, void *box, void *segment, VecFx32 *push, int once, u16 effect, u16 kind)
{
    int *state = (int *)node[1];
    int hits[4];
    VecFx32 pos;
    long n;
    long i;
    u8 bit;

    if (sphere != 0) {
        n = Ov107_CollectSphereOverlaps(*state, sphere, hits);
    } else if (box != 0) {
        n = Ov107_CollectCapsuleOverlaps(*state, box, hits);
    } else {
        n = Ov107_CollectSegmentOverlaps(*state, segment, hits);
    }
    for (i = 0; i < n; i++) {
        bit = 1 << *(u16 *)(hits[i] + 2);
        *push = Ov237_RotateByActorHeading(node, push);
        if (once != 0 && (*((u8 *)state + 0x57) & bit)) {
            continue;
        }
        if (Ov107_InvokeHitCallback(hits[i], *state, *state, kind, push, 0) == 0) {
            continue;
        }
        pos = *(VecFx32 *)(hits[i] + 0x190);
        pos.y += 0x1400;
        if (once != 0) {
            *((u8 *)state + 0x57) |= bit;
        }
        func_ov107_020c0b90(*state, effect, pos, 0);
        switch (kind) {
        case 0:
            Ov107_BuildAndSendUpdate(*state, 0x12d, 5, state[0xe]);
            break;
        case 2:
        case 4:
            Ov107_BuildAndSendUpdate(*state, 0x12d, 0xc, state[0xe]);
            break;
        }
        return 1;
    }
    return 0;
}

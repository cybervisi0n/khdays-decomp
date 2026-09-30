/* Ov212_PickRetreatSpot -- pick a retreat spot: walk the candidate list and take the Nth one that is
 * far enough from the target, then queue move 2 toward it.
 *
 * One of a 3-member shape family; the twins live in ov266/ov267 and are byte-identical modulo
 * relocs (matched here, fanned out with dedupprop).
 *
 * Re-acquires the target into ctx[0]+0x5a8 and measures the gap from ctx[0]+0x508 to the target's
 * +0x190, minus both collision radii. The search only runs when the target is ALREADY close
 * (gap < 0x10000) and there is at least one candidate (ctx[0]+0x604 > 0) -- i.e. this is the
 * "back off" decision, not a general reposition.
 *
 * `best` starts at our own position (ctx[2]), so if nothing qualifies we simply stay put.
 *
 * The walk is deliberately not "nearest": it rolls n = rand(count) up front and then takes every
 * candidate whose gap from the target is >= 0x10000, stopping once it has seen n of them. So it
 * picks a RANDOM far-enough spot rather than the best one -- `best` keeps being overwritten and the
 * last one written wins. Whatever it lands on gets lifted by our radius and pushed through
 * Ov107_MoveNodeAndRelayout.
 *
 * The list at ctx[0]+0x5e4 is iterated with List_First / List_Next, which take ONLY the list
 * pointer: the cursor lives in the list itself at +0x24 (see src/engine/List_Next.c -- it reads
 * r0[9], advances to node->next, stores it back, and returns the payload at +0xc, or 0 once it
 * wraps to the sentinel). Do not "fix" these into two-arg iterator calls.
 *
 * Every exit converges on the same tail: ctx[0x1a] = 1, queue move 2, dispatch.
 *
 * `gap = f(...) - radii` is left as ONE statement here, unlike Ov212_IdleTick: the `call() -
 * expr` rule means mwcc evaluates the radii sum FIRST, which is exactly what the ROM does (it
 * computes r7 = target_r + own_r before the two calls and keeps it in a callee-saved register).
 * Same rule, opposite conclusion -- read the ROM, do not apply the crack by reflex. */

#include "nitro/fx_types.h"
#include "game/enemy_common.h"
#include "game/engine.h"

extern int Ov107_FindNearestObject(int obj, int kind);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *dst);
extern int VEC_Normalize(const VecFx32 *v, VecFx32 *unit);
extern VecFx32 *List_First(void *list);
extern void SetIndexedSlot(int self, int slot, void *cb);

void Ov212_PickRetreatSpot(int self) {
    int *ctx;
    VecFx32 v;
    VecFx32 best;
    int radii;
    int n;
    int i;
    VecFx32 *it;

    ctx = *(int **)(self + 4);
    *(int *)(ctx[0] + 0x5a8) = Ov107_FindNearestObject(ctx[0], 0);
    if (*(int *)(ctx[0] + 0x5a8) != 0) {
        radii = *(int *)(*(int *)(ctx[0] + 0x5a8) + 0x80) + *(int *)(ctx[0] + 0x80);
        VEC_Subtract((const VecFx32 *)(*(int *)(ctx[0] + 0x5a8) + 0x190),
                     (const VecFx32 *)(ctx[0] + 0x508), &v);
        if (VEC_Normalize(&v, &v) - radii < 0x10000 && *(int *)(ctx[0] + 0x604) > 0) {
            n = RandNextScaled(*(int *)(ctx[0] + 0x604));
            best = *(VecFx32 *)ctx[2];
            it = List_First((void *)(ctx[0] + 0x5e4));
            i = 0;
            while (it != 0) {
                VEC_Subtract((const VecFx32 *)(*(int *)(ctx[0] + 0x5a8) + 0x190), it, &v);
                if (VEC_Normalize(&v, &v) - radii >= 0x10000) {
                    best = *it;
                    if (i >= n) {
                        break;
                    }
                }
                it = (VecFx32 *)List_Next((void *)(ctx[0] + 0x5e4));
                i++;
            }
            best.y = best.y + *(int *)(ctx[0] + 0x80);
            Ov107_MoveNodeAndRelayout((Actor *)ctx[0], &best);
        }
    }

    ctx[0x1a] = 1;
    *(signed char *)(ctx[0] + 0x1c7) = 2;
    SetIndexedSlot(self, *(signed char *)(self + 0x20), 0);
}

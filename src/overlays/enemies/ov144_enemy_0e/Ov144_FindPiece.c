/* Piece search of the ov144 enemy (and its byte-identical twin): with a non-negative charge
 * range, walks the scene's +0xa8 piece list for the nearest active (+0x40 bit 1, +0x60 bit 0)
 * piece that is either within 0x2800 or, within the range, no more than 15 degrees off the
 * facing (sin, 0, cos of the +0x30 yaw) as seen from 0x2800 behind the actor, and that is in
 * line of sight past its +0x80 margin; a piece whose +0x1b4 entry has flag 0x10000 set is not
 * a candidate but is reported to the actor's +0x3a8 hook instead. Returns the piece or 0. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct hw60 { unsigned short lo : 8, hi : 8; };

extern int Ov144_ChargeRange(int *state, int flag);
extern void ScaleVec3Fx12(int scale, void *v, void *d);
extern void VEC_Add(void *a, void *b, void *d);
extern int *List_First(int list);
extern int *List_Next(int list);
extern void VEC_Subtract(void *a, void *b, void *d);
extern int VEC_Normalize(void *a, void *d);
extern int VEC_DotProduct(void *a, void *b);
extern int Ov144_HasLineOfSight(int *state, VecFx32 target, int margin);
extern u64 *GetEntryField20ByIndex(int index);
extern short data_0203d210[];
extern VecFx32 data_02042258;

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

int Ov144_FindPiece(int *node)
{
    int *state = (int *)node[1];
    int best = 0;
    int bestDist = 0x7fffffff;
    int scene = *(int *)(*state + 4);
    int dist;
    int range;
    int piece;
    int *entry;
    unsigned int idx;
    VecFx32 facing;
    VecFx32 dir;
    VecFx32 behind;
    VecFx32 at;
    VecFx32 fwd;

    range = Ov144_ChargeRange(state, 1);
    if (range < 0) {
        return 0;
    }
    idx = ANG2IDX(state[0xc]);
    facing.x = data_0203d210[idx * 2];
    facing.z = data_0203d210[idx * 2 + 1];
    facing.y = 0;
    ScaleVec3Fx12(-0x2800, &facing, &behind);
    VEC_Add(&behind, (void *)(*state + 0x74), &behind);
    entry = List_First(scene + 0xa8);
    piece = entry == 0 ? 0 : *entry;
    if (piece != 0) {
        fwd = data_02042258;
        do {
        if (((*(int *)(piece + 0x40) << 30) >> 31) != 0 && (((struct hw60 *)(piece + 0x60))->lo & 1) != 0) {
            at = *(VecFx32 *)(piece + 0x74);
            VEC_Subtract(&at, (void *)(*state + 0x74), &dir);
            dir.y = 0;
            dist = VEC_Normalize(&dir, &dir);
            VEC_Subtract(&at, &behind, &dir);
            dir.y = 0;
            if (VEC_Normalize(&dir, &dir) == 0) {
                dir = fwd;
            }
            if (dist <= 0x2800 || (range > 0 && dist <= range && VEC_DotProduct(&dir, &facing) >= 0xf74)) {
                if (Ov144_HasLineOfSight(state, at, *(int *)(piece + 0x80)) != 0) {
                    if ((*GetEntryField20ByIndex(*(u8 *)(piece + 0x1b4)) & 0x10000) != 0) {
                        if (*(void (**)(int, int))(*state + 0x3a8) != 0) {
                            (*(void (**)(int, int))(*state + 0x3a8))(*(u8 *)(piece + 0x1b4), *state);
                        }
                    } else if (dist < bestDist) {
                        bestDist = dist;
                        best = piece;
                    }
                }
            }
        }
        entry = List_Next(scene + 0xa8);
        piece = entry == 0 ? 0 : *entry;
        } while (piece != 0);
    }
    return best;
}

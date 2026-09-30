/* Target finder of the ov123 enemy (and its byte-identical twin). Walks the owner's +0xa8 actor
 * list for live (+0x40 bit 1), enabled (+0x60 bit 0) actors whose +0x1b4 kind record (01fffde0)
 * lacks flag 0x10000 while the actor's +0x3c8 is set, and returns one with, through `out`, the
 * surface distance (squared distance between the +0x74 positions minus both +0x80 radii
 * squared, floored at zero). Without bit 2 of the 0204c240 flags the first candidate of kind 0
 * wins; with it the nearest of all candidates does. */

#include "nitro/fx_types.h"

struct flags40 { int bit0 : 1, bit1 : 1; };
struct hw60 { unsigned short lo : 8, hi : 8; };

extern unsigned char data_0204c240;
extern int *List_First(void *list);
extern int *List_Next(void *list);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern long long *GetEntryField20ByIndex(int kind);

int Ov123_FindTarget(int self, int *out)
{
    int best = 0;
    int bestDist = 0x7fffffff;
    int owner = *(int *)(self + 4);
    int *pNode;
    int actor;
    VecFx32 dNear;
    VecFx32 dAll;
    long long distSq;
    long long radSq;
    int dist;

    if ((data_0204c240 & 4) == 0) {
        pNode = List_First((void *)(owner + 0xa8));
        actor = pNode == 0 ? 0 : *pNode;
        while (actor != 0) {
            if (((struct flags40 *)(actor + 0x40))->bit1 && (((struct hw60 *)(actor + 0x60))->lo & 1) != 0
                && (*(int *)(self + 0x3c8) == 0 || (*GetEntryField20ByIndex(*(unsigned char *)(actor + 0x1b4)) & 0x10000) == 0)
                && *(unsigned char *)(actor + 0x1b4) == 0) {
                VEC_Subtract((VecFx32 *)(actor + 0x74), (VecFx32 *)(self + 0x74), &dNear);
                distSq = (long long)dNear.x * dNear.x + (long long)dNear.y * dNear.y
                       + (long long)dNear.z * dNear.z;
                radSq = (long long)*(int *)(self + 0x80) * *(int *)(self + 0x80)
                      + (long long)*(int *)(actor + 0x80) * *(int *)(actor + 0x80);
                bestDist = (int)((distSq + 0x800) >> 12) - (int)((radSq + 0x800) >> 12);
                if (bestDist < 0) {
                    bestDist = 0;
                }
                best = actor;
                break;
            }
            pNode = List_Next((void *)(owner + 0xa8));
            actor = pNode == 0 ? 0 : *pNode;
        }
    } else {
        pNode = List_First((void *)(owner + 0xa8));
        actor = pNode == 0 ? 0 : *pNode;
        while (actor != 0) {
            if (((struct flags40 *)(actor + 0x40))->bit1 && (((struct hw60 *)(actor + 0x60))->lo & 1) != 0
                && (*(int *)(self + 0x3c8) == 0 || (*GetEntryField20ByIndex(*(unsigned char *)(actor + 0x1b4)) & 0x10000) == 0)) {
                VEC_Subtract((VecFx32 *)(actor + 0x74), (VecFx32 *)(self + 0x74), &dAll);
                distSq = (long long)dAll.x * dAll.x + (long long)dAll.y * dAll.y
                       + (long long)dAll.z * dAll.z;
                radSq = (long long)*(int *)(self + 0x80) * *(int *)(self + 0x80)
                      + (long long)*(int *)(actor + 0x80) * *(int *)(actor + 0x80);
                dist = (int)((distSq + 0x800) >> 12) - (int)((radSq + 0x800) >> 12);
                if (dist < 0) {
                    dist = 0;
                }
                if (dist < bestDist) {
                    bestDist = dist;
                    best = actor;
                }
            }
            pNode = List_Next((void *)(owner + 0xa8));
            actor = pNode == 0 ? 0 : *pNode;
        }
    }
    if (best != 0 && out != 0) {
        *out = bestDist;
    }
    return best;
}

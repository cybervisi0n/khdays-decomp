/* Spawns a projectile of the ov032 enemy from a descriptor (and its byte-identical twins),
 * only while the +0x2644 item has an owner. The placement starts at `pos`, or at the
 * descriptor's +0x1c offset rotated by the actor's heading and added to its +0x48c position;
 * its scale is the descriptor's id and its direction the heading's (sin, 0, cos), or the
 * descriptor's +0x28 vector rotated and normalised. Flag 1 marks a kind-1/2 shot, bit 1 of
 * the descriptor's +0x14 adds mode bit 0 and the +0x19 byte gives the last field. Always 0. */

#include "nitro/fx_types.h"

typedef struct { int m[9]; } MtxFx33;
typedef struct {
    VecFx32 pos;
    short sx, sy, sz, scale;
    int f14, f18, f1c, f20, f24, f28;
} Placement;

struct ShotDesc {
    int nId;                  /* 0x00 */
    char pad004[0x10];
    unsigned char nFlags14;   /* 0x14 */
    char pad015[4];
    signed char nKind19;      /* 0x19 */
    char pad01a[2];
    VecFx32 vOffset;             /* 0x1c */
    VecFx32 vDir;                /* 0x28 */
};

struct b2 { unsigned char b0 : 1, b1 : 1; };

extern void MTX_RotY33_(MtxFx33 *, int, int);
extern void MTX_MultVec33(const VecFx32 *, const MtxFx33 *, VecFx32 *);
extern void VEC_Add(const VecFx32 *, const VecFx32 *, VecFx32 *);
extern int VEC_Mag(const VecFx32 *);
extern int VEC_Normalize(const VecFx32 *, VecFx32 *);
extern void Ov022_SendPlacementMessage(char *, Placement *);
extern const short data_0203d210[];

int Ov032_SpawnShot(char *self, VecFx32 *pos, struct ShotDesc *desc, int flag)
{
    VecFx32 dir;
    MtxFx33 mtx;
    Placement req;
    unsigned short biased;
    int i;

    if (*(unsigned char *)(*(char **)(self + 0x2000 + 0x644) + 0x18) != 0) {
        biased = (unsigned short)(*(unsigned short *)(*(char **)(self + 0x20) + 0x80) - 0x8000);
        if (desc->vOffset.x != 0 || desc->vOffset.y != 0 || desc->vOffset.z != 0) {
            i = biased >> 4;
            MTX_RotY33_(&mtx, -data_0203d210[i * 2], -data_0203d210[i * 2 + 1]);
            MTX_MultVec33(&desc->vOffset, &mtx, &req.pos);
            VEC_Add(&req.pos, (VecFx32 *)(self + 0x8c + 0x400), &req.pos);
        } else {
            req.pos = *pos;
        }
        req.scale = desc->nId;
        if (!(desc->vDir.x == 0 && desc->vDir.y == 0 && desc->vDir.z == 0)) {
            i = biased >> 4;
            MTX_RotY33_(&mtx, -data_0203d210[i * 2], -data_0203d210[i * 2 + 1]);
            MTX_MultVec33(&desc->vDir, &mtx, &dir);
            if (VEC_Mag(&dir) != 0) {
                VEC_Normalize(&dir, &dir);
            }
        } else {
            i = (unsigned short)((unsigned short)(*(unsigned short *)(*(char **)(self + 0x20) + 0x80) - 0x8000) + 0x8000) >> 4;
            dir.x = data_0203d210[i * 2];
            dir.y = 0;
            dir.z = data_0203d210[i * 2 + 1];
        }
        req.sx = dir.x;
        req.sy = dir.y;
        req.sz = dir.z;
        req.f14 = 0;
        req.f18 = 0;
        req.f1c = 0;
        req.f20 = 0;
        req.f28 = desc->nKind19;
        req.f24 = 0;
        if (flag != 0) {
            req.f18 = 1;
            req.f20 |= 2;
            req.f24 = 1;
        }
        if (((struct b2 *)&desc->nFlags14)->b1 != 0) {
            req.f20 |= 1;
        }
        Ov022_SendPlacementMessage(self, &req);
    }
    return 0;
}

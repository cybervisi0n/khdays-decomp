/* Sends a placement for a shot from the muzzle with a random spread (wider when charged). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { VecFx32 p; short a,b,c; short scale; int f14,f18,f1c,f20,f24,f28; } Placement;
typedef struct { int m[9]; } MtxFx33;
extern void MTX_RotY33_(MtxFx33 *, int, int);
extern void MTX_MultVec33(VecFx32 *, MtxFx33 *, VecFx32 *);
extern void VEC_Add(VecFx32 *, VecFx32 *, VecFx32 *);
extern int VEC_Mag(VecFx32 *);
extern int VEC_Normalize(VecFx32 *, VecFx32 *);
extern int Ov022_ValidateTargetRef(char *);
extern void Ov022_SendPlacementMessage(char *, Placement *);
extern short data_0203d210[];
extern VecFx32 data_ov054_020b736c;
extern char *data_ov054_020b74a0;
void Ov054_Weapon_FireSpreadShot(char *self) {
    Placement req;
    VecFx32 v0;
    VecFx32 v1;
    MtxFx33 m;
    int idx, scale, n, t;
    char *base = data_ov054_020b74a0;
    char *offset = base + 0x1c8;
    v0 = data_ov054_020b736c;
    offset += 0x2c00;
    v1 = *(VecFx32 *)offset;
    idx = (u16)(*(u16 *)(*(char **)(self + 0x20) + 0x80) - 0x8000) >> 4;
    MTX_RotY33_(&m, -data_0203d210[idx * 2], -data_0203d210[idx * 2 + 1]);
    MTX_MultVec33(&v1, &m, &req.p);
    VEC_Add(&req.p, (VecFx32 *)(self + 0x8c + 0x400), &req.p);
    scale = 0x266;
    if (Ov022_ValidateTargetRef(self)) scale = 0x1800;
    n = Session_RandNext() - 0x800;
    t = (int)(((s64)scale * n + 0x800) >> 12);
    v0.x += t;
    n = Session_RandNext() - 0x800;
    t = (int)(((s64)scale * n + 0x800) >> 12);
    v0.y += t;
    if (VEC_Mag(&v0) != 0) VEC_Normalize(&v0, &v0);
    MTX_MultVec33(&v0, &m, &v0);
    req.a = (short)v0.x; req.b = (short)v0.y; req.c = (short)v0.z;
    req.f14 = 0; req.f1c = 0; req.f20 = 0;
    req.f18 = 7; req.f24 = 0; req.f28 = 0; req.scale = 0x1800;
    Ov022_SendPlacementMessage(self, &req);
    if (((int)(*(s64 *)self & 0x10000)) == 0) {
        *(u8 *)(self + 0x47a) = 3; *(u8 *)(self + 0x47b) = 0;
    }
}

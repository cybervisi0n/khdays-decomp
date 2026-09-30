/* Fires the ov046 enemy's ground shot (x4: ov046/065/084/101): aims at the target position when
 * one is within 0x9000, otherwise two units ahead along the heading; the point is dropped onto
 * the ground with a 0x7000 probe (resting 0x19a above a hit, or the full probe depth without one)
 * and a record request is built there: kind 7, the rig's pattern (+0x2d a4) as its tag, and a
 * speed of 0x1980 / 0x2200 / 0x2400 by pattern (pattern 2 also sets the homing flag). */

#include "nitro/types.h"
#include "nitro/fx_types.h"
#include "game/engine.h"

typedef struct { int m[9]; } Mtx33;
typedef struct {
    VecFx32 pos;
    short f0c, f0e, f10, f12;
    int f14, f18, f1c, f20, f24, f28;
} Params;

extern int Ov022_ValidateTargetRef(char *self);
extern VecFx32 *func_ov022_020ad0c0(char *self);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int VEC_Mag(const VecFx32 *v);
extern void MTX_RotY33_(Mtx33 *m, int s, int c);
extern void MTX_MultVec33(const VecFx32 *v, const Mtx33 *m, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern char *EntityMgr_RunRayCast(unsigned int mask, VecFx32 *from, VecFx32 *dir, void *node);
extern void Ov022_SendPlacementMessage(char *self, Params *p);
extern char *data_ov084_020b9a20;
extern short data_0203d210[];

void Ov084_FireGroundShot(char *self)
{
    Params p;
    VecFx32 pos;
    VecFx32 origin;
    VecFx32 down;
    Mtx33 m;
    VecFx32 d;
    char *rig = data_ov084_020b9a20 + 0x2c80;
    int bTarget = 0;
    u16 angle = *(u16 *)(*(char **)(self + 0x20) + 0x80) - 0x8000;
    char *hit;
    int idx;

    origin = *(VecFx32 *)(self + 0x8c + 0x400);
    if (Ov022_ValidateTargetRef(self) != 0) {
        VEC_Subtract(func_ov022_020ad0c0(self), &origin, &d);
        if (VEC_Mag(&d) <= 0x9000) {
            pos = *func_ov022_020ad0c0(self);
            bTarget = 1;
        }
    }
    if (bTarget == 0) {
        idx = angle >> 4;
        MTX_RotY33_(&m, -data_0203d210[idx * 2], -data_0203d210[idx * 2 + 1]);
        pos.x = 0;
        pos.y = 0;
        pos.z = 0x2000;
        MTX_MultVec33(&pos, &m, &pos);
        VEC_Add(&origin, &pos, &pos);
    }
    down.x = 0;
    down.z = 0;
    down.y = -0x7000;
    hit = EntityMgr_RunRayCast(*(u16 *)(self + 0x66), &pos, &down, *(void **)(self + 0x20));
    if (hit != 0) {
        Vec3ScaleAddQ27(*(int *)(hit + 0xc), &down, &pos, &pos);
        pos.y += 0x19a;
    } else {
        pos.y -= 0x7000;
    }
    p.f10 = 0;
    p.f0e = 0;
    p.f0c = 0;
    p.pos = pos;
    p.f14 = 0;
    p.f1c = 0;
    p.f20 = 0;
    p.f18 = 7;
    p.f24 = *(int *)(rig + 0x124);
    p.f28 = 0;
    switch (*(int *)(rig + 0x124)) {
    case 0:
        p.f12 = 0x1980;
        break;
    case 1:
        p.f12 = 0x2200;
        break;
    case 2:
        p.f12 = 0x2400;
        p.f28 = 1;
        break;
    }
    Ov022_SendPlacementMessage(self, &p);
}

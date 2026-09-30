/* Strike sweep of the ov221 enemy. The hit mask is the owner's +0x420 byte for mode 6 and the
 * state's +0x76 byte otherwise. With a query the entities come from ov107 c8fd0; without one a
 * sphere at the +0x3b0 body's +0x20 point is swept (ov107 c8eb8): radius 0x2400 for mode 7,
 * else 0x1380 pushed that far along the sine/cosine of the +0x50 heading. Every entity whose
 * kind bit is clear in the mask is pushed 1.0 along the flattened unit direction from the
 * owner's +0x74 (data_02042258 when degenerate) with the mode as the kind; on acceptance the
 * owner is sent mode 0 at the entity's +0x74 (with a query) or at the sphere's edge along the
 * unit direction (without), and the bit is set. Any acceptance fires reaction 0x4f (mode 6) or
 * 0x51 at the +8 point. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Sphere { VecFx32 pos; int nRadius; };

struct Ov221Hit {
    char pad000[2];
    u16 nKind;
    char pad004[0x70];
    VecFx32 vOrigin74;
};

struct Ov221Owner {
    char pad000[0x74];
    VecFx32 vOrigin74;
    char pad080[0x330];
    int *pBody3b0;
    char pad3b4[0x6c];
    u8 bMask420;
};

struct Ov221Body { char pad[0x20]; VecFx32 vPoint20; };

struct Ov221Node {
    struct Ov221Owner *pOwner;
    char pad004[4];
    int nEffect08;
    char pad00c[0x44];
    int nHeading50;
    char pad054[0x22];
    u8 bMask76;
};

extern int Ov107_CollectSphereOverlaps(struct Ov221Owner *owner, void *query, void *out);
extern int Ov107_CollectEntitiesTouchingDisc(struct Ov221Owner *owner, void *params, void *out);
extern void VEC_Subtract(void *a, void *b, void *d);
extern int VEC_Normalize(void *a, void *d);
extern void ScaleVec3Fx12(int scale, void *v, void *d);
extern void VEC_Add(void *a, void *b, void *d);
/* Defined taking kind as int: declared narrower here, which is what makes mwcc truncate the
 * argument at the call as the ROM does (declared as defined, the code comes out different). */
extern int Ov107_InvokeHitCallback(struct Ov221Hit *hit, struct Ov221Owner *a, struct Ov221Owner *b,
                               u8 kind, void *push, int z);
extern void func_ov107_020c0b90(struct Ov221Owner *owner, int a, VecFx32 v, int b);
extern void Ov107_BuildAndSendUpdate(struct Ov221Owner *owner, int a, int id, int p);
extern const VecFx32 data_02042258;
extern short data_0203d210[];

#define ANG2IDX(a) ((unsigned short)(((long long)(a) * 0x28be60db9391LL + 0x80000000000LL) >> 44) >> 4)

void Ov226_StrikeSweepEntities(struct Ov221Node *node, unsigned int mode, void *params)
{
    struct Sphere sphere;
    struct Ov221Hit *hits[4];
    VecFx32 dir;
    VecFx32 push;
    VecFx32 step;
    int n;
    int pushed;
    int i;
    u8 bit;
    u8 *pMask;
    unsigned int idx;

    pushed = 0;
    if (mode == 6) {
        pMask = &node->pOwner->bMask420;
    } else {
        pMask = &node->bMask76;
    }
    if (params != 0) {
        n = Ov107_CollectEntitiesTouchingDisc(node->pOwner, params, hits);
    } else {
        sphere.pos = ((struct Ov221Body *)*node->pOwner->pBody3b0)->vPoint20;
        sphere.nRadius = 0x1cbb;
        if (mode == 7) {
            sphere.nRadius = 0x350a;
        } else {
            idx = ANG2IDX(node->nHeading50);
            dir.x = data_0203d210[idx * 2];
            dir.y = 0;
            dir.z = data_0203d210[idx * 2 + 1];
            ScaleVec3Fx12(sphere.nRadius, &dir, &dir);
            VEC_Add(&dir, &sphere.pos, &sphere.pos);
        }
        n = Ov107_CollectSphereOverlaps(node->pOwner, &sphere, hits);
    }
    i = 0;
    if (n > 0) {
        do {
            bit = (u8)(1 << hits[i]->nKind);
            if ((*pMask & bit) == 0) {
                VEC_Subtract(&hits[i]->vOrigin74, &node->pOwner->vOrigin74, &push);
                VEC_Normalize(&push, &step);
                push.y = 0;
                if (VEC_Normalize(&push, &push) == 0) {
                    push = data_02042258;
                }
                ScaleVec3Fx12(0x1000, &push, &push);
                if (Ov107_InvokeHitCallback(hits[i], node->pOwner, node->pOwner, mode, &push, 0) != 0) {
                    if (params != 0) {
                        step = hits[i]->vOrigin74;
                    } else {
                        ScaleVec3Fx12(sphere.nRadius, &step, &step);
                        VEC_Add(&step, &sphere.pos, &step);
                    }
                    func_ov107_020c0b90(node->pOwner, 0, step, 0);
                    *pMask |= bit;
                    pushed = 1;
                }
            }
            i++;
        } while (i < n);
    }
    if (pushed == 0) {
        return;
    }
    switch (mode) {
    case 6:
        Ov107_BuildAndSendUpdate(node->pOwner, 0, 0x4f, node->nEffect08);
        break;
    case 0:
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    default:
        Ov107_BuildAndSendUpdate(node->pOwner, 0, 0x51, node->nEffect08);
        break;
    }
}

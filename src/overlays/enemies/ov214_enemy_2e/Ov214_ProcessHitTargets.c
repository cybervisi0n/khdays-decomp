/* Attack hit step: unless the actor is paused, collects the objects in its sphere (or touching its
 * disc) and hits each one once, pushing it away and playing the hit effect. */

#pragma opt_dead_assignments off

#include "nitro/types.h"
#include "nitro/fx_types.h"

struct Vec4 { int x, y, z, w; };

struct Ov264Hit {
    char pad000[2];
    u16 nKind;
    char pad004[0x70];
    struct Vec4 vOrigin74;
};

struct Ov264Owner {
    char pad000[0x74];
    struct Vec4 vOrigin74;
    char pad084[0x140];
    u8 bFlags1c4;
};

struct Ov264Node {
    struct Ov264Owner *pOwner;
    char pad004[0x0c];
    int nEffect10;
    VecFx32 vStep14;
    char pad020[0x51];
    u8 bHandled71;
};

struct Ov264Params {
    VecFx32 aim;
    VecFx32 v0c;
    VecFx32 v18;
    VecFx32 v24;
    int nSteps;
    int bFlag;
};

extern int Ov107_CollectSphereOverlaps(struct Ov264Owner *owner, void *query, void *out);
extern int Ov107_CollectEntitiesTouchingDisc(struct Ov264Owner *owner, struct Ov264Params *params, void *out);
extern void VEC_Subtract(void *a, void *b, void *d);
extern int VEC_Normalize(void *a, void *d);
extern void ScaleVec3Fx12(int scale, void *v, void *d);
extern void VEC_Add(void *a, void *b, void *d);
/* Defined taking kind as int: declared narrower here, which is what makes mwcc truncate the
 * argument at the call as the ROM does (declared as defined, the code comes out different). */
extern int Ov107_InvokeHitCallback(struct Ov264Hit *hit, struct Ov264Owner *a, struct Ov264Owner *b,
                               u8 kind, void *push, int z);
extern void func_ov107_020c0b90(struct Ov264Owner *owner, int a, VecFx32 v, int b);
extern void Ov107_BuildAndSendUpdate(struct Ov264Owner *owner, int a, int id, int p);
extern const VecFx32 data_02042258;

void Ov214_ProcessHitTargets(struct Ov264Node *node, unsigned int kind, struct Ov264Params *params) {
    struct Vec4 origin;
    struct Ov264Hit *hits[4];
    VecFx32 push;
    VecFx32 step;
    int i;
    u8 bit;
    u8 seen;
    int n;
    int pushed;

    i = 1;
    seen = 1;
    bit = 0;
    seen = 0;
    origin = node->pOwner->vOrigin74;
    pushed = 0;
    if ((node->pOwner->bFlags1c4 & 2) != 0) {
        return;
    }
    if (params != 0) {
        n = Ov107_CollectEntitiesTouchingDisc(node->pOwner, params, hits);
    } else {
        n = Ov107_CollectSphereOverlaps(node->pOwner, &origin, hits);
    }
    i = 0;
    if (n > 0) {
        do {
            bit = (u8)(1 << hits[i]->nKind);
            seen |= bit;
            if ((node->bHandled71 & bit) == 0) {
                VEC_Subtract(&hits[i]->vOrigin74, &node->pOwner->vOrigin74, &push);
                VEC_Normalize(&push, &step);
                push.y = 0;
                if (VEC_Normalize(&push, &push) == 0) {
                    push = data_02042258;
                }
                ScaleVec3Fx12(0x800, &push, &push);
                if (Ov107_InvokeHitCallback(hits[i], node->pOwner, node->pOwner, kind, &push, 0) != 0) {
                    if (params != 0) {
                        VEC_Add(&hits[i]->vOrigin74, &push, &step);
                        func_ov107_020c0b90(node->pOwner, 1, step, 0);
                    } else {
                        ScaleVec3Fx12(origin.w, &step, &step);
                        VEC_Add(&step, &push, &step);
                        VEC_Add(&step, &origin, &step);
                        func_ov107_020c0b90(node->pOwner, 1, step, 0);
                    }
                    node->bHandled71 |= bit;
                    ScaleVec3Fx12(-0x1000, &push, &node->vStep14);
                    VEC_Normalize(&node->vStep14, &node->vStep14);
                    pushed = 1;
                }
            }
            i++;
        } while (i < n);
    }
    node->bHandled71 &= seen;
    if (pushed == 0) {
        return;
    }
    switch (kind) {
    case 1:
        Ov107_BuildAndSendUpdate(node->pOwner, 0, 0x50, node->nEffect10);
        break;
    case 0:
    case 2:
        Ov107_BuildAndSendUpdate(node->pOwner, 0, 0x52, node->nEffect10);
        break;
    }
}

/* ov070 actor: fire the attack. Marks the two effect slots busy, asks ov022_0209fe20
 * for a spawn point, offsets it by the actor's facing, builds the projectile parameter
 * block and hands it to ov022_02091324, then latches the fired bit and reports the
 * resulting state through ov022_020a35f4.
 *
 * The facing is turned into a rotation the usual way for this codebase: the raw
 * angle at +0x80 of the object at +0x20 is biased by 0x8000, truncated to 16 bits and
 * shifted down by four to index a table of sin/cos SHORT pairs, and BOTH components
 * are negated before MTX_RotY33_.
 *
 * The +0x464 and +0x46c flag words are 64-bit: each OR shows up as a pair of stores
 * with `orr rN, rN, #0` for the high half, and a constant that appears as 0x4000 in the
 * high word means 0x400000000000ULL, not 0x400000000ULL.
 *
 * The +0x664 handler takes TWO arguments and the second is zero. The ROM sets up only
 * r0 before that indirect call, because r1 already holds the zero the six clear stores
 * just used -- mwcc coalesces the two. Written as a one-argument call the whole tail
 * comes out one register lower and thirteen instructions differ; that single missing
 * argument was the last residue in this function. */

#include "nitro/fx_types.h"
#include "game/engine.h"

struct ActorBits {
    unsigned char bUnk0 : 1;
    unsigned char bFired : 1;
};

typedef struct { int m[9]; } Mtx33;

/* One object, not two: the position vector is its head and the projectile fields
 * are its tail, which is why the ROM passes a single pointer. */
struct FireParams {
    VecFx32 vPos;
    short vx;
    short vy;
    short vz;
    short nSpeed;
    int bFromSlot;
    int nKind;
    int pad1c[4];
};

extern void Ov022_StepAnchorDelta(char *self, VecFx32 *pOut);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void MTX_RotY33_(Mtx33 *m, int nSin, int nCos);
extern void MTX_MultVec33(const VecFx32 *v, const Mtx33 *m, VecFx32 *out);
extern int VEC_Mag(const VecFx32 *v);
extern void VEC_Normalize(const VecFx32 *v, VecFx32 *out);
extern void Ov022_SendPlacementMessage(char *self, struct FireParams *p);
extern int Ov022_ActorSetState(char *self, int nMode);

extern char *data_ov070_020b9ca0;
extern VecFx32 data_02041dc8;
extern VecFx32 data_ov070_020b9b28;
extern VecFx32 data_ov070_020b9b34;
extern short data_0203d210[];

int Ov070_ActorFireAttack(char *self)
{
    VecFx32 vSpawn;
    VecFx32 vAim;
    VecFx32 vDir;
    Mtx33 mFacing;
    struct FireParams p;
    VecFx32 vOffset;
    int nRet = 0;
    char *pBlock = data_ov070_020b9ca0 + 0x2c2c;

    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x464) |= 0x10000ULL;
    }
    if (Session_GetLocalPlayerIndex() == 0) {
        *(unsigned long long *)(self + 0x46c) |= 0x10000ULL;
    }

    vSpawn = data_02041dc8;
    Ov022_StepAnchorDelta(self, &vSpawn);
    if (vSpawn.y != 0) {
        *(int *)(self + 0x58) = vSpawn.y;
    } else if ((*(int *)(self + 0x24) & 4) == 0) {
        *(unsigned long long *)self |= 0x400000000000ULL;
        *(int *)(self + 0x58) = 0;
    }

    vOffset = vSpawn;
    vOffset.y = 0;
    VEC_Add((VecFx32 *)(self + 0x98 + 0x400), &vOffset, (VecFx32 *)(self + 0x98 + 0x400));

    if (*(int *)(self + 0x4cc) >= *(int *)(pBlock + 8) && *(int *)(pBlock + 4) == 0) {
        int nIndex;

        vAim = data_ov070_020b9b28;
        vDir = data_ov070_020b9b34;
        nIndex = (unsigned short)(*(unsigned short *)(*(char **)(self + 0x20) + 0x80)
                                  - 0x8000) >> 4;
        MTX_RotY33_(&mFacing, -data_0203d210[nIndex * 2], -data_0203d210[nIndex * 2 + 1]);
        MTX_MultVec33(&vDir, &mFacing, &p.vPos);
        VEC_Add(&p.vPos, (VecFx32 *)(self + 0x8c + 0x400), &p.vPos);
        MTX_MultVec33(&vAim, &mFacing, &vAim);
        if (VEC_Mag(&vAim) != 0) {
            VEC_Normalize(&vAim, &vAim);
        }
        p.vx = (short)vAim.x;
        p.vy = (short)vAim.y;
        p.vz = (short)vAim.z;
        p.bFromSlot = 0;
        p.nKind = 7;
        p.pad1c[0] = 0;
        p.pad1c[1] = 0;
        p.pad1c[2] = 0;
        p.pad1c[3] = 0;
        p.nSpeed = 0x900;
        if (*(int *)pBlock != 0) {
            p.bFromSlot = 1;
        }
        Ov022_SendPlacementMessage(self, &p);
        *(int *)(pBlock + 4) = 1;

    }

    /* Reached whether or not the shot was fired: the ROM's guard
     * branches HERE, not past this. */
    ((struct ActorBits *)(self + 0x694))->bFired = (*(int (**)(char *))(self + 0x668))(self);
    if (((struct ActorBits *)(self + 0x694))->bFired) {
        *(unsigned long long *)self |= 0x2000000000000ULL;
        if ((**(int **)(self + 0x20) & 0x20) == 0) {
            SceneNode_Enable(*(int **)(self + 0x20) + 1);
        }
        if (Session_GetLocalPlayerIndex() == 0) {
            *(unsigned long long *)(self + 0x464) |= 2ULL;
        }
    }

    if (((struct ActorBits *)(self + 0x694))->bFired) {
        *(int *)(self + 0x4a0) = 0;
        *(int *)(self + 0x49c) = 0;
        *(int *)(self + 0x498) = 0;
        *(int *)(self + 0x6a0) = 0;
        *(int *)(self + 0x69c) = 0;
        *(int *)(self + 0x698) = 0;
        if ((*(int *)(self + 0x24) & 4) != 0) {
            (*(void (**)(char *, int))(self + 0x664))(self, 0);
            nRet = Ov022_ActorSetState(self, 0);
        } else {
            nRet = Ov022_ActorSetState(self, 2);
        }
    }
    return nRet;
}

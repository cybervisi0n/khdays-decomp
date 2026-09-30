/* Ov252_PlaceBodyParts -- per-frame placement of the ov252 boss's nine body-part collision volumes.
 * For each part i the rig node aNode[i] gives the pose: part 0 is a capsule along the node's
 * down axis (from +2.5 up, direction -7.5); parts 1 and 2 are capsules from 2.0 in front of the
 * node towards the head (+3.0) / the other node, whose second hit volume copies the +0x554 pose;
 * parts 3-6 are points 0.5 along the node's X that also refresh the aHand[i - 3] pose moved 3.0
 * along X; parts 7 and 8 place both volumes at mirrored offsets. Afterwards it publishes the
 * +0xa0 state, drops the +0x580 effect when out of phase 5, finishes the +0x6a8 task once the
 * part is gone and runs the common actor update. */

#include "nitro/fx_types.h"

typedef struct Srt {
    int     aRot[4];
    VecFx32 trans;
    int     aScale[3];
    int     nFlags;
} Srt;

typedef struct Ov252Node {
    int pad_00;
    Srt srt;
} Ov252Node;

typedef struct Ov252Collider {
    unsigned char pad_00[0x10];
    Srt           srt;
    unsigned char pad_3c[0x58 - 0x3c];
    VecFx32       vPos;
    VecFx32       vDir;
    int           nLength;
} Ov252Collider;

typedef struct Ov252PartRef {
    Ov252Collider *pObj;
    int          pad_04;
    unsigned int nFlags : 8;
} Ov252PartRef;

typedef struct Ov252Boss {
    unsigned char pad_000[0x3c];
    int           nTaskList;
    unsigned char pad_040[0xa0 - 0x40];
    int           aState[4];
    unsigned char pad_0b0[0x1c6 - 0xb0];
    signed char   nPhase;
    unsigned char pad_1c7[0x4e8 - 0x1c7];
    Ov252PartRef *aPartB[9];
    Ov252Collider *aPartA[9];
    Ov252Node    *aNode[9];
    unsigned char pad_554[0x554 - 0x554];
    Ov252Node    *pPoseSrc;
    unsigned char pad_558[0x560 - 0x558];
    char         *pStateDst;
    unsigned char pad_564[0x580 - 0x564];
    int           nEffect;
    int           pad_584;
    int           nPhaseTimer;
    Srt           aHand[4];
    unsigned char pad_63c[0x6a8 - 0x63c];
    int           nTask;
} Ov252Boss;

extern void Vec3TransformViaTempMtx(VecFx32 *out, void *rot, VecFx32 *in);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern void VEC_Subtract(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int  VEC_Normalize(const VecFx32 *in, VecFx32 *out);
extern void ScaleVec3Fx12(int scale, const VecFx32 *in, VecFx32 *out);
extern void Srt_SetTranslation(Srt *srt, const VecFx32 *pos);
extern void Ov107_UnlinkNodeFromOwner(int effect);
extern void TaskList_FinishByTag(int list, int task);
extern void Ov107_AiState_PostTickBase(Ov252Boss *self);

static inline void VEC_Set(VecFx32 *a, int x, int y, int z)
{
    a->x = x;
    a->y = y;
    a->z = z;
}

void Ov252_PlaceBodyParts(Ov252Boss *self)
{
    int i;
    int j;
    int len;
    Ov252Collider *obj;
    Srt srt;
    VecFx32 pos;
    VecFx32 dir;

    for (i = 0; i < 9; i++) {
        switch (i) {
        case 0:
            VEC_Set(&pos, 0, 0x2800, 0);
            VEC_Set(&dir, 0, -0x7800, 0);
            Vec3TransformViaTempMtx(&pos, &self->aNode[i]->srt, &pos);
            VEC_Add(&self->aNode[i]->srt.trans, &pos, &pos);
            Vec3TransformViaTempMtx(&dir, &self->aNode[i]->srt, &dir);
            for (j = 0; j < 2; j++) {
                if (j == 0) {
                    obj = self->aPartA[i];
                } else {
                    obj = self->aPartB[i]->pObj;
                }
                obj->vPos = pos;
                obj->nLength = VEC_Normalize(&dir, &obj->vDir);
            }
            break;
        case 1:
            dir = self->aNode[0]->srt.trans;
            dir.y += 0x3000;
            VEC_Subtract(&dir, &self->aNode[i]->srt.trans, &dir);
            len = VEC_Normalize(&dir, &dir);
            ScaleVec3Fx12(-0x2000, &dir, &pos);
            VEC_Add(&self->aNode[i]->srt.trans, &pos, &pos);
            self->aPartA[i]->vPos = pos;
            self->aPartA[i]->vDir = dir;
            self->aPartA[i]->nLength = len;
            self->aPartB[i]->pObj->srt = self->pPoseSrc->srt;
            break;
        case 2:
            dir = self->aNode[1]->srt.trans;
            VEC_Subtract(&dir, &self->aNode[i]->srt.trans, &dir);
            len = VEC_Normalize(&dir, &dir);
            ScaleVec3Fx12(-0x2000, &dir, &pos);
            VEC_Add(&self->aNode[i]->srt.trans, &pos, &pos);
            self->aPartA[i]->vPos = pos;
            self->aPartA[i]->vDir = dir;
            self->aPartA[i]->nLength = len;
            self->aPartB[i]->pObj->srt = self->pPoseSrc->srt;
            break;
        case 7:
        case 8:
            for (j = 0; j < 2; j++) {
                if (j == 0) {
                    if (i == 7) {
                        VEC_Set(&pos, 0x3c00, 0x1400, 0);
                    } else {
                        VEC_Set(&pos, -0x3c00, -0x1400, 0);
                    }
                } else if (i == 7) {
                    VEC_Set(&pos, 0x5000, 0x1400, 0);
                } else {
                    VEC_Set(&pos, -0x5000, -0x1400, 0);
                }
                srt = self->aNode[i]->srt;
                Vec3TransformViaTempMtx(&pos, &self->aNode[i]->srt, &pos);
                VEC_Add(&srt.trans, &pos, &pos);
                Srt_SetTranslation(&srt, &pos);
                if (j == 0) {
                    self->aPartA[i]->srt = srt;
                } else {
                    self->aPartB[i]->pObj->srt = srt;
                }
            }
            break;
        case 3:
        case 4:
        case 5:
        case 6:
            VEC_Set(&pos, 0x800, 0, 0);
            Vec3TransformViaTempMtx(&pos, &self->aNode[i]->srt, &pos);
            VEC_Add(&self->aNode[i]->srt.trans, &pos, &pos);
            {
                Ov252Collider *pA = self->aPartA[i];
                Ov252Collider *pB = self->aPartB[i]->pObj;
                Srt *pHand;
                pA->vPos = pos;
                pHand = (Srt *)((char *)self->aHand + (i - 3) * (int)sizeof(Srt));
                pB->vPos = pA->vPos;
                VEC_Set(&pos, 0x3000, 0, 0);
                *pHand = self->aNode[i]->srt;
                Vec3TransformViaTempMtx(&pos, pHand, &pos);
                VEC_Add(&pos, &pHand->trans, &pos);
                Srt_SetTranslation(pHand, &pos);
            }
            break;
        }
    }
    *(int (*)[4])(self->pStateDst + 4) = *(int (*)[4])self->aState;
    if (self->nPhase != 5) {
        self->nPhaseTimer = 0;
    }
    if (self->nPhaseTimer == 0 && self->nEffect != 0) {
        Ov107_UnlinkNodeFromOwner(self->nEffect);
        self->nEffect = 0;
    }
    if (!(self->aPartB[1]->nFlags & 1) && self->nTask != 0) {
        TaskList_FinishByTag(self->nTaskList, self->nTask);
        self->nTask = 0;
    }
    Ov107_AiState_PostTickBase(self);
}

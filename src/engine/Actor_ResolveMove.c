
#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct CollCastParams {
    VecFx32 *origin;
    VecFx32 *direction;
    int radius;
    u16 directionIsUnit;
    u16 flags;
    void *exclude;
} CollCastParams;

typedef struct HitFull {
    void *model;
    void *face;
    int unknown08;
    int distance;
    u8 rest[0x98 - 0x10];
} HitFull;

typedef struct Body {
    u8 field_00[0x80];
    u16 heading;
    u8 field_82[0x148 - 0x82];
    int radius;
} Body;

/* Every recorded cast is one HitFull, the world-space contact point, and a
   kind code (1 = movable hit, 2 = static hit, 0 = none).  They sit back to
   back from +0xf0, so the actor holds them as one array. */
typedef struct CastRecord {
    HitFull hit;
    VecFx32 pos;
    int kind;
} CastRecord;

#define CAST_FALLBACK 0
#define CAST_GROUND   1
#define CAST_RESOLVED 2
#define CAST_STEP     4

typedef struct Actor {
    const Body *body;
    int flags;
    u8 field_08[0x2c - 0x08];
    VecFx32 anchor;
    u8 field_38[0x46 - 0x38];
    s16 worldId;
    u8 field_48[0xf0 - 0x48];
    CastRecord casts[5];
} Actor;

typedef struct Obj {
    VecFx32 pos;
} Obj;

extern void *GetTrackEntryBase(int id);
extern void ScaleVec3Fx12(int factor, int *src, int *dst);
extern int VEC_Normalize(const VecFx32 *source, VecFx32 *destination);
extern HitFull *Collision_RunSphereCast(void *world, CollCastParams *params);
extern void Vec3ScaleAddQ27(int s, const VecFx32 *a, const VecFx32 *b, VecFx32 *c);
extern void VecFx32FromVecS16(int r0, short *r1, int *r2);
extern void VEC_MultAdd(int scale, const VecFx32 *v, const VecFx32 *add, VecFx32 *dst);
extern HitFull *Collision_RunRayCast(void *world, CollCastParams *params);
extern int VEC_Distance(const VecFx32 *a, const VecFx32 *b);
extern int GetFrameRateMode(void);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern HitFull *Collision_CastNearest(void *world, CollCastParams *params);

extern short data_0203d210[];

int Actor_ResolveMove(const Obj *obj, VecFx32 *pOut, Actor *self)
{
    VecFx32 faceNormal;
    CollCastParams params;
    VecFx32 castDir;
    VecFx32 heightVec;
    VecFx32 normDir;
    VecFx32 originVec;
    VecFx32 castOrigin;
    VecFx32 anchorBase;
    VecFx32 slideVec;
    VecFx32 slideDir;
    int stepFound;
    int fallbackHit;
    int hitRecorded;
    void *world;
    int slideScale;
    int flags;
    s16 worldId;
    const Body *body;
    int heightScale;
    int dx, dz;
    int closeRange;
    HitFull *hit;
    int moveLen;
    int mag;
    int baseY, baseZ;
    int groundY;
    int resolved;
    int pushScale;

    self->flags &= ~2;
    stepFound = 0;
    self->flags &= ~0x400;
    self->flags &= ~0x800;

    self->casts[CAST_GROUND].kind = 0;
    self->casts[CAST_RESOLVED].kind = 0;
    self->casts[CAST_FALLBACK].kind = 0;

    worldId = self->worldId;
    fallbackHit = 0;
    closeRange = 0;
    hitRecorded = 0;
    if (worldId < 0) {
        return hitRecorded;
    }

    world = GetTrackEntryBase((unsigned short)worldId);

    body = self->body;
    heightScale = body->radius;
    dx = pOut->x - *(int *)&self->anchor;
    if (dx < 0) dx = -dx;
    if (dx < 0x10) {
        dz = pOut->z - self->anchor.z;
        if (dz < 0) dz = -dz;
        if (dz < 0x10) {
            int idx = (int)body->heading >> 4;
            heightVec.x = data_0203d210[idx * 2];
            heightVec.y = 0;
            heightVec.z = data_0203d210[idx * 2 + 1];
            ScaleVec3Fx12(heightScale, (int *)&heightVec, (int *)&heightVec);
            closeRange = 1;
            goto haveMoveVec;
        }
    }
    heightVec = *pOut;

haveMoveVec:
    baseY = obj->pos.y;
    baseZ = obj->pos.z;
    groundY = baseY + (heightScale + 0x333);
    originVec.x = obj->pos.x;
    originVec.y = groundY;
    originVec.z = baseZ;
    moveLen = VEC_Normalize(&heightVec, &normDir);
    castOrigin = originVec;
    castOrigin.y = obj->pos.y + 0x66 + 0xa00;
    ScaleVec3Fx12(moveLen + 0x410, (int *)&normDir, (int *)&castDir);

    if (closeRange) {
        moveLen = 0;
    }
    params.origin = &castOrigin;
    params.direction = &castDir;
    params.radius = 0x900;
    params.directionIsUnit = 0;
    params.flags = 0;
    params.exclude = (void *)self->body;

    hit = Collision_RunSphereCast(world, &params);
    if (hit == 0) {
        castOrigin.y = obj->pos.y + 0x66 + 0x600;
        hit = Collision_RunSphereCast(world, &params);
        if (hit != 0 && hit->unknown08 == 0) {
            hit = 0;
        }
    }

    if (hit != 0) {
        Vec3ScaleAddQ27(hit->distance, &castDir, &castOrigin, &self->casts[CAST_GROUND].pos);
        self->casts[CAST_GROUND].pos.y = obj->pos.y;
        VecFx32FromVecS16((int)hit->model, (short *)((u8 *)hit->face + 0x14), (int *)&faceNormal);
    }

    if (hit == 0) {
        castDir.x = 0;
        castDir.y = -0x333;
        castDir.z = 0;
        VEC_MultAdd(0x5000, &normDir, &castDir, &castDir);
        castOrigin = obj->pos;
        castOrigin.y = castOrigin.y + 0x333;
        hit = Collision_RunRayCast(world, &params);
        if (hit != 0) {
            int slope;
            VecFx32FromVecS16((int)hit->model, (short *)((u8 *)hit->face + 0x14), (int *)&faceNormal);
            VEC_Normalize(&faceNormal, &faceNormal);
            slope = faceNormal.y;
            if ((slope < 0x100 && slope > -0x100) || slope > 0xfae) {
                hit = 0;
            } else {
                int reach;
                self->casts[CAST_FALLBACK].kind = (hit->unknown08 != 0) ? 1 : 2;
                self->casts[CAST_FALLBACK].hit = *hit;
                Vec3ScaleAddQ27(hit->distance, &castDir, &castOrigin, &self->casts[CAST_FALLBACK].pos);
                reach = VEC_Distance(&self->casts[CAST_FALLBACK].pos, &originVec);
                if (heightScale + moveLen <= reach) {
                    hit = 0;
                } else {
                    fallbackHit = 1;
                    flags = self->flags;
                    flags |= 0x400;
                    self->flags = flags;
                }
            }
        }
    }

    if (hit == 0) {
        castDir.x = 0;
        castDir.y = 0x19a;
        castDir.z = 0;
        VEC_MultAdd(0x5000, &normDir, &castDir, &castDir);
        castOrigin = obj->pos;
        castOrigin.y = castOrigin.y + 0x66 + 0xd00;
        hit = Collision_CastNearest(world, &params);
        if (hit != 0) {
            int slope;
            VecFx32FromVecS16((int)hit->model, (short *)((u8 *)hit->face + 0x14), (int *)&faceNormal);
            VEC_Normalize(&faceNormal, &faceNormal);
            slope = faceNormal.y;
            if ((slope < 0x100 && slope > -0x100) || slope < -0xfae) {
                hit = 0;
            } else {
                int reach;
                self->casts[CAST_STEP].kind = (hit->unknown08 != 0) ? 1 : 2;
                self->casts[CAST_STEP].hit = *hit;
                Vec3ScaleAddQ27(hit->distance, &castDir, &castOrigin, &self->casts[CAST_STEP].pos);
                reach = VEC_Distance(&self->casts[CAST_STEP].pos, &originVec);
                if (heightScale + moveLen > reach) {
                    if (faceNormal.y > 0x333) {
                        fallbackHit = 1;
                        flags = self->flags;
                        flags |= 0x800;
                        self->flags = flags;
                    } else {
                        self->casts[CAST_GROUND].pos = self->casts[CAST_STEP].pos;
                        stepFound = 1;
                    }
                } else {
                    hit = 0;
                }
            }
        }
    }

    if (hit != 0) {
        if (fallbackHit == 0) {
            hitRecorded = 1;
            self->casts[CAST_GROUND].kind = (hit->unknown08 != 0) ? 1 : 2;
            if (stepFound == 0) {
                self->casts[CAST_GROUND].hit = *hit;
            }
            flags = self->flags;
            flags |= 2;
            self->flags = flags;
        }

        slideScale = (GetFrameRateMode() == 1) ? 0x400 : 0xb00;
        resolved = 0;

        mag = VEC_DotProduct(&heightVec, &faceNormal);
        VEC_MultAdd(-mag, &faceNormal, &heightVec, &slideVec);
        mag = VEC_Normalize(&slideVec, &slideDir);

        flags = self->flags;
        if (flags & 0x400) {
            if (VEC_DotProduct(&slideDir, &faceNormal) < 0) {
                VEC_MultAdd(0x600, &faceNormal, &slideDir, &slideDir);
                if (slideDir.y < 0) slideDir.y = 0;
                VEC_Normalize(&slideDir, &slideDir);
            }
        } else if (flags & 0x800) {
            if (VEC_DotProduct(&slideDir, &faceNormal) > 0) {
                VEC_MultAdd(-0x600, &faceNormal, &slideDir, &slideDir);
                if (slideDir.y > 0) slideDir.y = 0;
                VEC_Normalize(&slideDir, &slideDir);
            }
        }

        ScaleVec3Fx12(mag + heightScale - 0x7f0, (int *)&slideDir, (int *)&castDir);

        castOrigin = originVec;
        castOrigin.y = obj->pos.y + 0x66 + 0xa00;
        params.radius = 0x800;
        hit = Collision_RunSphereCast(world, &params);
        if (hit == 0) {
            castOrigin.y = obj->pos.y + 0x66 + 0x600;
            hit = Collision_RunSphereCast(world, &params);
            if (hit != 0 && hit->unknown08 == 0) {
                hit = 0;
            }
        }

        if (hit == 0) {
            if (closeRange) {
                pOut->z = 0;
                pOut->y = 0;
                pOut->x = 0;
            } else {
                flags = self->flags;
                if (flags & 2) {
                    ScaleVec3Fx12(slideScale, (int *)&slideVec, (int *)pOut);
                    pOut->y = 0;
                    anchorBase = self->casts[CAST_GROUND].pos;
                    resolved = 1;
                } else if (flags & 0x400) {
                    ScaleVec3Fx12(moveLen, (int *)&slideDir, (int *)pOut);
                }
                if (self->flags & 0x80) {
                    *pOut = slideVec;
                }
            }
        } else {
            self->casts[CAST_RESOLVED].kind = (hit->unknown08 != 0) ? 1 : 2;
            Vec3ScaleAddQ27(hit->distance, &castDir, &castOrigin, &self->casts[CAST_RESOLVED].pos);
            self->casts[CAST_RESOLVED].pos.y = obj->pos.y;
            self->casts[CAST_RESOLVED].hit = *hit;
            flags = self->flags;
            flags |= 2;
            self->flags = flags;
            pOut->z = 0;
            pOut->y = 0;
            pOut->x = 0;
            anchorBase = self->casts[CAST_RESOLVED].pos;
            resolved = 1;
        }

        if (resolved) {
            int gap;
            anchorBase.y = obj->pos.y;
            gap = VEC_Distance(&anchorBase, &obj->pos);
            if (heightScale > gap) {
                pushScale = -205;
                if (closeRange) {
                    pushScale = -2;
                }
                if (gap < 0x19a) {
                    pushScale = gap - (int)(((fx64)heightScale * 0x600 + 0x800) >> 12);
                    self->flags |= 0x200;
                } else {
                    self->flags &= ~0x200;
                }
                VEC_MultAdd(pushScale, &faceNormal, &self->anchor, &self->anchor);
                VEC_Normalize(&self->anchor, &self->anchor);
                ScaleVec3Fx12(pushScale, (int *)&self->anchor, (int *)&self->anchor);
            } else {
                flags = self->flags;
                flags &= ~0x200;
                self->flags = flags;
                self->anchor.z = 0;
                self->anchor.y = 0;
                *(int *)&self->anchor = 0;
            }
        } else {
            flags = self->flags;
            flags &= ~0x200;
            self->flags = flags;
            self->anchor.z = 0;
            self->anchor.y = 0;
            *(int *)&self->anchor = 0;
        }
    }

    if (hitRecorded == 0) {
        if (closeRange) {
            pOut->x = 0;
            pOut->z = 0;
        }
    }

    return hitRecorded;
}

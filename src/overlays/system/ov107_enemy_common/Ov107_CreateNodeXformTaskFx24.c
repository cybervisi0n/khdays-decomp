/* Builds an SRT (uniform scale, translation from packed 24-bit coordinates) and hands it to
 * Ov107_CreateNodeXformTask. */

#include "nitro/fx_types.h"

typedef struct { unsigned char hi, mid, lo; } Fx24;
typedef union { struct { unsigned char pad, lo, mid, hi; } b; int w; } Fx24Word;
typedef struct { int w[11]; } BoneXform;

extern void SrtTransform_SetIdentity(BoneXform *p);
extern void Srt_SetScaleUniform(BoneXform *p, int weight);
extern void Srt_SetTranslation(BoneXform *dst, VecFx32 *src);
extern void *Ov107_CreateNodeXformTask(void *taskList, void *subitem, int mode, int blend,
                                  BoneXform *xform);

void *Ov107_CreateNodeXformTaskFx24(void *taskList, void *subitem, int mode, int blend,
                           int weight, Fx24 *payload) {
    BoneXform local;

    SrtTransform_SetIdentity(&local);
    Srt_SetScaleUniform(&local, weight);

    if (payload != 0) {
        VecFx32 decoded;
        Fx24Word d[3];

        d[0].b.hi = payload[0].hi;
        d[0].b.mid = payload[0].mid;
        d[0].b.lo = payload[0].lo;
        decoded.x = d[0].w >> 8;

        d[1].b.hi = payload[1].hi;
        d[1].b.mid = payload[1].mid;
        d[1].b.lo = payload[1].lo;
        decoded.y = d[1].w >> 8;

        d[2].b.hi = payload[2].hi;
        d[2].b.mid = payload[2].mid;
        d[2].b.lo = payload[2].lo;
        decoded.z = d[2].w >> 8;

        Srt_SetTranslation(&local, &decoded);
    }

    return Ov107_CreateNodeXformTask(taskList, subitem, mode, blend, &local);
}

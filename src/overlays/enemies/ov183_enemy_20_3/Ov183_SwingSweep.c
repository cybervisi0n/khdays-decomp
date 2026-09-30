/*
 * Swing sweep of the ov181 enemy (x4: ov181/182/183/184): from the +0x14 heading builds the
 * forward vector (sine/cosine table), a 0.5 push along it and a probe sphere 1.375 ahead of the
 * +8 position with radius 1.0; every actor in the sphere that takes the push through the ov107
 * hit hook (the caller's kind) gets a kind-5 message whose cmd byte is 2 when the target's
 * +0x7c facing agrees with the swing (kind 1 flips the sense) and 0 otherwise, carrying the
 * impact point (sphere centre plus push) delivered to the owner's +0x24 handler, plus reaction
 * 0x131/5 at that point.
 * Coordinates are packed through a wrapped copy (the Ov131_throwRelease_tick spelling): copying
 * a one-value struct is a struct copy mwcc keeps, which is the ROM's late stack copy.
 *
 * Codegen: the impact point is packed through per-component Fx32 wrapper copies (ov122_020d12f4
 * spelling); `pMsg = &msg` taken after the template copy keeps the message address in r8 across
 * the side test, and the command byte is spelled `flip == 0 ? 0 : 2`. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct { int value; } Fx32;
typedef struct { Fx32 x, y, z; } FxVec;
typedef struct { VecFx32 pos; int radius; } Sphere;

struct Msg {
    u16 h[7];
};

struct Ov181Actor {
    char pad000[0x24];
    void (*pfnMessage)(struct Ov181Actor *self, struct Msg *msg, int size);
};

extern void ScaleVec3Fx12(int scale, const VecFx32 *v, VecFx32 *out);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *out);
extern int Ov107_CollectSphereOverlaps(struct Ov181Actor *owner, Sphere *sphere, struct Ov181Actor **out);
extern int Ov107_InvokeHitCallback(struct Ov181Actor *hit, struct Ov181Actor *a, struct Ov181Actor *b, int kind, const VecFx32 *push, int z);
extern char **Ov107_GetActorManager(void);
extern int VEC_DotProduct(const VecFx32 *a, const VecFx32 *b);
extern void Ov107_BuildAndSendUpdate(struct Ov181Actor *owner, u16 a, u16 id, VecFx32 *pos);
extern const short data_0203d210[];
extern const struct Msg data_ov183_020d2644;

void Ov183_SwingSweep(int *state, int kind)
{
    VecFx32 fwd;
    VecFx32 push;
    Sphere sphere;
    struct Ov181Actor *hits[4];
    VecFx32 impact;
    struct Msg msg;
    struct Msg tmpl;
    FxVec vContact;
    struct Msg *pMsg;
    int idx;
    int i;
    int n;
    int flip;

    idx = (unsigned short)((0x28BE60DB9391LL * state[5] + 0x80000000000LL) >> 44);   /* FX_RAD_TO_IDX */
    fwd.x = data_0203d210[(idx >> 4) << 1];                                              /* FX_SinIdx */
    fwd.y = 0;
    fwd.z = data_0203d210[((idx >> 4) << 1) + 1];                                        /* FX_CosIdx */
    ScaleVec3Fx12(0x800, &fwd, &push);
    ScaleVec3Fx12(0x1600, &fwd, &sphere.pos);
    VEC_Add((VecFx32 *)state[2], &sphere.pos, &sphere.pos);
    sphere.radius = 0x1000;
    n = Ov107_CollectSphereOverlaps((struct Ov181Actor *)*state, &sphere, hits);
    i = 0;
    if (n > 0) {
        tmpl = data_ov183_020d2644;
        do {
            if (Ov107_InvokeHitCallback(hits[i], (struct Ov181Actor *)*state, (struct Ov181Actor *)*state, kind, &push, 0) != 0) {
                msg = tmpl;
                pMsg = &msg;
                flip = kind == 1 ? 1 : 0;
                if (VEC_DotProduct((VecFx32 *)(*Ov107_GetActorManager() + 0x7c), &fwd) > 0) {
                    flip = (flip + 1) & 1;
                }
                ((u8 *)pMsg)[3] = flip == 0 ? 0 : 2;
                VEC_Add(&sphere.pos, &push, &impact);
                vContact.x = *(Fx32 *)&impact.x;
                ((u8 *)&msg)[5] = (u8)(((u32)vContact.x.value >> 16 & 0x7f) | ((u32)vContact.x.value >> 24 & 0x80));
                ((u8 *)&msg)[6] = (u8)((u32)vContact.x.value >> 8);
                ((u8 *)&msg)[7] = (u8)vContact.x.value;
                vContact.y = *(Fx32 *)&impact.y;
                ((u8 *)&msg)[8] = (u8)(((u32)vContact.y.value >> 16 & 0x7f) | ((u32)vContact.y.value >> 24 & 0x80));
                ((u8 *)&msg)[9] = (u8)((u32)vContact.y.value >> 8);
                ((u8 *)&msg)[10] = (u8)vContact.y.value;
                vContact.z = *(Fx32 *)&impact.z;
                ((u8 *)&msg)[11] = (u8)(((u32)vContact.z.value >> 16 & 0x7f) | ((u32)vContact.z.value >> 24 & 0x80));
                ((u8 *)&msg)[12] = (u8)((u32)vContact.z.value >> 8);
                ((u8 *)&msg)[13] = (u8)vContact.z.value;
                if (((struct Ov181Actor *)*state)->pfnMessage != 0) {
                    ((struct Ov181Actor *)*state)->pfnMessage((struct Ov181Actor *)*state, &msg, 0xe);
                }
                Ov107_BuildAndSendUpdate((struct Ov181Actor *)*state, 0x131, 5, &impact);
            }
            i++;
        } while (i < n);
    }
}

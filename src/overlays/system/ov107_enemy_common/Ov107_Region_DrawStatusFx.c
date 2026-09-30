/* Draws the status-effect model on every visible member with the given status bit, facing the
 * camera. */

#include "nitro/fx_types.h"
#include "game/actor.h"

typedef struct {
    char pad0[0xc4];
    int f_c4;
    int f_c8;
    int f_cc;
    char padd0[0xd4 - 0xd0];
    int f_d4;
} Xform394;

typedef struct { char pad[0x20]; int f20; } Ctx;
typedef struct { int m[9]; } Mtx33;

extern void *List_First(void *list);
extern void *List_Next(void *list);
extern void *Ov107_GetActorManager(void);
extern void VEC_Subtract(VecFx32 *a, VecFx32 *b, VecFx32 *out);
extern int VEC_Normalize(VecFx32 *src, VecFx32 *dst);
extern void ScaleVec3Fx12(int factor, VecFx32 *src, VecFx32 *dst);
extern void VEC_Add(VecFx32 *a, VecFx32 *b, VecFx32 *out);
extern void MTX_Identity33_(Mtx33 *m);
extern void Gfx_SubmitCachedCommandBlock(void);
extern void Obj_InitChannelsAndRun(unsigned int *p);

extern Xform394 data_02047394;
extern VecFx32 data_0204744c;
extern Mtx33 data_02047428;

void Ov107_Region_DrawStatusFx(char *self, int action) {
    VecFx32 *g = &data_0204744c;
    char *base = *(char **)(self + 0x84);
    void *listNode;

    (*(Ctx **)(self + 0x88))->f20 |= 1;

    listNode = List_First(base + 0x80);
    while (listNode != 0) {
        Actor *n = *(Actor **)listNode;

        if (action & n->flags1c4) {
            unsigned int f60 = (unsigned)(n->flags60.raw << 24) >> 24;
            if ((f60 & 1) != 0) {
                if ((n->field_1ac & 7) == 0) {
                    int v = n->sphere.radius << 1;
                    VecFx32 *src;
                    void *thread;
                    VecFx32 result;

                    data_02047394.f_c4 = v;
                    data_02047394.f_c8 = v;
                    data_02047394.f_cc = 1;

                    src = n->field_2cc != 0 ? (VecFx32 *)n->field_2cc : &n->sphere.center;
                    *g = *src;

                    thread = Ov107_GetActorManager();
                    VEC_Subtract((VecFx32 *)((char *)*(void **)thread + 0x88), g, &result);
                    VEC_Normalize(&result, &result);
                    ScaleVec3Fx12(n->sphere.radius, &result, &result);
                    VEC_Add(g, &result, g);

                    if (n->flags1c4 & 8) {
                        g->y += n->sphere.radius;
                    }

                    MTX_Identity33_(&data_02047428);
                    data_02047394.f_d4 &= ~0xa4;
                    Gfx_SubmitCachedCommandBlock();
                    Obj_InitChannelsAndRun((unsigned int *)&(*(Ctx **)(self + 0x88))->f20);
                }
            }
        }
        listNode = List_Next(base + 0x80);
    }
}

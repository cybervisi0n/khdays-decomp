/* Pushes the position away (in view space) from every active animation slot within the radius. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct MtxFx33 {
    int value[9];
} MtxFx33;

typedef struct Ov022AnimationSlot {
    u8 active00;
    char padding001[0xa7];
    VecFx32 positionA8;
    char padding0b4[0x60];
} Ov022AnimationSlot;

typedef struct Ov022AnimationRoot {
    u8 countAndFlags00;
    char padding001[0x0b];
    Ov022AnimationSlot *slots0c;
} Ov022AnimationRoot;

typedef struct Ov022DispatchContext {
    char padding000[0x20];
    Ov022AnimationRoot *animation20;
} Ov022DispatchContext;

extern MtxFx33 data_020473e0;
extern s16 data_0203d210[];

extern void MI_Copy36B(const void *source, void *destination);
extern int VEC_Distance(const VecFx32 *a, const VecFx32 *b);
extern int Rand16NextScaled(u32 range);
extern void ScaleVec3Fx12(int scale, const VecFx32 *source, VecFx32 *destination);
extern void MTX_MultVec33(const VecFx32 *source, const MtxFx33 *matrix,
                          VecFx32 *destination);
extern void VEC_Add(const VecFx32 *a, const VecFx32 *b, VecFx32 *destination);

void Ov022_AvoidAnimationSlots(Ov022DispatchContext *context,
                          const VecFx32 *position, int radius,
                          VecFx32 *output)
{
    int index;
    Ov022AnimationRoot *root;
    VecFx32 displacement;
    VecFx32 result;
    MtxFx33 viewMatrix;

    root = context->animation20;
    result = *position;
    MI_Copy36B(&data_020473e0, &viewMatrix);

    index = 0;
    if ((int)(((u32)root->countAndFlags00 << 24) >> 26) > 0) {
        int offset;
        int minusOne;

        offset = 0;
        minusOne = -1;
        do {
            Ov022AnimationSlot *slots;

            slots = root->slots0c;
            if (*(u8 *)((char *)slots + offset) != 0 &&
                VEC_Distance(&result,
                    (VecFx32 *)((char *)slots + offset + 0xa8)) < radius) {
                int angle;

                angle = Rand16NextScaled(0x8000) >> 4;
                displacement.x = data_0203d210[angle * 2];
                displacement.y = data_0203d210[angle * 2 + 1];
                displacement.z = 0;
                if (displacement.y < 0) {
                    displacement.y = displacement.y * minusOne;
                }
                ScaleVec3Fx12(radius, &displacement, &displacement);
                MTX_MultVec33(&displacement, &viewMatrix, &displacement);
                VEC_Add(&displacement, &result, &result);
            }
            index++;
            offset += 0x114;
        } while (index <
                 (int)(((u32)root->countAndFlags00 << 24) >> 26));
    }

    *output = result;
}


#include "nitro/fx/fx.h"
#include "nitro/fx/fx_mtx.h"
#include "nitro/fx/fx_vec.h"

extern void INITi_CpuClear32_0x01ff86fc(unsigned int data, void *dst, unsigned int size);
extern void ScaleVec3Fx12(int factor, int *src, int *dst);
extern VecFx32 data_02042264;
extern VecFx32 data_02042258;

/* Build a look-at rotation matrix: forward = to - from, with up as the
 * preferred reference vector. If up is (near) parallel to forward the
 * reference falls back to the world axis data_02042264, then to
 * data_02042258, before giving up and zeroing the matrix. */
int Mtx33_LookAt(MtxFx33 *out, VecFx32 *from, VecFx32 *to, VecFx32 *up)
{
    VecFx32 forward;
    VecFx32 ref;
    VecFx32 proj;
    VecFx32 right;

    #ifdef SDK_BUILD_ARM
    //TODO
    VEC_Subtract((int *)to, (int *)from, (int *)&forward);
    if (VEC_Normalize(&forward, &forward) == 0) {
        INITi_CpuClear32_0x01ff86fc(0, out, sizeof(MtxFx33));
        return 0;
    }

    ScaleVec3Fx12(VEC_DotProduct(up, &forward), (int *)&forward, (int *)&proj);
    VEC_Subtract((int *)up, (int *)&proj, (int *)&ref);
    if (VEC_Normalize(&ref, &ref) == 0) {
        ScaleVec3Fx12(forward.y, (int *)&forward, (int *)&proj);
        VEC_Subtract((int *)&data_02042264, (int *)&proj, (int *)&ref);
        if (VEC_Normalize(&ref, &ref) == 0) {
            ScaleVec3Fx12(forward.z, (int *)&forward, (int *)&proj);
            VEC_Subtract((int *)&data_02042258, (int *)&proj, (int *)&ref);
            if (VEC_Normalize(&ref, &ref) == 0) {
                INITi_CpuClear32_0x01ff86fc(0, out, sizeof(MtxFx33));
                return 0;
            }
        }
    }
    #endif

    VEC_CrossProduct(&ref, &forward, &right);
    out->_00 = -right.x;
    out->_10 = ref.x;
    out->_20 = -forward.x;
    out->_01 = -right.y;
    out->_11 = ref.y;
    out->_21 = -forward.y;
    out->_02 = -right.z;
    out->_12 = ref.z;
    out->_22 = -forward.z;
    return 1;
}

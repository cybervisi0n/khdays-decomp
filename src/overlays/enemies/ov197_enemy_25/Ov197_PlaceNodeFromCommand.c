/* Command handler: place one of the two attached nodes from a packed transform.
 *
 * Only acts on command 5 targeting slot 0 or 1.  The command carries three 24-bit
 * BIG-ENDIAN fixed-point components at +5, +8 and +0xb; each is reassembled by writing its
 * three bytes REVERSED into the high end of a stack word and taking `>> 8`, which both
 * swaps the byte order and sign-extends from bit 23 in one go.  Those become a VecFx32, an
 * identity transform is built and translated by it, and the result is handed to the
 * placement helper; the returned handle replaces the slot at owner + slot*8 + 0x398.
 * Everything else in the command stream falls through to the shared tail.
 *
 * TWO CODEGEN POINTS:
 *  - the slot guard must be written as `!(slot != 0 && slot != 1)`.  Spelled as
 *    `slot == 0 || slot == 1` mwcc folds it into an unsigned range test (`cmp #1 ; bhi`)
 *    and the function comes out 4 bytes short; the De Morgan form gives the ROM's
 *    `cmp #0 ; cmpne #1 ; bne` chain.  Reversing the operands or going through an int
 *    local does not help -- only the negation does.  (A three-or-more-value chain, as in
 *    Ov*_ReleaseByStateAndSyncSrt, has no range to fold into and needs no such care.)
 *  - declaration order sets the frame: the 0x2c-byte transform is at the TOP of the frame
 *    and the byte-assembly buffer at the bottom, so they are declared in that order --
 *    mwcc gives the lower address to the later declaration.
 */

#include "nitro/fx_types.h"

typedef struct { int w[11]; } SrtTransform;

extern void SrtTransform_SetIdentity(SrtTransform *m);
extern void Srt_SetTranslation(SrtTransform *m, const VecFx32 *v);
extern int Ov107_CreateNodeXformTask(int a, int node, int kind, int z, SrtTransform *m);
extern void Ov107_AiState_OnMessage(int owner, unsigned char *cmd, int arg);

void Ov197_PlaceNodeFromCommand(int owner, unsigned char *cmd, int arg) {
    SrtTransform m;
    VecFx32 v;
    union { int w[3]; unsigned char b[12]; } t;

    if (cmd[2] == 5 && !(cmd[3] != 0 && cmd[3] != 1)) {
        t.b[3] = cmd[5];
        t.b[2] = cmd[6];
        t.b[1] = cmd[7];
        v.x = t.w[0] >> 8;
        t.b[7] = cmd[8];
        t.b[6] = cmd[9];
        t.b[5] = cmd[0xa];
        v.y = t.w[1] >> 8;
        t.b[11] = cmd[0xb];
        t.b[10] = cmd[0xc];
        t.b[9] = cmd[0xd];
        v.z = t.w[2] >> 8;

        SrtTransform_SetIdentity(&m);
        Srt_SetTranslation(&m, &v);
        *(int *)((char *)owner + cmd[3] * 8 + 0x398) =
            Ov107_CreateNodeXformTask(*(int *)(owner + 0x3c),
                                *(int *)((char *)owner + cmd[3] * 8 + 0x394), 0x17, 0, &m);
    }
    Ov107_AiState_OnMessage(owner, cmd, arg);
}

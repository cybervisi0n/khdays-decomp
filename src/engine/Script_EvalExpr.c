#pragma thumb on
/* Script_EvalExpr -- evaluate a script expression, MAIN (THUMB). The expression is postfix bytecode
 * (u16 opcodes, 0xf ends it) run on the value stack of the script context (+0x178, 8-byte entries
 * {type, value}; type 0x10 is fixed point, 1 is integer). Opcode 0 pushes the operand that follows
 * (4 halfwords, resolved by ScriptVm_ResolveOperand); 1 negates the top, 2 is logical not; 3..6 are + - * /
 * and 7..12 the comparisons == > < >= <= !=, computed in fixed point (Script_ValueToFx32) when either
 * operand is fixed and in integers (GetField4UnlessState2) otherwise -- arithmetic keeps the operand type,
 * comparisons give an integer; 13 and 14 are && and ||. Returns the value left on top. The fixed
 * point addition adds the lower operand to itself, as in the original. The ROM rereads the opcode
 * for the dispatch instead of reusing the loop test's load, hence the volatile read. */

#include "nitro/types.h"
#include "nitro/fx_types.h"

typedef struct ScriptValue {
    s16 type;                           /* +0x00 */
    s16 pad02;
    int value;                          /* +0x04 */
} ScriptValue;

typedef struct ScriptContext {
    char pad000[0x178];
    ScriptValue stack[1];               /* +0x178 */
} ScriptContext;

#define SCRIPT_FIXED 0x10
#define SCRIPT_INT   1

extern void MIi_CpuCopy16(const void *src, void *dest, unsigned int size);
extern ScriptValue *ScriptVm_ResolveOperand(ScriptContext *ctx, u16 *operand);
extern fx32 Script_ValueToFx32(ScriptValue *v);      /* value as fixed point */
extern int GetField4UnlessState2(ScriptValue *v);       /* value as integer */
extern fx32 FX_Mul(fx32 a, fx32 b);      /* FX_Mul */
extern fx32 FX_Div(fx32 a, fx32 b);             /* FX_Div */
extern int func_02020400(int a, int b);         /* integer divide */

#define BINARY_CMP(OP)                                                          \
    {                                                                           \
        ScriptValue *b = &ctx->stack[sp - 1];                                   \
        ScriptValue *a = &ctx->stack[sp - 2];                                   \
                                                                                \
        if (a->type == SCRIPT_FIXED || b->type == SCRIPT_FIXED) {               \
            fx32 x = Script_ValueToFx32(a);                                          \
                                                                                \
            a->value = (x OP Script_ValueToFx32(b)) ? 1 : 0;                         \
            a->type = SCRIPT_INT;                                               \
        } else {                                                                \
            int x = GetField4UnlessState2(a);                                           \
                                                                                \
            a->value = (x OP GetField4UnlessState2(b)) ? 1 : 0;                         \
            a->type = SCRIPT_INT;                                               \
        }                                                                       \
        sp--;                                                                   \
    }

ScriptValue *Script_EvalExpr(ScriptContext *ctx, u16 *code)
{
    int sp = 0;

    while (*code != 0xf) {
        switch (*(volatile u16 *)code) {
        case 0: {
            u16 operand[4];

            MIi_CpuCopy16(code + 1, operand, sizeof(operand));
            ctx->stack[sp] = *ScriptVm_ResolveOperand(ctx, operand);
            sp++;
            code += 4;
            break;
        }
        case 1: {
            ScriptValue *v = &ctx->stack[sp - 1];

            v->value = -v->value;
            break;
        }
        case 2: {
            ScriptValue *v = &ctx->stack[sp - 1];

            v->value = v->value == 0 ? 1 : 0;
            break;
        }
        case 3: {
            ScriptValue *b = &ctx->stack[sp - 1];
            ScriptValue *a = &ctx->stack[sp - 2];

            if (a->type == SCRIPT_FIXED || b->type == SCRIPT_FIXED) {
                fx32 x = Script_ValueToFx32(a);

                a->value = x + Script_ValueToFx32(a);
                a->type = SCRIPT_FIXED;
            } else {
                int x = GetField4UnlessState2(a);

                a->value = x + GetField4UnlessState2(b);
                a->type = SCRIPT_INT;
            }
            sp--;
            break;
        }
        case 4: {
            ScriptValue *b = &ctx->stack[sp - 1];
            ScriptValue *a = &ctx->stack[sp - 2];

            if (a->type == SCRIPT_FIXED || b->type == SCRIPT_FIXED) {
                fx32 x = Script_ValueToFx32(a);

                a->value = x - Script_ValueToFx32(b);
                a->type = SCRIPT_FIXED;
            } else {
                int x = GetField4UnlessState2(a);

                a->value = x - GetField4UnlessState2(b);
                a->type = SCRIPT_INT;
            }
            sp--;
            break;
        }
        case 5: {
            ScriptValue *b = &ctx->stack[sp - 1];
            ScriptValue *a = &ctx->stack[sp - 2];

            if (a->type == SCRIPT_FIXED || b->type == SCRIPT_FIXED) {
                fx32 x = Script_ValueToFx32(a);

                a->value = FX_Mul(x, Script_ValueToFx32(b));
                a->type = SCRIPT_FIXED;
            } else {
                int x = GetField4UnlessState2(a);

                a->value = x * GetField4UnlessState2(b);
                a->type = SCRIPT_INT;
            }
            sp--;
            break;
        }
        case 6: {
            ScriptValue *b = &ctx->stack[sp - 1];
            ScriptValue *a = &ctx->stack[sp - 2];

            if (a->type == SCRIPT_FIXED || b->type == SCRIPT_FIXED) {
                fx32 x = Script_ValueToFx32(a);

                a->value = FX_Div(x, Script_ValueToFx32(b));
                a->type = SCRIPT_FIXED;
            } else {
                int x = GetField4UnlessState2(a);

                a->value = func_02020400(x, GetField4UnlessState2(b));
                a->type = SCRIPT_INT;
            }
            sp--;
            break;
        }
        case 7:
            BINARY_CMP(==)
            break;
        case 8:
            BINARY_CMP(>)
            break;
        case 9:
            BINARY_CMP(<)
            break;
        case 10:
            BINARY_CMP(>=)
            break;
        case 11:
            BINARY_CMP(<=)
            break;
        case 12:
            BINARY_CMP(!=)
            break;
        case 13: {
            ScriptValue *b = &ctx->stack[sp - 1];
            ScriptValue *a = &ctx->stack[sp - 2];

            a->value = (a->value != 0 && b->value != 0) ? 1 : 0;
            sp--;
            break;
        }
        case 14: {
            ScriptValue *b = &ctx->stack[sp - 1];
            ScriptValue *a = &ctx->stack[sp - 2];

            a->value = (a->value != 0 || b->value != 0) ? 1 : 0;
            sp--;
            break;
        }
        }
        code++;
    }
    return &ctx->stack[sp - 1];
}
#pragma thumb off

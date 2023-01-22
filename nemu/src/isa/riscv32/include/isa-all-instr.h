/*
 * @Author: Runze Li lirunze.me@gmail.com
 * @Date: 2023-01-11 02:09:44
 * @LastEditors: Runze Li
 * @LastEditTime: 2023-01-21 20:30:12
 * @Description:  
 */
#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(auipc) \
                      f(jal) f(jalr) \
                      f(beq) f(bne) f(blt) f(bge) \
                      f(lh) f(lw) f(lbu) \
                      f(sb) f(sh) f(sw) \
                      f(addi) f(sltiu) f(xori) f(andi) f(srai) \
                      f(add) f(sub) f(sll) f(slt) f(sltu) f(xor) f(or) f(and) \
                      f(mul) f(div) f(rem) \
                      f(inv) f(nemu_trap)

def_all_EXEC_ID();

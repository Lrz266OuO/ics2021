/*
 * @Author: Runze Li lirunze.me@gmail.com
 * @Date: 2023-01-11 02:09:44
 * @LastEditors: Runze Li
 * @LastEditTime: 2023-01-21 01:58:32
 * @Description:  
 */
#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(auipc) \
                      f(jal) f(jalr) \
                      f(beq) f(bne) \
                      f(lw) f(lbu) \
                      f(sh) f(sw) \
                      f(addi) f(sltiu) f(xori) f(andi) f(srai) \
                      f(add) f(sub) f(sll) f(sltu) f(xor) f(or) f(and) \
                      f(inv) f(nemu_trap)

def_all_EXEC_ID();

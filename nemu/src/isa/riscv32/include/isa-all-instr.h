/*
 * @Author: Runze Li lirunze.me@gmail.com
 * @Date: 2023-01-11 02:09:44
 * @LastEditors: Runze Li
 * @LastEditTime: 2023-01-20 00:43:24
 * @Description:  
 */
#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(auipc) \
                      f(jal) f(jalr) \
                      f(lw) \
                      f(sw) \
                      f(addi) f(sltiu) \
                      f(add) f(sltu) f(xor) f(or) \
                      f(inv) f(nemu_trap)

def_all_EXEC_ID();

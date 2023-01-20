/*
 * @Author: Runze Li lirunze.me@gmail.com
 * @Date: 2023-01-11 02:09:44
 * @LastEditors: Runze Li
 * @LastEditTime: 2023-01-19 22:09:39
 * @Description:  
 */
#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(auipc) \
                      f(jal) f(jalr) \
                      f(lw) \
                      f(sw) \
                      f(addi) \
                      f(add) f(sltu) \
                      f(inv) f(nemu_trap)

def_all_EXEC_ID();

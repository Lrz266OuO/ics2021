/*
 * @Author: Runze Li lirunze.me@gmail.com
 * @Date: 2023-01-11 02:09:44
 * @LastEditors: Runze Li
 * @LastEditTime: 2023-01-12 19:55:00
 * @Description:  
 */
#include <isa.h>
#include "local-include/reg.h"

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

// 打印寄存器状态
void isa_reg_display() {
  int i;
  for (i=0; i<32; i++) {
    // printf("%s\t0x%08x\n", regs[i], gpr(i));
    printf("%s\t0x%08x\n", reg_name(i, 0), gpr(i));
  }
  printf("pc\t0x%08x\n", cpu.pc);
  return;
}

word_t isa_reg_str2val(const char *s, bool *success) {
  return 0;
}

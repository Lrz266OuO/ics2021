/*
 * @Author: Runze Li lirunze.me@gmail.com
 * @Date: 2023-01-11 02:09:44
 * @LastEditors: Runze Li
 * @LastEditTime: 2023-01-17 00:19:23
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
  for (i = 0; i < 32; i++) {
    // reg_name函数中第二个参数width不知道是做什么用的，暂且设定为0
    printf("%s\t0x%08x\n", reg_name(i, 0), gpr(i));
  }
  printf("pc\t0x%08x\n", cpu.pc);
  return;
}

// 通过寄存器名字找到寄存器
word_t isa_reg_str2val(const char *s, bool *success) {
  // 别忘了cpu.pc
  if (strcmp(s, "$pc") == 0) return cpu.pc;
  // 注意regs[]的名称，第一个是$0，后面都省略了$
  else if (strcmp(s, reg_name(0, 0)) == 0) return gpr(0);
  else {
    int i;
    for (i = 1; i < 32; i++) {
      if (strcmp(s+1, reg_name(i, 0)) == 0) {
        return gpr(i);
      }
    }
    // 未找到寄存器名称
    if (i == 32) {
      *success = false;
      printf("Wrong register: %s\n", s);
      return 0;
    }
  }
  return 0;
}

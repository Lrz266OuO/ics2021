/*
 * @Author: Runze Li lirunze.me@gmail.com
 * @Date: 2023-01-18 23:43:00
 * @LastEditors: Runze Li
 * @LastEditTime: 2023-01-19 20:16:37
 * @Description:  
 */

def_EHelper(jal) {
  rtl_addi(s, ddest, &s->pc, 4);
  rtl_addi(s, &s->dnpc, &s->pc, id_src1->imm);
  // stack_call(s->pc, s->dnpc);
}

def_EHelper(jalr) {
  rtl_addi(s, s0, dsrc1, id_src2->imm);
  rtl_andi(s, &s->dnpc, s0, ~1);
  rtl_addi(s, ddest, &s->pc, 4);
  // stack_?
}

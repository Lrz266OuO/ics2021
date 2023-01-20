/*
 * @Author: Runze Li lirunze.me@gmail.com
 * @Date: 2023-01-18 23:43:00
 * @LastEditors: Runze Li
 * @LastEditTime: 2023-01-19 23:26:53
 * @Description:  
 */

def_EHelper(jal) {
  rtl_addi(s, ddest, &s->pc, 4);
  // rtl_addi(s, s0, &s->pc, id_src1->imm);
  // rtl_j(s, *s0);
  rtl_j(s, *((&s->pc) + (id_src1->imm)));
  /* TODO: The standard software calling convention 
   * uses x1 as the return address register 
   * and x5 as an alternate link register.
   */
}

def_EHelper(jalr) {
  rtl_addi(s, s0, dsrc1, id_src2->imm);
  rtl_andi(s, &s->dnpc, s0, ~1);
  rtl_addi(s, ddest, &s->pc, 4);
  /* TODO: Register x0 can be used as the destination if the result is not required.*/
}

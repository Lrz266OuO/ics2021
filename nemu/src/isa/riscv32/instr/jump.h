/*
 * @Author: Runze Li lirunze.me@gmail.com
 * @Date: 2023-01-18 23:43:00
 * @LastEditors: Runze Li
 * @LastEditTime: 2023-01-19 23:39:35
 * @Description:  
 */

def_EHelper(jal) {
  rtl_addi(s, ddest, &s->pc, 4);
  rtl_addi(s, &s->dnpc, &s->pc, id_src1->imm);
  /* TODO: The standard software calling convention 
   * uses x1 as the return address register 
   * and x5 as an alternate link register.
   */
}

def_EHelper(jalr) {
  rtl_addi(s, ddest, &s->pc, 4);
  rtl_addi(s, &s->dnpc, dsrc1, id_src2->imm);
  rtl_andi(s, &s->dnpc, &s->dnpc, ~1);
  /* TODO: Register x0 can be used as the destination if the result is not required.*/
}

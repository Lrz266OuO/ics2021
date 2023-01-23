/*
 * @Author: Runze Li lirunze.me@gmail.com
 * @Date: 2023-01-11 02:09:44
 * @LastEditors: Runze Li
 * @LastEditTime: 2023-01-22 21:30:13
 * @Description:  
 */

def_EHelper(lb) {
  rtl_lms(s, ddest, dsrc1, id_src2->imm, 1);
}

def_EHelper(lh) {
  /* LH loads a 16-bit value from memory,
   * then sign-extends to 32-bits before storing in rd.
   */
  rtl_lms(s, ddest, dsrc1, id_src2->imm, 2);
}

def_EHelper(lw) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 4);
}

def_EHelper(lbu) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 1);
}

def_EHelper(lhu) {
  rtl_lm(s, ddest, dsrc1, id_src2->imm, 2);
}

def_EHelper(sb) {
  rtl_sm(s, ddest, dsrc1, id_src2->imm, 1);
}

def_EHelper(sh) {
  rtl_sm(s, ddest, dsrc1, id_src2->imm, 2);
}

def_EHelper(sw) {
  rtl_sm(s, ddest, dsrc1, id_src2->imm, 4);
}

/*
 * @Author: Runze Li lirunze.me@gmail.com
 * @Date: 2023-01-11 02:09:44
 * @LastEditors: Runze Li
 * @LastEditTime: 2023-01-22 04:43:41
 * @Description:  
 */
def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
}

def_EHelper(auipc) {
  rtl_addi(s, ddest, &s->pc, id_src1->imm);
}

def_EHelper(addi) {
  rtl_addi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(sltiu) {
  rtl_setrelopi(s, RELOP_LTU, ddest, dsrc1, id_src2->imm);
}

def_EHelper(xori) {
  rtl_xori(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(andi) {
  rtl_andi(s, ddest, dsrc1, id_src2->imm);
}

def_EHelper(slli) {
  rtl_slli(s, ddest, dsrc1, (id_src2->imm & 0x1f));
}

def_EHelper(srli) {
  rtl_srli(s, ddest, dsrc1, (id_src2->imm & 0x1f));
}

def_EHelper(srai) {
  /* Shifts by a constant are encoded as a specialization of the I-type format. 
   * The operand to be shifted is in rs1, 
   * and the shift amount is encoded in the lower 5 bits of the I-immediate field. 
   * The right shift type is encoded in bit 30. 
   * SLLI is a logical left shift (zeros are shifted into the lower bits);
   * SRLI is a logical right shift (zeros are shifted into the upper bits); 
   * and SRAI is an arithmetic right shift (the original sign bit is copied into the vacated upper bits).
   */
  rtl_srai(s, ddest, dsrc1, (id_src2->imm & 0x1f));
}

def_EHelper(add) {
  rtl_add(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sub) {
  rtl_sub(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sll) {
  rtl_sll(s, ddest, dsrc1, dsrc2);
}

def_EHelper(slt) {
  rtl_setrelop(s, RELOP_LT, ddest, dsrc1, dsrc2);
}

def_EHelper(sltu) {
  /* SLT and SLTU perform signed and unsigned compares respectively, 
   * writing 1 to rd if rs1 < rs2, 0 otherwise.
   * Note, SLTU rd, x0, rs2 sets rd to 1 if rs2 is not equal to zero, 
   * otherwise sets rd to zero (assembler pseudoinstruction SNEZ rd, rs).
   */
  rtl_setrelop(s, RELOP_LTU, ddest, dsrc1, dsrc2);
}

def_EHelper(xor) {
  rtl_xor(s, ddest, dsrc1, dsrc2);
}

def_EHelper(sra) {
  rtl_sra(s, ddest, dsrc1, dsrc2);
}

def_EHelper(or) {
  rtl_or(s, ddest, dsrc1, dsrc2);
}

def_EHelper(and) {
  rtl_and(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mul) {
  /* MUL performs an XLEN-bit*XLEN-bit multiplication of rs1 by rs2 
   * and places the lower XLEN bits in the destination register.
   */
  rtl_mulu_lo(s, ddest, dsrc1, dsrc2);
}

def_EHelper(mulh) {
  /* MULH, MULHU, and MULHSU perform the same multiplication 
   * but return the upper XLEN bits of the full 2×XLEN-bit product, 
   * for signed×signed, unsigned×unsigned, and signed rs1×unsigned rs2 multiplication, respectively.
   */
  rtl_muls_hi(s, ddest, dsrc1, dsrc2);
}

def_EHelper(div) {
  /* DIV and DIVU perform an XLEN bits by XLEN bits 
   * signed and unsigned integer division of rs1 by rs2, 
   * rounding towards zero.
   */
  rtl_divs_q(s, ddest, dsrc1, dsrc2);
}

def_EHelper(rem) {
  /* REM and REMU provide the remainder of the corresponding division operation. 
   * For REM, the sign of the result equals the sign of the dividend.
   */
  rtl_divs_r(s, ddest, dsrc1, dsrc2);
}

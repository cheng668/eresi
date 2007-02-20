#include "libasm.h"

int
asm_sparc_stqfa(asm_instr * ins, u_char * buf, u_int len,
		asm_processor * proc)
{
  struct s_decode_format3 opcode;
  struct s_asm_proc_sparc *inter;
  sparc_convert_format3(&opcode, buf, proc);
  inter = proc->internals;
  ins->instr = inter->op3_table[opcode.op3];
  
  ins->type = ASM_TYPE_STORE;

  ins->nb_op = 2;
  if (opcode.i) {
    ins->op1.type = ASM_SP_OTYPE_IMM_ADDRESS;
    ins->op1.imm = opcode.imm;
    /* This value doesn't matter. Just has to be != 0x80 */
    ins->op1.address_space = -1;
  }
  else {
    ins->op1.type = ASM_SP_OTYPE_REG_ADDRESS;
    ins->op1.index_reg = opcode.rs2;
    ins->op1.address_space = opcode.none;
  }
  ins->op1.base_reg = opcode.rs1;

  ins->op2.type = ASM_SP_OTYPE_FREGISTER;
  ins->op2.base_reg = opcode.rd;
  
  return 4;

}
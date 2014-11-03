#include "mips.h"
#include "mips_cpu_safe.h"
#include <string>
#include <stdlib.h>
#include "mips_cpu_helpers.h"
#include "mips_cpu_r.h"
#include "mips_cpu_i.h"


struct mips_cpu_impl
{
	uint32_t pc;
	uint32_t pcN;
	uint32_t regs[32];
	uint32_t hi;
	uint32_t lo;
	mips_mem_h mem;
};

mips_cpu_h mips_cpu_create(mips_mem_h mem)
{
	unsigned i;
	mips_cpu_h res=(mips_cpu_h)malloc(sizeof(struct mips_cpu_impl));

	if (res == NULL)
		return 0;
	res->mem=mem;

	res->pc=0;
	res->pcN=4;	// NOTE: why does this make sense?

	for( i=0;i<32;i++){
		res->regs[i]=0;
	}

	return res;
}
mips_error mips_cpu_reset(mips_cpu_h state)
{
	for (unsigned int i = 0; i < 32; i++)
		state->regs[i] = 0;
	state->pc = 0;
	state->pcN = 4;

	return mips_Success;
}
void mips_cpu_free(mips_cpu_h state)
{
	free(state);
}

mips_error mips_cpu_get_register(
	mips_cpu_h state,	//!< Valid (non-empty) handle to a CPU
	unsigned index,		//!< Index from 0 to 31
	uint32_t *value		//!< Where to write the value to
	)
{
	if(state==0)
		return mips_ErrorInvalidHandle;
	if(index>=32)
		return mips_ErrorInvalidArgument;
	if(value==0)
		return mips_ErrorInvalidArgument;

	if (index == 0) // $0 must always be 0
		*value = 0;
	else
		*value = state->regs[index];

	return mips_Success;
}

/*! Modifies one of the 32 general purpose MIPS registers. */
mips_error mips_cpu_set_register(
	mips_cpu_h state,	//!< Valid (non-empty) handle to a CPU
	unsigned index,		//!< Index from 0 to 31
	uint32_t value		//!< New value to write into register file
)
{
	if(state==0)
		return mips_ErrorInvalidHandle;
	if(index>=32)
		return mips_ErrorInvalidArgument;

	if (index == 0)
		return mips_ErrorInvalidArgument; // Don't change $0
	else
		state->regs[index]=value;

	return mips_Success;
}

mips_error mips_cpu_set_pc(
	mips_cpu_h state,	//!< Valid (non-empty) handle to a CPU
	uint32_t pc			//!< Address of the next instruction to exectute.
	)
{
	if (state == 0)
		return mips_ErrorInvalidHandle;

	if (pc < 0) //Put an upper bound by accessing memory size?
		return mips_ExceptionInvalidAddress;

	state->pc = pc;

	return mips_Success;
}

mips_error mips_cpu_get_pc(mips_cpu_h state, uint32_t *pc)
{


	*pc = state->pc;

	return mips_Success;
}

mips_error mips_cpu_step(mips_cpu_h state)
{
	
	if(state==0)
		return mips_ErrorInvalidHandle;

	state->regs[0] = 0;
	uint32_t pc=state->pc;


	// - Fetch the instruction from memory 

	uint8_t encoding_bytes[4];
	mips_error err = mips_mem_read(state->mem, pc, 4, encoding_bytes);

	//Reversing the order of the bytes
	uint8_t temp;
	unsigned i;
	for (i = 0; i < 2; i++)    
	{
		temp = encoding_bytes[4 - i - 1];
		encoding_bytes[4 - i - 1] = encoding_bytes[i];
		encoding_bytes[i] = temp;
	}

	uint32_t encoding = (encoding_bytes[3]) | (encoding_bytes[2] << 8) | (encoding_bytes[1] << 16) | (encoding_bytes[0] << 24);

	// - Decode the instruction (is it R, I, J)?
	char type = decode_instruction_type(encoding_bytes);
	
	
	//Decode the operation of the instruction and execute
	if (type == 'r')
	{
		uint8_t func = encoding_bytes[3] & 0x3F; //Shift out top 2 bits to leave func code
		uint32_t rs; // Source 1 
		uint32_t rt; // Source 2
		err = get_source_regs_r(state, rs, rt, encoding);
		uint32_t sa; // Shift Amount
		sa = (encoding >> 6) & 0x1F;

		switch (func)
		{
		case 0x20: // 10 0000 ADD

			if (signed_overflow(rs, rt, rs + rt))
				return mips_ExceptionArithmeticOverflow;

			err = set_dest_reg_r(state, encoding, (int32_t)rs + (int32_t)rt);
			
			break;
			
		case 0x21: // 10 0001 ADDU

			err = set_dest_reg_r(state, encoding, rs + rt);
			
			break;

		case 0x24: //10 0100 AND

			err = set_dest_reg_r(state, encoding, rs & rt);
			
			break;
			
		case 0x1A: // 01 1010 DIV
		{
			if (rt != 0)
			{
				// Unsure about whether the remainder should be -ve or +ve when dividing a -ve number: This gives a -ve remainder
				int32_t remainder = (int32_t)rs % (int32_t)rt;
				int32_t quotient = (int32_t)rs / (int32_t)rt;

				state->lo = (uint32_t)quotient;
				state->hi = (uint32_t)remainder;
			}

			else
			{
				state->lo = 0;
				state->hi = 0;
			}

			break;
		}
		case 0x1B: // 01 1011 DIVU
		{
			if (rt != 0)
			{
				uint32_t remainder = rs % rt;
				uint32_t quotient = rs / rt;

				state->lo = quotient;
				state->hi = remainder;
			}

			else
			{
				state->lo = 0;
				state->hi = 0;
			}

			break;
		}

		case 0x08: // 00 1000 JR

			if ((encoding & 0x1FFFC0) != 0)
				return mips_ErrorInvalidArgument;
			if ((rs & 0x3) != 0)
				err = mips_ExceptionInvalidAddress;

			// Increment pc to execute the next instruction (stored in the branch delay slot)
			state->pc += 4;

			state->pcN = rs;
			return mips_Success;

		case 0x25: //10 0101 OR
	
			err = set_dest_reg_r(state, encoding, rs | rt);
			
			break;

		case 0x10: // 01 0000 MFHI

			if (rs | rt) // rs and rt must both be 0
				return mips_ExceptionInvalidInstruction;

	
			err = set_dest_reg_r(state, encoding, state->hi);
			
			break;

		case 0x12: // 01 0010 MFLO

			if (rs | rt) // rs and rt must both be 0
				return mips_ExceptionInvalidInstruction;

			err = set_dest_reg_r(state, encoding, state->lo);
			
			break;

		case 0x18: // 01 1000 MULT
		{
			
			int64_t result = (int64_t)((int32_t)rs) * (int64_t)((int32_t)rt);

			state->lo = (int32_t)(result & 0xFFFFFFFF);
			state->hi = (int32_t)((result & 0xFFFFFFFF00000000) >> 32);
			
			break;
		}

		case 0x19: // 01 1001 MULTU
		{
			uint64_t result = (uint64_t)rs * (uint64_t)rt;

			state->lo = (uint32_t)(result & 0xFFFFFFFF);
			state->hi = (uint32_t)((result & 0xFFFFFFFF00000000) >> 32);
			
			break;
		}
		case 0x00: // 00 0000 SLL

			if (rs != 0) // rs must be 00000 for sll
				return mips_ExceptionInvalidInstruction;

			err = set_dest_reg_r(state, encoding, rt << sa);
			
			break;

		case 0x04: // 00 0100 SLLV

			if (sa != 0) // sa must be 00000 for sll
				return mips_ExceptionInvalidInstruction;

			err = set_dest_reg_r(state, encoding, rt << rs);
			
			break;

		case 0x2A: // 10 1010 SLT

			if ((int32_t)rs < (int32_t)rt)
				err = set_dest_reg_r(state, encoding, 0x1);
			else
				err = set_dest_reg_r(state, encoding, 0);

			
			break;

		case 0x2B: // 10 1011 SLTU

			if (rs < rt)
				err = set_dest_reg_r(state, encoding, 0x1);
			else
				err = set_dest_reg_r(state, encoding, 0);

			
			break;

		case 0x03: // 00 0011 SRA

			if (rs != 0) // rs must be 00000 for sra
				return mips_ExceptionInvalidInstruction;

			if (rt & 0x80000000) // If negative, sign extend
			{
				for (unsigned i = 0; i < sa; i++)
				{
					rt >>= 1;
					rt |= 0x80000000;
				}
			}
			else // If positive, zero extend
				rt >>= sa;

			err = set_dest_reg_r(state, encoding, rt);
			
			break;

		case 0x02: // 00 0010 SRL

			if (rs != 0) // rs must be 00000 for srl
				return mips_ExceptionInvalidInstruction;

			err = set_dest_reg_r(state, encoding, rt >> sa);
			
			break;

		case 0x06: // 00 0110 SRLV

			err = set_dest_reg_r(state, encoding, rt >> rs);
			
			break;

		case 0x22: //10 0010 SUB

			if (signed_overflow(rs, rt, rs - rt))
				return mips_ExceptionArithmeticOverflow;

			err = set_dest_reg_r(state, encoding, rs - rt);

			break;
		case 0x23: //10 0011 SUBU

			err = set_dest_reg_r(state, encoding, rs - rt);
			
			break;
			
		case 0x26: //10 0110 XOR
			
			err = set_dest_reg_r(state, encoding, rs ^ rt);
			
			break;

		default:
			return mips_ExceptionInvalidInstruction;
		}
	}

	else if (type == 'i')
	{
		uint16_t imm = encoding & 0xFFFF;
		uint32_t opcode = encoding_bytes[0] >> 2;
		uint32_t rs; // Source reg
		
		err = get_source_reg_i(state, rs, encoding);

		switch (opcode)
		{
		case 0x8: // 0010 00 ADDI
			
			if (signed_overflow(rs, imm, rs + imm))
				return mips_ExceptionArithmeticOverflow;

			set_dest_reg_i(state, encoding, (int32_t)rs + (int32_t)imm);
			
			break;

		case 0x9: // 0010 01 ADDIU
			
			set_dest_reg_i(state, encoding, rs + imm);
			
			break;
				
		case 0xC: // 0011 00 ANDI
			
			imm = (uint32_t)imm; // Zero extension

			set_dest_reg_i(state, encoding, rs & imm);
			
			break;

		case 0x4: // 0001 00 BEQ
		{
			uint32_t rt;
			err = mips_cpu_get_register(state, (encoding >> 16) & 0x1F, &rt);

			int32_t offset = (int32_t)imm << 2;

			// Increment pc to execute the next instruction (stored in the branch delay slot)
			state->pc += 4; 

			if (rs == rt)
				state->pcN += offset;

			return mips_Success;
		}

		case 0x1: // REGIMM == 0000 01 : BGEZ/BGEZAL/BLTZ/BLTZAL
		{
			int32_t offset = (int32_t)imm << 2;
			uint32_t rt = (encoding >> 16) & 0x1F;
			switch (rt)
			{
			case 0x01: // 0000 01 BGEZ


				// Increment pc to execute the next instruction (stored in the branch delay slot)
				state->pc += 4;
				
				if ((int32_t)rs >= 0)
					state->pcN += offset;

				return mips_Success;
			case 0x11: // 1 0001 BGEZAL
				// Increment pc to execute the next instruction (stored in the branch delay slot)
				state->pc += 4;

				//Store link address
				state->regs[31] = state->pc + 4;

				if ((int32_t)rs >= 0)
					state->pcN += offset;

				return mips_Success;
			
			case 0x00: // 0 0000 BLTZ

				// Increment pc to execute the next instruction (stored in the branch delay slot)
				state->pc += 4;

				if ((int32_t)rs < 0)
					state->pcN += offset;

				return mips_Success;

			case 0x10: // 1 0000 BLTZAL

				// Increment pc to execute the next instruction (stored in the branch delay slot)
				state->pc += 4;

				//Store link address
				state->regs[31] = state->pc + 4;

				if ((int32_t)rs < 0)
					state->pcN += offset;

				return mips_Success;

			}

			break;
		}

		case 0x07: // 0001 11 BGTZ
		{
			int32_t offset = (int32_t)imm << 2;

			// Increment pc to execute the next instruction (stored in the branch delay slot)
			state->pc += 4;

			if ((int32_t)rs > 0)
				state->pcN += offset;

			return mips_Success;
		}

		case 0x06: // 0001 10 BLEZ
		{
			uint32_t rt;
			err = mips_cpu_get_register(state, (encoding >> 16) & 0x1F, &rt);

			int32_t offset = (int32_t)imm << 2;

			// Increment pc to execute the next instruction (stored in the branch delay slot)
			state->pc += 4;

			if ((int32_t)rs <= (int32_t)rt)
				state->pcN += offset;

			return mips_Success;
		}

		case 0x05: // 0001 01 BNE
		{
			uint32_t rt;
			err = mips_cpu_get_register(state, (encoding >> 16) & 0x1F, &rt);

			int32_t offset = (int32_t)imm << 2;

			// Increment pc to execute the next instruction (stored in the branch delay slot)
			state->pc += 4;

			if (rs != rt)
				state->pcN += offset;

			return mips_Success;
		}

		case 0xF: // 0011 11 LUI

			imm = (uint32_t)imm; // Zero extension

			set_dest_reg_i(state, encoding, imm << 16);
			
			break;

		case 0x20: // 1000 00 LB
		{
			imm = (int32_t)imm; // Sign extension

			unsigned int index = 3 - ((rs + imm) % 4); // 3-index to get big endian address -- Not sure if I'm doing endianness correctly here
			uint32_t address = rs + imm - ((rs + imm) % 4);
			uint8_t word_bytes[4];
			err = mips_mem_read(state->mem, address, 4, word_bytes);

			//Big endian value will be taken from memory and stored in rt still big endian
			int32_t byte = (int32_t)((int8_t)(word_bytes[index]));

			set_dest_reg_i(state, encoding, byte);

			break;
		}

		case 0x24: // 1001 00 LBU
		{
			imm = (int32_t)imm; // Sign extension

			unsigned int index = 3 - ((rs + imm) % 4); // 3-index to get big endian address -- Not sure if I'm doing endianness correctly here
			uint32_t address = rs + imm - ((rs + imm) % 4);
			uint8_t word_bytes[4];
			err = mips_mem_read(state->mem, address, 4, word_bytes);

			//Big endian value will be taken from memory and stored in rt still big endian
			uint32_t byte =(uint32_t)word_bytes[index];

			set_dest_reg_i(state, encoding, byte);
			
			break;
		}

		case 0x23: // 1000 11 LW
		{
			imm = (int32_t)imm; // Sign extension

			if ((rs +imm) % 4)
				return mips_ExceptionInvalidAlignment;

			uint8_t word_bytes[4];
			err = mips_mem_read(state->mem, rs + imm, 4, word_bytes);

			//Big endian value will be taken from memory and stored in rt still big endian
			uint32_t word = (word_bytes[3]) | (word_bytes[2] << 8) | (word_bytes[1] << 16) | (word_bytes[0] << 24);

			set_dest_reg_i(state, encoding, word);
			
			break;
		}
		case 0xD: // 0011 01 ORI

			imm = (uint32_t)imm; // Zero extension

			set_dest_reg_i(state, encoding, rs | imm);
			
			break;
		
		case 0x28: // 1010 00 SB
		{
			imm = (int32_t)imm; // Sign extension

			uint32_t rt;
			err = mips_cpu_get_register(state, (encoding >> 16) & 0x1F, &rt);

			unsigned int index = (rs + imm) % 4;

			uint32_t current_word; // Word currently in memory at effective address (rs + imm)
			err = mips_mem_read(state->mem, rs + imm - index, 4, (uint8_t*)&current_word);

			uint8_t word_bytes[4] =
			{
				current_word & 0xFF, // Byte 0 of current word
				(current_word & 0xFF00) >> 8, // Byte 1 of current word
				(current_word & 0xFF0000) >> 16, // Byte 2 of current word
				(current_word & 0xFF000000) >> 24 // Byte 3 of current word
			};

			word_bytes[index] = rt & 0xFF; // byte i of rt

			err = mips_mem_write(state->mem, rs + imm, 4, word_bytes);
			
			break;
		}

		case 0x29: // 1010 01 SH
		{
			imm = (int32_t)imm; // Sign extension

			uint32_t rt;
			err = mips_cpu_get_register(state, (encoding >> 16) & 0x1F, &rt);

			unsigned int index = (rs + imm) % 4;
			if (index == 1 || index == 3)
				return mips_ExceptionInvalidAddress;

			uint32_t current_word; // Word currently in memory at effective address (rs + imm)
			err = mips_cpu_get_register(state, rs + imm - index, &current_word);

			uint8_t word_bytes[4];
			if (index == 0)
			{
				
				word_bytes[0] = (current_word & 0xFF000000) >> 24; // Byte 3 of current word
				word_bytes[1] = (current_word & 0xFF0000) >> 16; // Byte 2 of current word
				word_bytes[2] = (rt & 0xFF00) >> 8; //byte 1 of rt
				word_bytes[3] = rt & 0xFF; // byte 0 of rt
				
			}
			else if (index == 2)
			{
				
				word_bytes[0] = (rt & 0xFF00) >> 8; //byte 1 of rt
				word_bytes[1] = (rt & 0xFF); // byte 0 of rt
				word_bytes[2] = (current_word & 0xFF00) >> 8; // Byte 3 of current word
				word_bytes[3] = current_word & 0xFF; // Byte 2 of current word
				
			}

			err = mips_mem_write(state->mem, rs + imm, 4, word_bytes);
			
			break;
		}

		case 0x0A: // 00 1010 SLTI

			if ((int32_t)rs < (int32_t)imm)
				err = set_dest_reg_i(state, encoding, 0x1);
			else
				err = set_dest_reg_i(state, encoding, 0);

			
			break;

		case 0x0B: // 00 1011 SLTIU

			sign_extend_16_to_32(imm);
			imm = (uint32_t)imm; // Spec states to sign extend then treat as unsigned.

			if (rs < imm)
				err = set_dest_reg_i(state, encoding, 0x1);
			else
				err = set_dest_reg_i(state, encoding, 0);

			
			break;
		case 0x2B: // 1010 11 SW
		{
			imm = (int32_t)imm; // Sign extension

			uint32_t rt;
			err = mips_cpu_get_register(state, (encoding >> 16) & 0x1F, &rt);


			if (((rs + imm) % 4) != 0)
				return mips_ExceptionInvalidAddress;

			uint8_t word_bytes[4] =
			{
				word_bytes[0] = (rt & 0xFF000000) >> 24, // Byte 3 of current word
				word_bytes[1] = (rt & 0xFF0000) >> 16, // Byte 2 of current word
				word_bytes[2] = (rt & 0xFF00) >> 8, //byte 1 of rt
				word_bytes[3] = rt & 0xFF // byte 0 of rt
			};
			

			err = mips_mem_write(state->mem, rs + imm, 4, word_bytes);

			break;
		}
		case 0xE: // 0011 10 XORI

			imm = (uint32_t)imm; // Zero extension

			set_dest_reg_i(state, encoding, rs ^ imm);
			
			break;

		default:
			return mips_ExceptionInvalidInstruction; 
		}
	}

	else if (type == 'j')
	{
		uint8_t opcode = encoding_bytes[0] >> 2;
		uint32_t instr_index = encoding & 0x3FFFFFF;
		uint32_t target_address = instr_index << 2;

		switch (opcode)
		{
		case 0x02: // 0000 10 J
		{

			// Increment pc to execute the next instruction (stored in the branch delay slot)
			state->pc += 4;

			uint32_t upper_pc = state->pc & 0xF0000000;
			target_address |= upper_pc;

			state->pcN = target_address;
			return mips_Success;
		}
		case 0x03: // 0000 11 JAL
		{
			// Increment pc to execute the next instruction (stored in the branch delay slot)
			state->pc += 4;

			uint32_t upper_pc = state->pc & 0xF0000000;
			target_address |= upper_pc;
			
			state->regs[31] = state->pc + 4; // Set LR to address of second instruction after branch
			state->pcN = target_address;
			return mips_Success;
		}

		}
	}

	state->pc = state->pcN;
	state->pcN += 4;
	return err;

}

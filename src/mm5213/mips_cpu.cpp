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

	mips_mem_h mem;
};

mips_cpu_h mips_cpu_create(mips_mem_h mem)
{
	unsigned i;
	mips_cpu_h res=(mips_cpu_h)malloc(sizeof(struct mips_cpu_impl));

	res->mem=mem;

	res->pc=0;
	res->pcN=4;	// NOTE: why does this make sense?

	for( i=0;i<32;i++){
		res->regs[i]=0;
	}

	return res;
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

	// TODO : What about register zero?
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
	uint32_t pc=state->pc;

	if(state==0)
		return mips_ErrorInvalidHandle;

	
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
	char type;
	if (encoding_bytes[0] >> 2 == 0) // if any of first 6 bits are 1, it is not r type
		type = 'r';
	else if (encoding_bytes[0] >> 4 == 0)// if any of the first 4 bits are 1, it is not j type
		type = 'j';
	else
		type = 'i';
	
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

			err = set_dest_reg_r(state, encoding, (signed)rs + (signed)rt);
			state->pc += state->pcN;
			break;
			
		case 0x21: // 10 0001 ADDU

			err = set_dest_reg_r(state, encoding, rs + rt);
			state->pc += state->pcN;
			break;

		case 0x24: //10 0100 AND

			err = set_dest_reg_r(state, encoding, rs & rt);
			state->pc += state->pcN;
			break;
			
		case 0x25: //10 0101 OR
	
			err = set_dest_reg_r(state, encoding, rs | rt);
			state->pc += state->pcN;
			break;

		case 0x2A: // 10 1010 SLT

			if ((signed)rs < (signed)rt)
				err = set_dest_reg_r(state, encoding, 0x1);
			else
				err = set_dest_reg_r(state, encoding, 0);

			state->pc += state->pcN;
			break;

		case 0x03: // 00 0011 SRA

			if (rs != 0) // rs must be 00000 for sra
				return mips_ExceptionInvalidInstruction;

			if (rt & 0x80000000) // If negative, sign extend
			{
				for (int i = 0; i < sa; i++)
				{
					rt >>= 1;
					rt |= 0x80000000;
				}
			}
			else // If positive, zero extend
				rt >>= sa;

			err = set_dest_reg_r(state, encoding, rt);
			state->pc += state->pcN;
			break;

		case 0x02: // 00 0010 SRL

			if (rs != 0) // rs must be 00000 for srl
				return mips_ExceptionInvalidInstruction;

			err = set_dest_reg_r(state, encoding, rt >> sa);
			state->pc += state->pcN;
			break;

		case 0x06: // 00 0110 SRLV

			err = set_dest_reg_r(state, encoding, rt >> rs);
			state->pc += state->pcN;
			break;

		case 0x23: //10 0011 SUBU

			err = set_dest_reg_r(state, encoding, rs - rt);
			state->pc += state->pcN;
			break;
			
		case 0x26: //10 0110 XOR
			
			err = set_dest_reg_r(state, encoding, rs ^ rt);
			state->pc += state->pcN;
			break;
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

			set_dest_reg_i(state, encoding, (signed)rs + (signed)imm);
			state->pc += state->pcN;
			break;

		case 0x9: // 0010 01 ADDIU
			
			set_dest_reg_i(state, encoding, rs + imm);
			state->pc += state->pcN;
			break;
				
		case 0xC: // 0011 00 ANDI
			
			imm = (uint32_t)imm; // Zero extension

			set_dest_reg_i(state, encoding, rs & imm);
			state->pc += state->pcN;
			break;

		case 0xF: // 0011 11 LUI

			imm = (uint32_t)imm; // Zero extension

			set_dest_reg_i(state, encoding, imm << 16);
			state->pc += state->pcN;
			break;

		case 0x23: // 1000 11 LW
		{
			imm = (int32_t)imm; // Sign extension

			if ((rs +imm) % 4)
				return mips_ExceptionInvalidAlignment;

			uint8_t word_bytes[4];
			err = mips_mem_read(state->mem, rs + imm, 4, word_bytes);

			//Big endian value will be taken from memory and stored in rt stil big endian
			uint32_t word = (word_bytes[3]) | (word_bytes[2] << 8) | (word_bytes[1] << 16) | (word_bytes[0] << 24);

			set_dest_reg_i(state, encoding, word);
			state->pc += state->pcN;
			break;
		}
		case 0xD: // 0011 01 ORI
			
			imm = (uint32_t)imm; // Zero extension

			set_dest_reg_i(state, encoding, rs | imm);
			state->pc += state->pcN;
			break;

		case 0xE: // 0011 10 XORI

			imm = (uint32_t)imm; // Zero extension

			set_dest_reg_i(state, encoding, rs ^ imm);
			state->pc += state->pcN;
			break;
		}
	}

	if (!err)
		return mips_Success;
	else
		return err;

	//return mips_ErrorNotImplemented;
}

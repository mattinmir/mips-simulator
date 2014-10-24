#include "mips.h"
#include "mips_cpu_safe.h"
#include <string>
#include <stdlib.h>

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

	//TODO: Here is where the magic happens
	// - Fetch the instruction from memory (mips_mem_read)
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
	

	std::string instruction;
	if (type == 'r')
	{
		uint8_t func = (encoding_bytes[3] << 2) >> 2; //Shift out top 2 bits to leave func code

		if (func == 0x24)//0010 0100 AND
			instruction = "AND";
	}
	// - Execute the instruction (do maths, access memory, ...)



	/********** r type *************/
	if (instruction == "AND")
	{
		uint32_t rs = encoding >> 21; // extract first 5 bit source register. No need to shift left as opcode will be all 0's anyway
		mips_error err = mips_cpu_get_register(state, rs, &rs);

		uint32_t rt = (encoding >> 16) & 0x1F; // extract second 5 bit source register. 
		err = mips_cpu_get_register(state, rt, &rt);

		// - Writeback the results (update registers, advance pc)
		uint32_t rd = (encoding >> 11) & 0x1F; // extract 5 bit destination register. 
		err = mips_cpu_set_register(state, rd, rs&rt);

		uint32_t r10;
		err = mips_cpu_get_register(state, 10, &r10);
		state->pc += state->pcN;

		if (!err)
			return mips_Success;
	}
	

	return mips_ErrorNotImplemented;
}

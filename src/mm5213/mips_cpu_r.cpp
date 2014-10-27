#include "mips_cpu_r.h"

mips_error get_source_regs_r(mips_cpu_h &state, uint32_t &rs, uint32_t &rt, const uint32_t &encoding)
{
	rs = (encoding >> 21) & 0x1F; // extract first 5 bit source register 
	mips_error err = mips_cpu_get_register(state, rs, &rs);

	rt = (encoding >> 16) & 0x1F; // extract second 5 bit source register
	err = mips_cpu_get_register(state, rt, &rt);

	return err;
}

mips_error set_dest_reg_r(mips_cpu_h &state, const uint32_t &encoding, const uint32_t &value)
{
	uint32_t rd = (encoding >> 11) & 0x1F; // extract 5 bit destination register
	mips_error err = mips_cpu_set_register(state, rd, value);

	return err;
}

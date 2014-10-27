#include "mips_cpu_helpers.h"

mips_error get_source_regs(mips_cpu_h &state, uint32_t &rs, uint32_t &rt, const uint32_t &encoding)
{
	rs = (encoding >> 21) & 0x1F; // extract first 5 bit source register 
	mips_error err = mips_cpu_get_register(state, rs, &rs);

	rt = (encoding >> 16) & 0x1F; // extract second 5 bit source register
	err = mips_cpu_get_register(state, rt, &rt);

	return err;
}

mips_error set_dest_reg(mips_cpu_h &state, const uint32_t &encoding, const uint32_t &value)
{
	uint32_t rd = (encoding >> 11) & 0x1F; // extract 5 bit destination register
	mips_error err = mips_cpu_set_register(state, rd, value);

	return err;
}

bool signed_overflow(uint32_t s, uint32_t t, uint32_t result)
{
	uint32_t s_sign = s & 0x80000000;
	uint32_t t_sign = t & 0x80000000;
	uint32_t result_sign = result & 0x80000000;

	bool overflow;
	if (s_sign != t_sign) // If the signs are different, there is no signed overflow
		overflow = false;

	else if (result_sign != s_sign)
		overflow = true;

	else
		overflow = false;

	return overflow;
}

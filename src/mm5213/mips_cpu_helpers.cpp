#include "mips_cpu_helpers.h"


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


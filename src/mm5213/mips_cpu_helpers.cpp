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

void sign_extend(uint32_t &value)
{
	value = (int32_t)((int16_t)value);
}

uint32_t sra(uint32_t rt, uint32_t sa)
{
	if (rt & 0x8000)
	{
		rt |= 0xFFFF0000;

		for (int i = 0; i < sa; i++)
		{
			rt >>= 1;
			rt |= 0x80000000;
		}
	}
	else
		rt >>= sa;

	return rt;

}
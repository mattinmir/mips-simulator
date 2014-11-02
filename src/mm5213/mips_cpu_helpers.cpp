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

void sign_extend_16_to_32(uint16_t &value)
{
	value = (int32_t)((int16_t)value);
}

char decode_instruction_type(uint8_t encoding_bytes[4])
{
	char type;

	if (encoding_bytes[0] >> 2 == 0) // if all 6 bits are 0, it is r type
	type = 'r';
	else if (encoding_bytes[0] >> 4 == 0 && (encoding_bytes[0] & 0x8))// if the first 4 bits are 0 and the 5th bit is 1, it is j type
	type = 'j';
	else
	type = 'i';

	return type;
}

#include "mips.h"

bool signed_overflow(uint32_t s, uint32_t t, uint32_t result);

void sign_extend_16_to_32(uint16_t &value);

char decode_instruction_type(uint8_t encoding_bytes[4]);
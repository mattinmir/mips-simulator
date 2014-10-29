#include "mips.h"

bool signed_overflow(uint32_t s, uint32_t t, uint32_t result);

void sign_extend(uint32_t &value);

uint32_t sra(uint32_t rt, uint32_t sa);
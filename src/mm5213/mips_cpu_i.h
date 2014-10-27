#include "mips.h"

mips_error get_source_reg_i(mips_cpu_h &state, uint32_t &rs, const uint32_t &encoding);

mips_error set_dest_reg_i(mips_cpu_h &state, const uint32_t &encoding, const uint32_t &value);
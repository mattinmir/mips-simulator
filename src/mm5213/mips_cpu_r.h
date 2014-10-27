#include "mips.h"

mips_error get_source_regs_r(mips_cpu_h &state, uint32_t &rs, uint32_t &rt, const uint32_t &encoding);

mips_error set_dest_reg_r(mips_cpu_h &state, const uint32_t &encoding, const uint32_t &value);

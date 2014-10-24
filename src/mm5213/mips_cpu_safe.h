#include "mips_cpu.h"

void mips_cpu_free_safe(mips_cpu_h state)
{
	mips_cpu_free(state);
	state = 0;
}
#include "mips_test.h"
#include "mips.h"
#include <assert.h>


int main()
{
	mips_mem_h mem = mips_mem_create_ram(
		1<<20,
		4
		);

	mips_cpu_h cpu = mips_cpu_create(mem);

	mips_test_begin_suite();

	uint32_t address = 0;

	int testId = mips_test_begin_test("and");
	int passed = 0;

	
	mips_error err = mips_cpu_set_register(cpu, 8, 0x0000FFFFul);
	if(!err)
		err = mips_cpu_set_register(cpu, 9, 0x00FFFF00ul);
	//if (!err)
	//{
	//	uint32_t* reg8 = new uint32_t;
	//	uint32_t* reg9 = new uint32_t;

	//	err = mips_cpu_set_register(cpu, 10, *reg8 & *reg9);

	//	delete reg8, reg9;
	//}
	
	// TODO : work out the bit-wise encoding for the instruction.
	if (!err)
	{


		uint8_t encoding[4] = { 0x24, 0x50, 0x09, 0x01 }; // and $10, $8, $9 : 0000 0001 0000 1001 0101 0000 0010 0100 (big endian)

		// TODO : Write it into mempory at a known address
		err = mips_mem_write(mem, address, 4, encoding);
	}
	// TODO : Make sure the program-counter is at that address
	uint32_t pc;
	err = mips_cpu_set_pc(cpu, address);
	/*if (!err)
		err = mips_cpu_get_pc(cpu, &pc);*/
	if (!err)
		err = mips_cpu_step(cpu);
	

	address += 4;
	uint32_t got;
	if(!err)
		err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

	passed = (err == mips_Success) && (got==0x0000FF00);

	mips_test_end_test(testId, passed, NULL);



	mips_test_end_suite();

	mips_cpu_free(cpu);
	cpu = 0;
	mips_mem_free(mem);

	return 0;
}

#include "mips_test.h"
#include "mips.h"

#define DEBUG_LEVEL 0

int main()
{
	mips_mem_h mem = mips_mem_create_ram(
		1 << 20,
		4
		);
	mips_cpu_h cpu = mips_cpu_create(mem);
	mips_test_begin_suite();

	uint32_t address = 0x0;
	uint8_t word_bytes[4];

	mips_error err;
	int testId;
	int passed = 0;

	/**************************************************************************************************/
	/********************************************* ADD ************************************************/
	/**************************************************************************************************/

	//--------------------------------------------- 1 ------------------------------------------------//
	{
		uint32_t got;
		testId = mips_test_begin_test("add");
		passed = 0;

		err = mips_cpu_set_register(cpu, 8, 0x000000FFul);
		if (!err)
			err = mips_cpu_set_register(cpu, 9, 0x0000FF00ul);

		if (!err)
		{
			uint8_t encoding_bytes[4] = { 0x20, 0x50, 0x09, 0x01 }; // add $10, $8, $9 : 0000 0001 0000 1001 0101 0000 0010 0000 (encoding_bytes is big endian form of this)

			// Write encoding into memory at a known address
			err = mips_mem_write(mem, address, 4, encoding_bytes);
		}

		//  Make sure the program-counter is at that address
		if (!err)
			err = mips_cpu_set_pc(cpu, address);

		if (!err)
			err = mips_cpu_step(cpu);

		address += 4;

		if (!err)
			err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

		passed = (err == mips_Success) && (got == 0x0000FFFF);

		mips_test_end_test(testId, passed, NULL);
	}
	/**************************************************************************************************/
	/********************************************* ADDI ***********************************************/
	/**************************************************************************************************/

	//--------------------------------------------- 1 ------------------------------------------------//
	{
		uint32_t got;
		testId = mips_test_begin_test("addi");
		passed = 0;

		err = mips_cpu_set_register(cpu, 8, 0x000000FFul);


		if (!err)
		{
			uint8_t encoding_bytes[4] = { 0x34, 0x12, 0xA, 0x21 }; // addi $10, $8, 0x1234 : 0010 0001 0000 1010 0001 0010 0011 0100 (encoding_bytes is big endian form of this)

			// Write encoding into memory at a known address
			err = mips_mem_write(mem, address, 4, encoding_bytes);
		}

		//  Make sure the program-counter is at that address
		if (!err)
			err = mips_cpu_set_pc(cpu, address);

		if (!err)
			err = mips_cpu_step(cpu);

		address += 4;

		if (!err)
			err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

		passed = (err == mips_Success) && (got == 0x00001333);

		mips_test_end_test(testId, passed, NULL);
	}
	/**************************************************************************************************/
	/********************************************* ADDIU **********************************************/
	/**************************************************************************************************/

	//--------------------------------------------- 1 ------------------------------------------------//
	{
		uint32_t got;
		testId = mips_test_begin_test("addiu");
		passed = 0;

		err = mips_cpu_set_register(cpu, 8, 0x000000FFul);


		if (!err)
		{
			uint8_t encoding_bytes[4] = { 0x34, 0x12, 0xA, 0x25 }; // addiu $10, $8, 0x1234 : 0010 0101 0000 1010 0001 0010 0011 0100 (encoding_bytes is big endian form of this)

			// Write encoding into memory at a known address
			err = mips_mem_write(mem, address, 4, encoding_bytes);
		}

		// Make sure the program-counter is at that address
		if (!err)
			err = mips_cpu_set_pc(cpu, address);

		if (!err)
			err = mips_cpu_step(cpu);

		address += 4;

		if (!err)
			err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

		passed = (err == mips_Success) && (got == 0x00001333);

		mips_test_end_test(testId, passed, NULL);
	}
	/**************************************************************************************************/
	/********************************************* ANDI ***********************************************/
	/**************************************************************************************************/

	//--------------------------------------------- 1 ------------------------------------------------//
	{
		uint32_t got;
		testId = mips_test_begin_test("andi");
		passed = 0;

		err = mips_cpu_set_register(cpu, 8, 0x000000FFul);


		if (!err)
		{
			uint8_t encoding_bytes[4] = { 0xF0, 0x00, 0xA, 0x31 }; // andi $10, $8, 0xF0 : 0011 0001 0000 1010 0000 0000 1111 0000 (encoding_bytes is big endian form of this)

			// Write encoding into memory at a known address
			err = mips_mem_write(mem, address, 4, encoding_bytes);
		}

		// Make sure the program-counter is at that address
		if (!err)
			err = mips_cpu_set_pc(cpu, address);

		if (!err)
			err = mips_cpu_step(cpu);

		address += 4;

		if (!err)
			err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

		passed = (err == mips_Success) && (got == 0x000000F0);

		mips_test_end_test(testId, passed, NULL);
	}
	/**************************************************************************************************/
	/********************************************* ADDU ***********************************************/
	/**************************************************************************************************/

	//--------------------------------------------- 1 ------------------------------------------------//
	{
		uint32_t got;
		testId = mips_test_begin_test("addu");
		passed = 0;

		err = mips_cpu_set_register(cpu, 8, 0x000000FFul);
		if (!err)
			err = mips_cpu_set_register(cpu, 9, 0x0000FF00ul);

		if (!err)
		{
			uint8_t encoding_bytes[4] = { 0x21, 0x50, 0x09, 0x01 }; // addu $10, $8, $9 : 0000 0001 0000 1001 0101 0000 0010 0001 (encoding_bytes is big endian form of this)

			// Write encoding into memory at a known address
			err = mips_mem_write(mem, address, 4, encoding_bytes);
		}

		//  Make sure the program-counter is at that address
		if (!err)
			err = mips_cpu_set_pc(cpu, address);

		if (!err)
			err = mips_cpu_step(cpu);

		address += 4;

		if (!err)
			err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

		passed = (err == mips_Success) && (got == 0x0000FFFF);

		mips_test_end_test(testId, passed, NULL);
	}
	/**************************************************************************************************/
	/********************************************* AND ************************************************/
	/**************************************************************************************************/

	//--------------------------------------------- 1 ------------------------------------------------//
	{
		uint32_t got;
		testId = mips_test_begin_test("and");
		passed = 0;

		err = mips_cpu_set_register(cpu, 8, 0x0000FFFFul);
		if (!err)
			err = mips_cpu_set_register(cpu, 9, 0x00FFFF00ul);

		if (!err)
		{
			uint8_t encoding_bytes[4] = { 0x24, 0x50, 0x09, 0x01 }; // and $10, $8, $9 : 0000 0001 0000 1001 0101 0000 0010 0100 (encoding_bytes is big endian form of this)

			// Write encoding into memory at a known address
			err = mips_mem_write(mem, address, 4, encoding_bytes);
		}

		//  Make sure the program-counter is at that address
		if (!err)
			err = mips_cpu_set_pc(cpu, address);

		if (!err)
			err = mips_cpu_step(cpu);

		address += 4;


		if (!err)
			err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

		passed = (err == mips_Success) && (got == 0x0000FF00);

		mips_test_end_test(testId, passed, NULL);
	}
	/**************************************************************************************************/
	/********************************************* LUI ************************************************/
	/**************************************************************************************************/

	//--------------------------------------------- 1 ------------------------------------------------//
	{
		uint32_t got;
		testId = mips_test_begin_test("lui");
		passed = 0;

		if (!err)
		{
			uint8_t encoding_bytes[4] = { 0xF0, 0x0F, 0xA, 0x3C }; // lui $10, 0xFF0 : 0011 1100 0000 1010 0000 1111 1111 0000 (encoding_bytes is big endian form of this)

			// Write encoding into memory at a known address
			err = mips_mem_write(mem, address, 4, encoding_bytes);
		}

		// Make sure the program-counter is at that address
		if (!err)
			err = mips_cpu_set_pc(cpu, address);

		if (!err)
			err = mips_cpu_step(cpu);

		address += 4;

		if (!err)
			err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

		passed = (err == mips_Success) && (got == 0x0FF00000);

		mips_test_end_test(testId, passed, NULL);
	}

	/**************************************************************************************************/
	/********************************************* LBU ************************************************/
	/**************************************************************************************************/

	//--------------------------------------------- 1 ------------------------------------------------//
	{
		uint32_t got;
		testId = mips_test_begin_test("lbu");
		passed = 0;

		uint8_t word[4] = { 0x01, 0x02, 0x03, 0x04 }; // Put 0x04030201 into memory
		err = mips_mem_write(mem, 0x0001004, 4, word);

		if (!err)
			err = mips_cpu_set_register(cpu, 8, 0x00001000ul);


		if (!err)
		{
			uint8_t encoding_bytes[4] = { 0x04, 0x00, 0x09, 0x91 }; // lb $9, 4($8) : 1001 0001 0000 1001 0000 0000 0000 0100 (encoding_bytes is big endian form of this)

			// Write encoding into memory at a known address
			err = mips_mem_write(mem, address, 4, encoding_bytes);
		}

		// Make sure the program-counter is at that address
		if (!err)
			err = mips_cpu_set_pc(cpu, address);

		if (!err)
			err = mips_cpu_step(cpu);

		address += 4;

		if (!err)
			err = (mips_error)(err | mips_cpu_get_register(cpu, 9, &got));

		passed = (err == mips_Success) && (got == 0x00000004);

		mips_test_end_test(testId, passed, NULL);

		/**************************************************************************************************/
		/********************************************* LW *************************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			uint32_t got;
			testId = mips_test_begin_test("lw");
			passed = 0;

			uint8_t word[4] = { 0x01, 0x02, 0x03, 0x04 };
			err = mips_mem_write(mem, 0x0001004, 4, word);

			if (!err)
				err = mips_cpu_set_register(cpu, 8, 0x00001000ul);


			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x04, 0x00, 0x09, 0x8D }; // lw $9, 4($8) : 1000 1101 0000 1001 0000 0000 0000 0100 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			// Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 9, &got));

			passed = (err == mips_Success) && (got == 0x01020304);

			mips_test_end_test(testId, passed, NULL);
		}

		/**************************************************************************************************/
		/********************************************* MFHI **********************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			uint32_t lo;
			uint32_t hi;
			testId = mips_test_begin_test("mfhi");
			passed = 0;

			err = mips_cpu_set_register(cpu, 8, 0xFFFFFFFFul);
			if (!err)
				err = mips_cpu_set_register(cpu, 9, 0x00000002ul);

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x19, 0x00, 0x09, 0x01 }; // multu $8, $9 : 0000 0001 0000 1001 0000 0000 0001 1001 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;


			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x10, 0x60, 0x00, 0x00 }; // mfhi $12 : 0000 0000 0000 0000 0110 0000 0001 0000 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;


			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 12, &hi));

			passed = (err == mips_Success) && (hi == 0x00000001);

			mips_test_end_test(testId, passed, NULL);
		}

		/**************************************************************************************************/
		/********************************************* MFLO ***********************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			uint32_t lo;
			uint32_t hi;
			testId = mips_test_begin_test("mflo");
			passed = 0;

			err = mips_cpu_set_register(cpu, 8, 0xFFFFFFFFul);
			if (!err)
				err = mips_cpu_set_register(cpu, 9, 0x00000002ul);

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x19, 0x00, 0x09, 0x01 }; // multu $8, $9 : 0000 0001 0000 1001 0000 0000 0001 1001 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x12, 0x58, 0x00, 0x00 }; //mflo &11 : 0000 0000 0000 0000 0101 1000 0001 0010 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}
			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 11, &lo));

			passed = (err == mips_Success) && (lo == 0xFFFFFFFE);

			mips_test_end_test(testId, passed, NULL);
		}

		/**************************************************************************************************/
		/********************************************* MULT **********************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			uint32_t lo;
			uint32_t hi;
			testId = mips_test_begin_test("mult");
			passed = 0;

			err = mips_cpu_set_register(cpu, 8, 0xFFFFFFFFul);
			if (!err)
				err = mips_cpu_set_register(cpu, 9, 0x00000002ul);

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x18, 0x00, 0x09, 0x01 }; // multu $8, $9 : 0000 0001 0000 1001 0000 0000 0001 1000 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x12, 0x58, 0x00, 0x00 }; //mflo &11 : 0000 0000 0000 0000 0101 1000 0001 0010 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}
			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x10, 0x60, 0x00, 0x00 }; // mfhi $12 : 0000 0000 0000 0000 0110 0000 0001 0000 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;


			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 11, &lo) | mips_cpu_get_register(cpu, 12, &hi));

			passed = (err == mips_Success) && (lo == 0xFFFFFFFE) && (hi == 0x00000000);

			mips_test_end_test(testId, passed, NULL);
		}

		/**************************************************************************************************/
		/********************************************* MULTU **********************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			uint32_t lo;
			uint32_t hi;
			testId = mips_test_begin_test("multu");
			passed = 0;

			err = mips_cpu_set_register(cpu, 8, 0xFFFFFFFFul);
			if (!err)
				err = mips_cpu_set_register(cpu, 9, 0x00000002ul);

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x19, 0x00, 0x09, 0x01 }; // multu $8, $9 : 0000 0001 0000 1001 0000 0000 0001 1001 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x12, 0x58, 0x00, 0x00 }; //mflo &11 : 0000 0000 0000 0000 0101 1000 0001 0010 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}
			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x10, 0x60, 0x00, 0x00 }; // mfhi $12 : 0000 0000 0000 0000 0110 0000 0001 0000 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;


			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 11, &lo) | mips_cpu_get_register(cpu, 12, &hi));

			passed = (err == mips_Success) && (lo == 0xFFFFFFFE) && (hi == 0x00000001);

			mips_test_end_test(testId, passed, NULL);
		}
		/**************************************************************************************************/
		/********************************************* OR *************************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			uint32_t got;
			testId = mips_test_begin_test("or");
			passed = 0;

			err = mips_cpu_set_register(cpu, 8, 0x0000FFFFul);
			if (!err)
				err = mips_cpu_set_register(cpu, 9, 0x00FFFF00ul);

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x25, 0x50, 0x09, 0x01 }; // or $10, $8, $9 : 0000 0001 0000 1001 0101 0000 0010 0101 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

			passed = (err == mips_Success) && (got == 0x00FFFFFF);

			mips_test_end_test(testId, passed, NULL);
		}
		/**************************************************************************************************/
		/********************************************* ORI ************************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			uint32_t got;
			testId = mips_test_begin_test("ori");
			passed = 0;

			err = mips_cpu_set_register(cpu, 8, 0x000000FFul);


			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0xF0, 0x0F, 0xA, 0x35 }; // ori $10, $8, 0xFF0 : 0011 0101 0000 1010 0000 1111 1111 0000 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			// Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

			passed = (err == mips_Success) && (got == 0x00000FFF);

			mips_test_end_test(testId, passed, NULL);
		}
		/**************************************************************************************************/
		/********************************************* SB *************************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			
			testId = mips_test_begin_test("sb");
			passed = 0;

			err = mips_cpu_set_register(cpu, 8, 0x00001000ul);
			if (!err)
				err = mips_cpu_set_register(cpu, 9, 0x00000012ul);

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x04, 0x00, 0x09, 0xA5 }; // sb $9, 4($8)  : 1010 0001 0000 1001 0000 0000 0000 0100 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			// Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_mem_read(mem, 0x1004, 4, word_bytes));

			passed = (err == mips_Success) && (word_bytes[3] == 0x12);

			mips_test_end_test(testId, passed, NULL);
		}
		/**************************************************************************************************/
		/********************************************* SH *************************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			
			testId = mips_test_begin_test("sh");
			passed = 0;

			err = mips_cpu_set_register(cpu, 8, 0x00001000ul);
			if (!err)
				err = mips_cpu_set_register(cpu, 9, 0x00001234ul);

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x04, 0x00, 0x09, 0xA5 }; // sh $9, 4($8)  : 1010 0101 0000 1001 0000 0000 0000 0100 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			// Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_mem_read(mem, 0x1004, 4, word_bytes));

			passed = (err == mips_Success) && (word_bytes[3] == 0x34) && (word_bytes[2] == 0x12);

			mips_test_end_test(testId, passed, NULL);
		}
		/**************************************************************************************************/
		/******************************************** SLL *************************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			uint32_t got;
			testId = mips_test_begin_test("sll");
			passed = 0;

			err = mips_cpu_set_register(cpu, 9, 0x00FFFF00ul);


			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x00, 0x51, 0x09, 0x00 }; // sll $10, $9, 0x4 : 0000 0000 0000 1001 0101 0001 0000 0000 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

			passed = (err == mips_Success) && (got == 0x0FFFF000);

			mips_test_end_test(testId, passed, NULL);
		}
		/**************************************************************************************************/
		/******************************************** SLLV *************************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			uint32_t got;
			testId = mips_test_begin_test("sllv");
			passed = 0;

			err = mips_cpu_set_register(cpu, 8, 0x00000004ul);
			if (!err)
				err = mips_cpu_set_register(cpu, 9, 0x00FFFF00ul);


			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x04, 0x50, 0x09, 0x01 }; // sllv $10, $9, $8 : 0000 0001 0000 1001 0101 0000 0000 0100 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

			passed = (err == mips_Success) && (got == 0x0FFFF000);

			mips_test_end_test(testId, passed, NULL);
		}
		/**************************************************************************************************/
		/********************************************* SLT ************************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			uint32_t got;
			testId = mips_test_begin_test("slt");
			passed = 0;

			err = mips_cpu_set_register(cpu, 8, 0x0000000Ful);
			if (!err)
				err = mips_cpu_set_register(cpu, 9, 0x000000F0ul);

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x2A, 0x50, 0x09, 0x01 }; // slt $10, $8, $9 : 0000 0001 0000 1001 0101 0000 0010 1010 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

			passed = (err == mips_Success) && (got == 0x00000001);

			mips_test_end_test(testId, passed, NULL);
		}
		/**************************************************************************************************/
		/********************************************* SLTI ***********************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//

		{
			uint32_t got;
			testId = mips_test_begin_test("slti");
			passed = 0;


			err = mips_cpu_set_register(cpu, 8, 0x0000000Dul);


			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x0E, 0x00, 0x09, 0x29 }; // slti $9, $8, 0xE : 0010 1001 0000 1001 0000 0000 0000 1110 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 9, &got));

			passed = (err == mips_Success) && (got == 0x00000001);

			mips_test_end_test(testId, passed, NULL);
		}

		//--------------------------------------------- 2 ------------------------------------------------//

		{
			uint32_t got;
			testId = mips_test_begin_test("slti");
			passed = 0;


			err = mips_cpu_set_register(cpu, 8, 0x0000000Ful);


			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x0E, 0x00, 0x09, 0x29 }; // slti $9, $8, 0xE : 0010 1001 0000 1001 0000 0000 0000 1110 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 9, &got));

			passed = (err == mips_Success) && (got == 0);

			mips_test_end_test(testId, passed, NULL);
		}

		/**************************************************************************************************/
		/********************************************* SLTIU **********************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//

		{
			uint32_t got;
			testId = mips_test_begin_test("sltiu");
			passed = 0;


			err = mips_cpu_set_register(cpu, 8, 0x0000000Dul);


			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x0E, 0x00, 0x09, 0x29 }; // slti $9, $8, 0xE : 0010 1001 0000 1001 0000 0000 0000 1110 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 9, &got));

			passed = (err == mips_Success) && (got == 0x00000001);

			mips_test_end_test(testId, passed, NULL);
		}

		//--------------------------------------------- 2 ------------------------------------------------//

		{
			uint32_t got;
			testId = mips_test_begin_test("sltiu");
			passed = 0;


			err = mips_cpu_set_register(cpu, 8, 0x0000000Ful);


			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x0E, 0x00, 0x09, 0x29 }; // slti $9, $8, 0xE : 0010 1001 0000 1001 0000 0000 0000 1110 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 9, &got));

			passed = (err == mips_Success) && (got == 0);

			mips_test_end_test(testId, passed, NULL);
		}
		/**************************************************************************************************/
		/********************************************* SLTU ************************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			uint32_t got;
			testId = mips_test_begin_test("sltu");
			passed = 0;

			err = mips_cpu_set_register(cpu, 8, 0x0000000Ful);
			if (!err)
				err = mips_cpu_set_register(cpu, 9, 0x000000F0ul);

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x2B, 0x50, 0x09, 0x01 }; // sltu $10, $8, $9 : 0000 0001 0000 1001 0101 0000 0010 1011 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

			passed = (err == mips_Success) && (got == 0x00000001);

			mips_test_end_test(testId, passed, NULL);
		}
		/**************************************************************************************************/
		/******************************************** SRA *************************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			uint32_t got;
			testId = mips_test_begin_test("sra");
			passed = 0;

			err = mips_cpu_set_register(cpu, 9, 0x00FFFF00ul);


			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x03, 0x51, 0x09, 0x00 }; // sra $10, $9, 0x4 : 0000 0000 0000 1001 0101 0001 0000 0011 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

			passed = (err == mips_Success) && (got == 0x000FFFF0);

			mips_test_end_test(testId, passed, NULL);
		}
		//--------------------------------------------- 2 ------------------------------------------------//
		{
			uint32_t got;
			testId = mips_test_begin_test("sra");
			passed = 0;

			err = mips_cpu_set_register(cpu, 9, 0x80000000ul);

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0xC3, 0x50, 0x09, 0x00 }; // sra $10, $9, 0x3 : 0000 0000 0000 1001 0101 0000 1100 0011 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

			passed = (err == mips_Success) && (got == 0xF0000000);

			mips_test_end_test(testId, passed, NULL);
		}
		/**************************************************************************************************/
		/******************************************** SRL *************************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			uint32_t got;
			testId = mips_test_begin_test("srl");
			passed = 0;

			err = mips_cpu_set_register(cpu, 9, 0x00FFFF00ul);


			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x02, 0x51, 0x09, 0x00 }; // srl $10, $9, 0x4 : 0000 0000 0000 1001 0101 0001 0000 0010 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

			passed = (err == mips_Success) && (got == 0x000FFFF0);

			mips_test_end_test(testId, passed, NULL);
		}
		/**************************************************************************************************/
		/******************************************** SRLV ************************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			uint32_t got;
			testId = mips_test_begin_test("srlv");
			passed = 0;

			err = mips_cpu_set_register(cpu, 9, 0x00FFFF00ul);
			if (!err)
				err = mips_cpu_set_register(cpu, 8, 0x00000004ul);

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x06, 0x50, 0x09, 0x01 }; // srlv $10, $9, $8 : 0000 0001 0000 1001 0101 0000 0000 0110 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

			passed = (err == mips_Success) && (got == 0x000FFFF0);

			mips_test_end_test(testId, passed, NULL);
		}
		/**************************************************************************************************/
		/******************************************** SUBU ************************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			uint32_t got;
			testId = mips_test_begin_test("subu");
			passed = 0;

			err = mips_cpu_set_register(cpu, 8, 0x00FFFF00ul);
			if (!err)
				err = mips_cpu_set_register(cpu, 9, 0x0000FF00ul);

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x23, 0x50, 0x09, 0x01 }; // subu $10, $8, $9 : 0000 0001 0000 1001 0101 0000 0010 0011 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			//  Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

			passed = (err == mips_Success) && (got == 0x00FF0000);

			mips_test_end_test(testId, passed, NULL);
		}
		/**************************************************************************************************/
		/******************************************** XOR *************************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			uint32_t got;
			testId = mips_test_begin_test("xor");
			passed = 0;

			err = mips_cpu_set_register(cpu, 8, 0x00FFFFFFul);
			if (!err)
				err = mips_cpu_set_register(cpu, 9, 0x0000FF00ul);

			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0x26, 0x50, 0x09, 0x01 }; // xor $10, $8, $9 : 0000 0001 0000 1001 0101 0000 0010 0110 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

			passed = (err == mips_Success) && (got == 0x00FF00FF);

			mips_test_end_test(testId, passed, NULL);
		}
		/**************************************************************************************************/
		/********************************************* XORI ***********************************************/
		/**************************************************************************************************/

		//--------------------------------------------- 1 ------------------------------------------------//
		{
			uint32_t got;
			testId = mips_test_begin_test("xori");
			passed = 0;

			err = mips_cpu_set_register(cpu, 8, 0x000000FFul);


			if (!err)
			{
				uint8_t encoding_bytes[4] = { 0xF0, 0x0F, 0xA, 0x39 }; // xori $10, $8, 0xFF0 : 0011 1001 0000 1010 0000 1111 1111 0000 (encoding_bytes is big endian form of this)

				// Write encoding into memory at a known address
				err = mips_mem_write(mem, address, 4, encoding_bytes);
			}

			// Make sure the program-counter is at that address
			if (!err)
				err = mips_cpu_set_pc(cpu, address);

			if (!err)
				err = mips_cpu_step(cpu);

			address += 4;

			if (!err)
				err = (mips_error)(err | mips_cpu_get_register(cpu, 10, &got));

			passed = (err == mips_Success) && (got == 0x00000F0F);

			mips_test_end_test(testId, passed, NULL);

			mips_test_end_suite();
			mips_cpu_free(cpu);
			cpu = 0;
			mips_mem_free(mem);
			mem = 0;

			return 0;
		}
	}
}

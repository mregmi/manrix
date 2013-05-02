/*
 * Misc I/O manager for ManRiX OS
 * Copyright (C) 2004, 2005	Manish Regmi (regmi dot manish at gmail dot com)
 *				            Rajesh R.C (rajee5283 at hotmail dot com)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
/*
 * File: x86.c
 * 		X86 processor services and identification.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "x86.h"

/* Cpu info */
static struct cpu_info *cinfo;

/*
 * Check if cpuid instruction is available.
 * Just ckeck if we can overwrite EFLAG_ID bit in eflags.
 */
static inline int check_cpuid()
{
	int ret = 0;

	asm volatile(
		"pushfl \n\t"
		"popl %%eax \n\t"
		"movl %%eax, %%edx \n\t"
		"xorl $0x200000, %%edx \n\t"
		"pushl %%edx \n\t"
		"popfl \n\t"
		"pushfl \n\t"
		"popl %%eax \n\t"
		"xor %%eax, %%edx \n\t"
		"jnz 1 \n\t"
		"movl $1, %0 \n\t"
		"1:"
		:"=r"(ret)
		:
		:"eax", "edx"
		);
	
	return ret;
}

/* Get the vendor string */
static inline void get_vendor(char *string)
{
	struct cpu_regs regs;

	cpuid(0, &regs);

	string[0] = regs.ebx & 0xFF;
	string[1] = (regs.ebx >> 8) & 0xFF;
	string[2] = (regs.ebx >> 16) & 0xFF;
	string[3] = (regs.ebx >> 24) & 0xFF;
	string[4] = (regs.edx & 0xFF);
	string[5] = (regs.edx >> 8) & 0xFF;
	string[6] = (regs.edx >> 16) & 0xFF;
	string[7] = (regs.edx >> 24) & 0xFF;
	string[8] = (regs.ecx & 0xFF);
	string[9] = (regs.ecx >> 8) & 0xFF;
	string[10] = (regs.ecx >> 16) & 0xFF;
	string[11] = (regs.ecx >> 24) & 0xFF;
	string[12] = '\0';	/* terminate with NULL */

//	printf("%s", string);
}

/* Get the cpu signature and Misc info */
static inline void get_signature(struct cpu_info *info)
{
	struct cpu_regs regs;

	cpuid(1, &regs);

	info->signature = regs.eax;
	info->serial[2] = regs.eax;
	info->misc_info = regs.ebx;
	info->features_low = regs.edx;
	info->features_high = regs.ecx;
}

/* Get the Processor Serial Number */
static inline void get_processor_serial(struct cpu_info *info)
{
	struct cpu_regs regs;

	cpuid(3, &regs);

	info->serial[0] = regs.ecx;
	info->serial[1] = regs.edx;
	info->serial[3] = 0;
}

/*
 * Get the Brand Info From the String.
 */
static inline void get_brand_from_sig(unsigned int signature, char *brand)
{
	int family, model, stepping;
	char *str;

	family = (signature >> 8) & 0xF;
	model = (signature >> 4) & 0xF;
	stepping = signature & 0xF;

	switch(family)
	{
		case 4:	/* Intel 486 processor */
			switch(model)
			{
				case 0:
				case 1:
					str = "Intel486 DX Processor";
					break;
				case 2:
					str = "Intel486 SX Processor";
					break;
				case 3:
					str = "IntelDX2 Processor";
					break;
				case 4:
					str = "Intel486 SL Processor";
					break;
				case 5:
					str = "IntelSX2 Processor";
					break;
				case 7:
					str = "IntelDX2 Processor (WriteBack Enhanced)";
					break;
				case 8:
					str = "IntelDX4 Processor";
					break;
				default:
					str = "Unknown Intel486 Processor";
			}
			break;

		case 5:	/* Intel Pentium Processor */
			switch(model)
			{
				case 1:
					str = "Intel Pentium Processor (60, 66) MHZ";
					break;
				case 2:
					str = "Intel Pentium Processor(75, 90, 100, 120, 133, 150, 166, 200) MHZ";
					break;
				case 3:
					str = "Intel Pentium Overdrive Processor for Intel486";
					break;
				case 4:
					str = "Intel Pentium Processor with MMX Technology";
					break;
				case 5:
					str = "Intel Pentium Processor with MMX Technology (166, 200) MHZ";
					break;
				default:
					str = "Unknown Intel Pentium Processor";
			}
			break;

		case 6:	/* Intel Pentium Pro, II, III and Celeron Processors */
			switch(model)
			{
				case 1:
					str = "Intel Pentium Pro Processor";
					break;
				case 3:
					str = "Intel Pentium II Processor";
					break;
				case 5:
					/*
					 * TODO: Check the caches to differentiate between them.
					 * Check the cache descriptor values through executing CPUID instruction
					 * with EAX = 2. If no L2 cache is returned, the processor is identified
					 * as an Intel Celeron processor, model 5. If 1M or 2M L2 cache size is
					 * reported, the processor is the Pentium II Xeon processor otherwise
					 * it is a Pentium II processor, model 5 or a Pentium II Xeon processor
					 * with 512K L2 cache size.
					 */
					 if(!cinfo->cache_L2)
					 	str = "Intel Celeron Processor";
					// else if(cinfo->cache_L2 >= )
					str = "Intel Pentium II, XEON or Celeron Processor";
					break;
				case 6:
					str = "Intel Celeron Processor";
					break;
				case 7:
					/*
					 * TODO: differentiate between the Pentium III processor, model 7 and
					 * the Pentium III Xeon processor, model 7, software should check the
					 * cache descriptor values through executing CPUID instruction with EAX = 2.
					 * If 1M or 2M L2 cache size is reported, the processor is the
					 * Pentium III Xeon processor otherwise it is a Pentium III processor or
					 * a Pentium III Xeon processor with 512K L2 cache size.
					 */
					str = "Intel Pentium III or XEON Processor";
					break;
				case 8:
					/*
					 * TODO: Differentiate between the Pentium III processor, model 8 and
					 * the Pentium III Xeon processor, model 8, software should check
					 * the Brand ID values through executing CPUID instruction with EAX = 1.
					 */
					str = "Intel Pentium III, XEON or Celeron Processor";
					break;
				case 9:
					str = "Intel Pentium M Processor";
					break;
				case 10:
					str = "Intel Pentium III XEON Processor";
					break;
				case 11:
					str = "Intel Pentium III Processor";
					break;
				default:
					str = "Unknown Intel Pentium Pro, II, III or Celeron Processor";

			}
			break;

		case 0xF:	/* Intel Pentium 4 Processors */
			switch(model)
			{
				case 0:
					str = "Intel Pentium 4 or XEON Processor (0.18 micron)";
					break;
				case 1:
					str = "Intel Pentium 4, XEON, XEON MP or Celeron Processor (0.18 micron)";
					break;
				case 2:
					str = "Intel Pentium 4, 4 Mobile, XEON MP, \
							Celeron or Celeron Mobile Processor (0.18 micron)";
					break;
				default:
					str = "Unknown Intel Pentium 4 Processor";
			}
			break;
		default:
			str = "Unknown X86 Processor";
	}

	strcpy(brand, str);
}


/* Get the Brand String of the CPU */
static inline void get_brand_string(struct cpu_info *info)
{
	struct cpu_regs regs;
	unsigned long *ptr = (unsigned long *)&info->brand_str;

	/*
	 * Check if the Procssor Support Brands.
	 * If the processor does not support brand string,
	 * Use the signature to find the Brand String.
	 */
	cpuid(0x80000000, &regs);
	if(regs.eax <= 0x80000000)
	{
		get_brand_from_sig(info->signature, info->brand_str);
		goto end;
	}

	cpuid(0x80000002, &regs);
	ptr[0] = regs.eax;
	ptr[1] = regs.ebx;
	ptr[2] = regs.ecx;
	ptr[3] = regs.edx;

	cpuid(0x80000003, &regs);
	ptr[4] = regs.eax;
	ptr[5] = regs.ebx;
	ptr[6] = regs.ecx;
	ptr[7] = regs.edx;

	cpuid(0x80000004, &regs);
	ptr[8] = regs.eax;
	ptr[9] = regs.ebx;
	ptr[10] = regs.ecx;
	ptr[11] = regs.edx;

end:
	printf("%s ", info->vendor_id);
	printf("%s\n", info->brand_str);
}

/* Get the information about TLB */
static inline void get_cach_info(struct cpu_info *info, unsigned char cachinfo[])
{
	int index;
	for(index = 0; cachinfo[index] != 0 ;index++)
	{
		switch(cachinfo[index])
		{
			/* Instruction TLB */
			case 1:
				info->tlb_code = cachinfo[index];
				info->tlb_code_str = "4K-byte Pages, 4-way set associative, 32 entries";
				break;

			case 2:
				info->tlb_code = cachinfo[index];
				info->tlb_code_str = "4M-byte Pages, fully associative, 2 entries";
				break;

			case 0x50:
				info->tlb_code = cachinfo[index];
				info->tlb_code_str = "4K, 2M or 4M pages, fully associative, 64 entries";
				break;

			case 0x51:
				info->tlb_code = cachinfo[index];
				info->tlb_code_str = "4K, 2M or 4M pages, fully associative, 128 entries";
				break;

			case 0x52:
				info->tlb_code = cachinfo[index];
				info->tlb_code_str = "4K, 2M or 4M pages, fully associative, 256 entries";
				break;

			case 0xB0:
				info->tlb_code = cachinfo[index];
				info->tlb_code_str = "4K-byte Pages, 4-way set associative, 128 entries";
				break;

			/* Data TLB */
			case 3:
				info->tlb_data = cachinfo[index];
				info->tlb_data_str = "4K-byte Pages, 4-way set associative, 64 entries";
				break;

			case 4:
				info->tlb_data = cachinfo[index];
				info->tlb_data_str = "4M-byte Pages, 4-way set associative, 8 entries";
				break;

			case 0x5B:
				info->tlb_data = cachinfo[index];
				info->tlb_data_str = "4K or 4M pages, fully associative, 64 entries";
				break;

			case 0x5C:
				info->tlb_data = cachinfo[index];
				info->tlb_data_str = "4K or 4M pages, fully associative, 128 entries";
				break;

			case 0x5D:
				info->tlb_data = cachinfo[index];
				info->tlb_data_str = "4K or 4M pages, fully associative, 256 entries";
				break;

			case 0xB3:
				info->tlb_data = cachinfo[index];
				info->tlb_data_str = "4K-byte Pages, 4-way set associative, 128 entries";
				break;

			/* Trace Cache */
			case 0x70:
				info->cache_trace = cachinfo[index];
				info->trace_str = "12K-uops, 8-way set associative";
				break;

			case 0x71:
				info->cache_trace = cachinfo[index];
				info->trace_str = "16K-uops, 8-way set associative";
				break;

			case 0x72:
				info->cache_trace = cachinfo[index];
				info->trace_str = "32K-uops, 8-way set associative";
				break;

			/* L1 code cache */
			case 6:
				info->cache_L1_code = cachinfo[index];
				info->L1_code_str = "8K-bytes, 4-way set associative, 32 byte line size";
				break;

			case 8:
				info->cache_L1_code = cachinfo[index];
				info->L1_code_str = "16K-bytes, 4-way set associative, 32 byte line size";
				break;

			case 0x30:
				info->cache_L1_code = cachinfo[index];
				info->L1_code_str = "32K-bytes, 8-way set associative, 64-byte line size";
				break;

			/* L1 data cache */
			case 0xA:
				info->cache_L1_data = cachinfo[index];
				info->L1_data_str = "8K-bytes, 2-way set associative, 32 byte line size";
				break;

			case 0xC:
				info->cache_L1_data = cachinfo[index];
				info->L1_data_str = "16K-bytes, 4-way set associative, 32 byte line size";
				break;

			case 0x2C:
				info->cache_L1_data = cachinfo[index];
				info->L1_data_str = "32K-bytes, 8-way set associative, 64-byte line size";
				break;

			case 0x66:
				info->cache_L1_data = cachinfo[index];
				info->L1_data_str = "8K-bytes, 4-way set associative, sectored cache, 64-byte line size";
				break;

			case 0x67:
				info->cache_L1_data = cachinfo[index];
				info->L1_data_str = "16K-bytes, 4-way set associative, sectored cache, 64-byte line size";
				break;

			case 0x68:
				info->cache_L1_data = cachinfo[index];
				info->L1_data_str = "32K-bytes, 4 way set associative, sectored cache, 64-byte line size";
				break;

			/* L2 cache */
			case 0x39:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "128K-bytes, 4-way set associative, sectored cache, 64-byte line size";
				break;

			case 0x3B:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "128KB, 2-way set associative, sectored cache, 64-byte line size";
				break;

			case 0x3C:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "256K-bytes, 4-way set associative, sectored cache, 64-byte line size";
				break;

			case 0x41:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "128K-bytes, 4-way set associative, 32 byte line size";
				break;

			case 0x42:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "256K-bytes, 4-way set associative, 32 byte line size";
				break;

			case 0x43:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "512K-bytes, 4-way set associative, 32 byte line size";
				break;

			case 0x44:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "1M-bytes, 4-way set associative, 32 byte line size";
				break;

			case 0x45:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "2M-bytes, 4-way set associative, 32 byte line size";
				break;

			case 0x79:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "128K-bytes, 8-way set associative, sectored cache, 64-byte line size";
				break;

			case 0x7A:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "256K-bytes, 8-way set associative, sectored cache, 64-byte line size";
				break;

			case 0x7B:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "512K-bytes, 8-way set associative, sectored cache, 64-byte line size";
				break;

			case 0x7C:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "1M-bytes, 8-way set associative, sectored cache, 64-byte line size";
				break;

			case 0x82:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "256K-bytes, 8-way set associative, 32 byte line size";
				break;

			case 0x83:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "512K-bytes, 8-way set associative, 32 byte line size";
				break;

			case 0x84:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "1M-bytes, 8-way set associative, 32 byte line size";
				break;

			case 0x85:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "2M-bytes, 8-way set associative, 32 byte line size";
				break;

			case 0x86:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "512K-bytes, 4-way set associative, 64 byte line size";
				break;

			case 0x87:
				info->cache_L2 = cachinfo[index];
				info->L2_str = "1M-bytes, 8-way set associative, 64 byte line size";
				break;

			/* L3 cache */
			case 0x22:
				info->cache_L3 = cachinfo[index];
				info->L3_str = "512K-bytes, 4-way set associative, sectored cache, 64-byte line size";
				break;

			case 0x23:
				info->cache_L3 = cachinfo[index];
				info->L3_str = "1M-bytes, 8-way set associative, sectored cache, 64-byte line size";
				break;

			case 0x25:
				info->cache_L3 = cachinfo[index];
				info->L3_str = "2MB, 8-way set associative, sectored cache, 64-byte line size";
				break;

			case 0x29:
				info->cache_L3 = cachinfo[index];
				info->L3_str = "4MB, 8-way set associative, sectored cache, 64-byte line size";
				break;
		}
	}
}

/* Get the Cache and TLB info */
static inline void get_cache_info(struct cpu_info *info)
{
	struct cpu_regs regs;
	int ntimes;
	unsigned char cache_info[16];
	unsigned char val;

	cpuid(2, &regs);

	ntimes = regs.eax & 0xFF;

	if(!ntimes)
		ntimes = 1; /* ???? */
	
	while(--ntimes)
		cpuid(2, &regs);

	ntimes = 0;
	/*
	 * If the 31'st bit is zero, the register contains the
	 * valid cache info.
	 */
	if((regs.eax & 0x80000000) == 0)
	{
		val = (regs.eax >> 24) & 0xFF;
		if(val)
			cache_info[ntimes++] = val;

		val = (regs.eax >> 16) & 0xFF;
		if(val)
			cache_info[ntimes++] = val;

		val = (regs.eax >> 8) & 0xFF;
		if(val)
			cache_info[ntimes++] = val;
	}

	if((regs.ebx & 0x80000000) == 0)
	{
		val = (regs.ebx >> 24) & 0xFF;
		if(val)
			cache_info[ntimes++] = val;

		val = (regs.ebx >> 16) & 0xFF;
		if(val)
			cache_info[ntimes++] = val;

		val = (regs.ebx >> 8) & 0xFF;
		if(val)
			cache_info[ntimes++] = val;

		val = (regs.ebx) & 0xFF;
		if(val)
			cache_info[ntimes++] = val;
	}

	if((regs.ecx & 0x80000000) == 0)
	{
		val = (regs.ecx >> 24) & 0xFF;
		if(val)
			cache_info[ntimes++] = val;

		val = (regs.ecx >> 16) & 0xFF;
		if(val)
			cache_info[ntimes++] = val;

		val = (regs.ecx >> 8) & 0xFF;
		if(val)
			cache_info[ntimes++] = val;

		val = (regs.ecx) & 0xFF;
		if(val)
			cache_info[ntimes++] = val;
	}

	if((regs.edx & 0x80000000) == 0)
	{
		val = (regs.edx >> 24) & 0xFF;
		if(val)
			cache_info[ntimes++] = val;

		val = (regs.edx >> 16) & 0xFF;
		if(val)
			cache_info[ntimes++] = val;

		val = (regs.edx >> 8) & 0xFF;
		if(val)
			cache_info[ntimes++] = val;

		val = (regs.edx) & 0xFF;
		if(val)
			cache_info[ntimes++] = val;
	}

	cache_info[ntimes] = 0;

	get_cach_info(info, cache_info);
	
	if(!info->L1_code_str)
		info->L1_code_str = info->L1_data_str;
	else if(!info->L1_data_str)
		info->L1_data_str = info->L1_code_str;

	printf("TLB:Code:%s\n    Data:%s\n", info->tlb_code_str, info->tlb_data_str);
	printf("L1 Cache:Code:%s\n      Data:%s\n", info->L1_code_str, info->L1_data_str);
	printf("L2 Cache: %s\n", info->L2_str);

	if(!info->cache_L3)
		printf("L3 Cache: (Not Available)\n");
	else
		printf("L3 Cache: %s\n", info->L3_str);
}

/* Get the info on CPU */
void get_cpu_info(struct cpu_info *info)
{
	get_vendor(info->vendor_id);
	get_signature(info);
	get_processor_serial(info);
	get_cache_info(info);
	get_brand_string(info);
}

/*
 * Initialize...
 */
int x86_init()
{
	if(!check_cpuid())
	{
		printf("CPUID instruction Not Supported. \n");
		return 0;
	}

	/* FIXME: What about Multiprocessor? */
	cinfo = (struct cpu_info *) malloc(sizeof(struct cpu_info));
	memset(cinfo, 0, sizeof(struct cpu_info));

	get_cpu_info(cinfo);

	return 0;
}

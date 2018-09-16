// Task State Segment Handling

#include <string.h>
#include "gdt.h"
#include "tss.h"

static tss_entry TSS;

void TSS_Flush() 
{
	asm volatile ("movl $0x2b, %eax\n\t"
				  "ltr %ax\n\t"
				  );
}

void TSS_SetStack(uint16_t kernelSS, uint32_t kernelESP) 
{
	TSS.ss0 = kernelSS;
	TSS.esp0 = kernelESP;
}

void TSS_Install(uint32_t idx, uint16_t kernelSS, uint32_t kernelESP) 
{
	//! install TSS descriptor
	uint32_t base = (uint32_t)&TSS;

	// Install descriptor
	I86_GDT_SetDescriptor(idx, base, base + sizeof(tss_entry),
						  I86_GDT_DESC_ACCESS|I86_GDT_DESC_EXEC_CODE|I86_GDT_DESC_DPL|I86_GDT_DESC_MEMORY,
						  0);

	// Initialize TSS
	memset((void*) &TSS, 0, sizeof (tss_entry));

	// Set stack and segments
	TSS.ss0  = kernelSS;
	TSS.esp0 = kernelESP;
	TSS.cs   = 0x0b;
	TSS.ss   = 0x13;
	TSS.es   = 0x13;
	TSS.ds   = 0x13;
	TSS.fs   = 0x13;
	TSS.gs   = 0x13;
}


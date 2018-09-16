#ifndef _TSS_H_INCLUDED
# define _TSS_H_INCLUDED

#include <stdint.h>

typedef struct _tss_entry 
{
	uint32_t prevTss;
	uint32_t esp0;
	uint32_t ss0;
	uint32_t esp1;
	uint32_t ss1;
	uint32_t esp2;
	uint32_t ss2;
	uint32_t cr3;
	uint32_t eip;
	uint32_t eflags;
	uint32_t eax;
	uint32_t ecx;
	uint32_t edx;
	uint32_t ebx;
	uint32_t esp;
	uint32_t ebp;
	uint32_t esi;
	uint32_t edi;
	uint32_t es;
	uint32_t cs;
	uint32_t ss;
	uint32_t ds;
	uint32_t fs;
	uint32_t gs;
	uint32_t ldt;
	uint16_t trap;
	uint16_t iomap;
} __attribute__((packed)) tss_entry;

void TSS_Flush();
void TSS_SetStack(uint16_t kernelSS, uint32_t kernelESP);
void TSS_Install(uint32_t sel, uint16_t kernelSS, uint32_t kernelESP);

#endif

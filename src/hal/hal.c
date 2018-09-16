//    Hal.c
// 	  Hardware Abstraction Layer for i86 architecture
//
//	The Hardware Abstraction Layer (HAL) provides an abstract interface
//	to control the basic motherboard hardware devices. This is accomplished
//	by abstracting hardware dependencies behind this interface.
//

#include <Hal.h>
#include "cpu.h"
#include "idt.h"
#include "pic.h"
#include "pit.h"
#include <exception.h>
#include <console.h>
#include "tss.h"

static bool _halInitialised = false;

// Initialize hardware devices
int  HAL_Initialise () 
{
	// Initialize motherboard controllers and system timer
	I86_CPU_Initialise();
	I86_PIC_Initialise(0x20,0x28);
	I86_PIT_Initialise();
	I86_PIT_StartCounter(100,I86_PIT_OCW_COUNTER_0, I86_PIT_OCW_MODE_SQUAREWAVEGEN);
	HAL_InitialiseInterrupts();
	_halInitialised = true;
	return 0;
}

// Shutdown hardware devices
int  HAL_Shutdown () 
{
	_halInitialised = false;
	I86_CPU_Shutdown();
	return 0;
}

bool HAL_IsInitialised()
{
	return _halInitialised;
}

// Generate interrupt call

void HAL_GenerateInterrupt (int n) 
{
	// This generates the appropriate interrupt by modifying the byte
	// after the 'int' instruction
	asm("movb (%0), %%al\n\t " 
		"movb  %%al, (genint + 1)\n\t" 
		"jmp  genint \n"
	"genint:\n\t"
		"int $0"
	: : "r" (n));
}

// Notify hal thatinterrupt is done
inline void HAL_InterruptDone(unsigned int intno) 
{
	// Ensure it's a valid hardware irq
	if (intno > 16)
	{
		return;
	}
	//! test if we need to send end-of-interrupt to second pic
	if (intno >= 8)
	{
		I86_PIC_SendCommand(I86_PIC_OCW2_MASK_EOI, 1);
	}
	// Always send end-of-interrupt to primary pic
	I86_PIC_SendCommand(I86_PIC_OCW2_MASK_EOI, 0);
}

// Read byte from device using port mapped io
uint8_t HAL_InputByteFromPort(uint16_t portid) 
{
	uint8_t result = 0;
	
	asm volatile ("inb %1, %0" : "=a"(result) : "Nd"(portid));
	return result;
}

// Write byte to device through port mapped io
void  HAL_OutputByteToPort(uint16_t portid, uint8_t value) 
{
	asm volatile ("outb %0, %1"
				  :
				  : "a"(value), "Nd"(portid));

}

//! Enable all hardware interrupts
void HAL_EnableInterrupts() 
{
	asm("sti");
}


//! Disable all hardware interrupts
void  HAL_DisableInterrupts() 
{
	asm("cli");
}


// Sets new interrupt vector
void  HAL_SetInterruptVector(int intno, void (*vect)(), int flags ) 
{
	// Install interrupt handler! This overwrites prev interrupt descriptor
	I86_IDT_InstallInterruptHandler(intno, 
									I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32 | flags,
									0x8, 
									vect);
}

// Return current interrupt vector
void (*HAL_GetInterruptVector(int intno))() 
{
	// Get the descriptor from the idt
	IDT_Descriptor* desc = I86_IDT_GetInterruptHandler (intno);
	if (!desc)
	{
		return 0;
	}
	// Get address of interrupt handler
	uint32_t addr = desc->baseLo | (desc->baseHi << 16);

	// Return interrupt handler
	I86_IRQ_HANDLER irq = (I86_IRQ_HANDLER)addr;
	return irq;
}

// Returns cpu vender
const char*  HAL_GetCPUVendor() 
{
	return I86_CPU_GetVendor();
}

// Return current tick count 
uint32_t HAL_GetTickCount() 
{
	return I86_PIT_HAL_GetTickCount();
}

// Sleep for specified number of clock ticks.
// This uses the HALs HAL_GetTickCount() which in turn uses the PIT

void HAL_Sleep(uint32_t tickCount) 
{
	uint32_t ticks;

	ticks = tickCount + HAL_GetTickCount();
	while (ticks > HAL_GetTickCount());
}

void HAL_EnablePaging() 
{
	asm volatile("movl %cr0, %eax \n\t"
				 "orl  $0x80000000, %eax\n\t"
				 "movl %eax, %cr0");
}

void HAL_DisablePaging()
{
	asm volatile("movl %cr0, %eax \n\t"
				 "and  $0x7FFFFFFF, %eax\n\t"
				 "movl %eax, %cr0");
	
}

bool HAL_IsPaging() 
{
	uint32_t res=0;

	asm volatile("movl %%cr0, %0" : "=r"(res));
	return (res & 0x80000000) ? false : true;
}

void HAL_LoadPageDirectoryBaseRegister(uint32_t addr) 
{
	asm("movl %0, %%cr3" : : "r"(addr));
}

uint32_t HAL_GetPageDirectoryBaseRegister() 
{
	uint32_t addr = 0;

	asm volatile("movl %%cr3, %0" : "=r"(addr));
	return addr;
}

void HAL_EnterUserMode() 
{
	asm volatile ("cli\n\t"
				  "movw $0x23, %ax\n\t"    // Setup data selectors to point to user-mode selector
				  "mov  %ax, %ds\n\t"
				  "mov  %ax, %es\n\t"
				  "mov  %ax, %fs\n\t"
				  "mov  %ax, %gs\n\t"
				  "mov  %esp, %eax\n\t"   // Save ESP before we start to pudh the values for iret
				  "pushl $0x23\n\t"		  // Stack segment is same as data segment
				  "pushl %eax\n\t"
				  "pushf\n\t"
				  "popl  %eax\n\t"
				  "orl  $0x200, %eax\n\t"  // Enable IF in flags (since we cannot do STI in user mode).
				  "pushl %eax\n\t"
				  "pushl $0x1b\n\t"		  // User code segment`
				  "pushl $1f\n\t"		  // Push address of label 1.
				  "iret\n\t"
				  "1:\n\t"
				  );
}

void HAL_TSSInitialise()
{
	TSS_Install(5,0x10,0x80000);
}

void HAL_SetTSSStack(uint16_t kernelSS, uint32_t kernelESP)
{
	TSS_SetStack(kernelSS, kernelESP);
}

void HAL_InitialiseInterrupts()
{
	// Install our exception handlers
	HAL_SetInterruptVector (0, DivideByZeroFault, 0);
	HAL_SetInterruptVector (1, SingleStepTrap, 0);
	HAL_SetInterruptVector (2, NMITrap, 0);
	HAL_SetInterruptVector (3, BreakpointTrap, 0);
	HAL_SetInterruptVector (4, OverflowTrap, 0);
	HAL_SetInterruptVector (5, BoundsCheckFault, 0);
	HAL_SetInterruptVector (6, InvalidOpcodeFault, 0);
	HAL_SetInterruptVector (7, NoDeviceFault, 0);
	HAL_SetInterruptVector (8, DoubleFaultAbort, 0);
	HAL_SetInterruptVector (10, InvalidTSSFault, 0);
	HAL_SetInterruptVector (11, NoSegmentFault, 0);
	HAL_SetInterruptVector (12, StackFault, 0);
	HAL_SetInterruptVector (13, GeneralProtectionFault, 0);
	HAL_SetInterruptVector (14, PageFault, 0);
	HAL_SetInterruptVector (16, FPUFault, 0);
	HAL_SetInterruptVector (17, AlignmentCheckFault, 0);
	HAL_SetInterruptVector (18, MachineCheckAbort, 0);
	HAL_SetInterruptVector (19, SimdFPUFault, 0);
	HAL_EnableInterrupts();
}

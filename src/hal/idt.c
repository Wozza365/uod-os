//    Idt.c
//
//	Interrupt Descriptor Table. The IDT is responsible for providing
//	the interface for managing interrupts, installing, setting, requesting,
//	generating, and interrupt callback managing.

#include "idt.h"
#include <string.h>
#include <hal.h>
#include <console.h>
#include <exception.h>

// The structure for the processors idtr register
typedef struct 
{
	uint16_t		limit;		// Size of the interrupt descriptor table (idt)
	uint32_t		base;		// base address of idt
} __attribute__((packed)) idtr;


// Interrupt descriptor table
static IDT_Descriptor	_idt [I86_MAX_INTERRUPTS];

// idtr structure used to help define the cpu's idtr register
static idtr				_idtr;

// Install idtr into processors idtr register

static void IDT_Install() 
{
	asm volatile("lidt (%0) \n" : : "m"(_idtr));
}

// Default handler to catch unhandled system interrupts.  If you want it to 
// display a message when this happens, then uncomment out the appropriate lines.
// However, Bochs is known to generate spurious interrupts occasionally, so only
// do this if necessary.

#if __GNUC__ >= 7

static __attribute__ ((interrupt)) void I86_DefaultInterruptHandler(struct interrupt_frame *frame) 
{
	//ConsoleWriteString("I86_DefaultInterruptHandler: Unhandled Exception at ");
	//ConsoleWriteInt(frame->ip, 16);
	//ConsoleWriteString("\n");
}
#else
static void I86_DefaultInterruptHandler() 
{
	// Save the registers
	//asm("pushal");
	
	//ConsoleWriteString("I86_DefaultInterruptHandler: Unhandled Exception\n");

	// Restore registers
	//asm("popal");	
	// Since this is an interrupt handler, we
	// need to finish via iret, not ret
	asm("leave");
	asm("iret");
}

#endif

// Returns interrupt descriptor

IDT_Descriptor* I86_IDT_GetInterruptHandler(uint32_t i) 
{
	if (i > I86_MAX_INTERRUPTS)
	{
		return 0;
	}
	return &_idt[i];
}

// Install a new interrupt handler
int I86_IDT_InstallInterruptHandler(uint32_t i, 
                                    uint16_t flags, 
									uint16_t sel, 
									I86_IRQ_HANDLER irq) 
{
	if (i > I86_MAX_INTERRUPTS)	{
		return 0;
	}
	if (!irq)
	{
		return 0;
	}
	
	// Get base address of interrupt handler
	uint32_t uiBase = (uint32_t)irq;

	// Store base address into idt
	_idt[i].baseLo	 = (uint16_t)(uiBase & 0xffff);
	_idt[i].baseHi	 = (uint16_t)((uiBase >> 16) & 0xffff);
	_idt[i].reserved = 0;
	_idt[i].flags	 = (uint8_t)(flags);
	_idt[i].sel		 = sel;
	return 0;
}

// Initialise idt

int I86_IDT_Initialise(uint16_t codeSel) 
{
	// Set up idtr for processor
	_idtr.limit = sizeof(IDT_Descriptor) * I86_MAX_INTERRUPTS - 1;
	_idtr.base	= (uint32_t)&_idt[0];

	// Null out the idt
	memset((void*)&_idt[0], 0, sizeof(IDT_Descriptor) * I86_MAX_INTERRUPTS);

	// Register default handlers
	for (int i = 0; i < I86_MAX_INTERRUPTS; i++)
	{
		I86_IDT_InstallInterruptHandler(i, 
										I86_IDT_DESC_PRESENT | I86_IDT_DESC_BIT32,
										codeSel, 
										I86_DefaultInterruptHandler);
	}
	// install our idt
	IDT_Install ();
	return 0;
}

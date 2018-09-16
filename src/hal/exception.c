//	System exception handlers. These are registered during system
//	initialization and called automatically when they are encountered

#include <exception.h>
#include <hal.h>
#include <console.h>

#if __GNUC__ >= 7
// Exception handlers that make use of the new attributes provided by 
// GCC 7.x.

void KernelPanic (const char* message, struct interrupt_frame *frame);

// Divide by zero

__attribute__ ((interrupt)) void DivideByZeroFault(struct interrupt_frame *frame) 
{
	KernelPanic("Divide by 0", frame);
}

// Single Step
__attribute__ ((interrupt)) void SingleStepTrap(struct interrupt_frame *frame) 
{
	KernelPanic("Single step", frame);
}

// Non-Maskable Interrupt
__attribute__ ((interrupt)) void NMITrap(struct interrupt_frame *frame) 
{
	KernelPanic("NMI trap", frame);
}

// Breakpoint hit
__attribute__ ((interrupt)) void BreakpointTrap(struct interrupt_frame *frame) 
{
	KernelPanic("Breakpoint trap", frame);
}

// Overflow
__attribute__ ((interrupt)) void OverflowTrap(struct interrupt_frame *frame) 
{
	KernelPanic("Overflow trap", frame);
}

// Bounds check
__attribute__ ((interrupt)) void BoundsCheckFault(struct interrupt_frame *frame) 
{
	KernelPanic("Bounds check fault", frame);
}

// Invalid opcode / instruction
__attribute__ ((interrupt)) void InvalidOpcodeFault(struct interrupt_frame *frame) 
{
	KernelPanic("Invalid opcode", frame);
}

// Device not available
__attribute__ ((interrupt)) void NoDeviceFault(struct interrupt_frame *frame) 
{
	KernelPanic("Device not found", frame);
}

// Double fault
__attribute__ ((interrupt)) void DoubleFaultAbort(struct interrupt_frame *frame, uint32_t err) 
{
	KernelPanic("Double fault", frame);
}

// Invalid Task State Segment (TSS)
__attribute__ ((interrupt)) void InvalidTSSFault(struct interrupt_frame *frame, uint32_t err) 
{
	KernelPanic("Invalid TSS", frame);
}

// Segment not present
__attribute__ ((interrupt)) void NoSegmentFault(struct interrupt_frame *frame, uint32_t err) 
{
	KernelPanic("Invalid segment", frame);
}

// Stack fault
__attribute__ ((interrupt)) void StackFault(struct interrupt_frame *frame, uint32_t err) 
{
	KernelPanic("Stack fault", frame);
}

// General protection fault
__attribute__ ((interrupt)) void GeneralProtectionFault(struct interrupt_frame *frame, uint32_t err) 
{
	KernelPanic("General Protection Fault", frame);
}

// Page fault
__attribute__ ((interrupt)) void PageFault(struct interrupt_frame *frame, uint32_t err) 
{
	KernelPanic("Page Fault", frame);
}

// Floating Point Unit (FPU) error
__attribute__ ((interrupt)) void FPUFault(struct interrupt_frame *frame) 
{
	KernelPanic("FPU Fault", frame);
}

// Alignment check
__attribute__ ((interrupt)) void AlignmentCheckFault(struct interrupt_frame *frame, uint32_t err) 
{
	KernelPanic("Alignment Check", frame);
}

// machine check
__attribute__ ((interrupt)) void MachineCheckAbort(struct interrupt_frame *frame) 
{
	KernelPanic("Machine Check", frame);
}

// Floating Point Unit (FPU) Single Instruction Multiple Data (SIMD) error
__attribute__ ((interrupt)) void SimdFPUFault(struct interrupt_frame *frame) 
{
	KernelPanic("FPU SIMD fault", frame);
}

// Something has gone very wrong. We cannot continue.
void KernelPanic(const char* message, struct interrupt_frame *frame) 
{
	HAL_DisableInterrupts();

	ConsoleClearScreen(0x1f);
	ConsoleWriteString((char *)message);
	ConsoleWriteString(" at ");
	ConsoleWriteInt(frame->ip, 16);
	for (;;);
}
#else
// Exception handlers pre-GCC 7.x
//	
// For now, all of these interrupt handlers just disable hardware interrupts
// and calls kernal_panic(). This displays an error and halts the system

void KernelPanic (const char* message);

// Divide by zero
void DivideByZeroFault() 
{
	KernelPanic("Divide by 0");
	for (;;);
}

// Single Step
void SingleStepTrap() 
{
	KernelPanic("Single step");
	for (;;);
}

// Non-Maskable Interrupt
void NMITrap() 
{
	KernelPanic("NMI trap");
	for (;;);
}

// Breakpoint hit
void BreakpointTrap() 
{
	KernelPanic("Breakpoint trap");
	for (;;);
}

// Overflow
void OverflowTrap() 
{
	KernelPanic("Overflow trap");
	for (;;);
}

// Bounds check
void BoundsCheckFault() 
{
	KernelPanic("Bounds check fault");
	for (;;);
}

// Invalid opcode / instruction
void InvalidOpcodeFault() 
{
	KernelPanic("Invalid opcode");
	for (;;);
}

// Device not available
void NoDeviceFault() 
{
	KernelPanic("Device not found");
	for (;;);
}

// Double fault
void DoubleFaultAbort(unsigned int err) 
{
	KernelPanic("Double fault");
	for (;;);
}

// Invalid Task State Segment (TSS)
void InvalidTSSFault(unsigned int err) 
{
	KernelPanic("Invalid TSS");
	for (;;);
}

// Segment not present
void NoSegmentFault(unsigned int err) 
{
	KernelPanic("Invalid segment");
	for (;;);
}

// Stack fault
void StackFault(unsigned int err) 
{
	KernelPanic("Stack fault");
	for (;;);
}

// General protection fault
void GeneralProtectionFault(unsigned int err) 
{
	KernelPanic("General Protection Fault");
	for (;;);
}

// Page fault
void PageFault(unsigned int err) 
{
	KernelPanic("Page Fault");
	for (;;);
}

// Floating Point Unit (FPU) error
void FPUFault() 
{
	KernelPanic("FPU Fault");
	for (;;);
}

// Alignment check
void AlignmentCheckFault(unsigned int err) 
{
	KernelPanic("Alignment Check");
	for (;;);
}

// machine check
void MachineCheckAbort() 
{
	KernelPanic("Machine Check");
	for (;;);
}

// Floating Point Unit (FPU) Single Instruction Multiple Data (SIMD) error
void SimdFPUFault() 
{
	KernelPanic("FPU SIMD fault");
	for (;;);
}

// Something has gone very wrong. We cannot continue.
void KernelPanic(const char* message) 
{
	HAL_DisableInterrupts();

	ConsoleClearScreen(0x1f);
	ConsoleWriteString((char *)message);
	for (;;);
}

	
#endif

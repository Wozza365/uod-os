
#ifndef _EXCEPTION_H
#define _EXCEPTION_H
#include <stdint.h>

#if __GNUC__ >= 7
// GCC 7.x versions

struct interrupt_frame
{
	uint32_t	ip;
	uint32_t	cs;
	uint32_t	flags;
	uint32_t	sp;
	uint32_t	ss;
};

// Divide by zero
__attribute__ ((interrupt)) void DivideByZeroFault(struct interrupt_frame *frame) ; 

// Single Step
__attribute__ ((interrupt)) void SingleStepTrap(struct interrupt_frame *frame);

// Non-Maskable Interrupt
__attribute__ ((interrupt)) void NMITrap(struct interrupt_frame *frame); 

// Breakpoint hit
__attribute__ ((interrupt)) void BreakpointTrap(struct interrupt_frame *frame); 

// Overflow
__attribute__ ((interrupt)) void OverflowTrap(struct interrupt_frame *frame); 

// Bounds check
__attribute__ ((interrupt)) void BoundsCheckFault(struct interrupt_frame *frame); 

// Invalid opcode / instruction
__attribute__ ((interrupt)) void InvalidOpcodeFault(struct interrupt_frame *frame); 

// Device not available
__attribute__ ((interrupt)) void NoDeviceFault(struct interrupt_frame *frame); 

// Double fault
__attribute__ ((interrupt)) void DoubleFaultAbort(struct interrupt_frame *frame, uint32_t err); 

// Invalid Task State Segment (TSS)
__attribute__ ((interrupt)) void InvalidTSSFault(struct interrupt_frame *frame, uint32_t err); 

// Segment not present
__attribute__ ((interrupt)) void NoSegmentFault(struct interrupt_frame *frame, uint32_t err); 

// Stack fault
__attribute__ ((interrupt)) void StackFault(struct interrupt_frame *frame, uint32_t err); 

// General protection fault
__attribute__ ((interrupt)) void GeneralProtectionFault(struct interrupt_frame *frame, uint32_t err); 

// Page fault
__attribute__ ((interrupt)) void PageFault(struct interrupt_frame *frame, uint32_t err); 

// Floating Point Unit (FPU) error
__attribute__ ((interrupt)) void FPUFault(struct interrupt_frame *frame); 

// Alignment check
__attribute__ ((interrupt)) void AlignmentCheckFault(struct interrupt_frame *frame, uint32_t err); 

// machine check
__attribute__ ((interrupt)) void MachineCheckAbort(struct interrupt_frame *frame); 

// Floating Point Unit (FPU) Single Instruction Multiple Data (SIMD) error
__attribute__ ((interrupt)) void SimdFPUFault(struct interrupt_frame *frame); 
#else

// Divide by zero
void DivideByZeroFault(); 

// Single Step
void SingleStepTrap();

// Non-Maskable Interrupt
void NMITrap(); 

// Breakpoint hit
void BreakpointTrap(); 

// Overflow
void OverflowTrap(); 

// Bounds check
void BoundsCheckFault(); 

// Invalid opcode / instruction
void InvalidOpcodeFault(); 

// Device not available
void NoDeviceFault(); 

// Double fault
void DoubleFaultAbort(unsigned int err); 

// Invalid Task State Segment (TSS)
void InvalidTSSFault(unsigned int err); 

// Segment not present
void NoSegmentFault(unsigned int err); 

// Stack fault
void StackFault(unsigned int err); 

// General protection fault
void GeneralProtectionFault(unsigned int err); 

// Page fault
void PageFault(unsigned int err); 

// Floating Point Unit (FPU) error
void FPUFault(); 

// Alignment check
void AlignmentCheckFault(unsigned int err); 

// machine check
void MachineCheckAbort(); 

// Floating Point Unit (FPU) Single Instruction Multiple Data (SIMD) error
void SimdFPUFault(); 

#endif

#endif
#include <hal.h>
#include <console.h>
#include <keyboard.h>
#include <draw.h>
#include <print.h>

#define MAX_CONSOLECALL 5
#define MAX_DRAWCALL 11
#define MAX_TEXTCALL 2

typedef struct _SysCallInfo
{
	void *SysCall;
	int ParamCount;
} SysCallInfo;

SysCallInfo _ConsoleCalls[MAX_CONSOLECALL];
SysCallInfo _DrawCalls[MAX_DRAWCALL];
SysCallInfo _TextCalls[MAX_TEXTCALL];

void InitialiseConsoleCall(int index, void *sysCall, int paramCount)
{
	if (index >= 0 && index < MAX_CONSOLECALL)
	{
		_ConsoleCalls[index].SysCall = sysCall;
		_ConsoleCalls[index].ParamCount = paramCount;
	}
}

void InitialiseDrawCall(int index, void *sysCall, int paramCount)
{
	if (index >= 0 && index < MAX_DRAWCALL)
	{
		_DrawCalls[index].SysCall = sysCall;
		_DrawCalls[index].ParamCount = paramCount;
	}
}
void InitialiseTextCall(int index, void *sysCall, int paramCount)
{
	if (index >= 0 && index < MAX_TEXTCALL)
	{
		_TextCalls[index].SysCall = sysCall;
		_TextCalls[index].ParamCount = paramCount;
	}
}

// The new interrupt attribute has not been used here since
// the code is so specialised.

void ConsoleCallDispatcher()
{
	static int index = 0;
	// Get index into _SysCalls table from eax
	asm volatile("movl %%eax, %0"
				 : "=r"(index));

	if (index < MAX_CONSOLECALL)
	{
		// Temporarily save the registers that are used to pass in the parameters
		asm volatile("push %edx\n\t"
					 "push %ecx\n\t"
					 "push %ebx\n\t");
		void *sysFunction = _ConsoleCalls[index].SysCall;
		int paramCount = _ConsoleCalls[index].ParamCount;
		// Now generate the code for the user call.  There is different
		// code depending on how many parameters are passed to the function.
		// After the call to the kernel routine, we remove the parameters from teh
		// stack by adjusting the stack pointer.  This is the standard C calling convention.
		switch (paramCount)
		{
		case 3:
			asm volatile("pop %%ebx\n\t"
						 "pop %%ecx\n\t"
						 "pop %%edx\n\t"
						 "push %%edx\n\t"
						 "push %%ecx\n\t"
						 "push %%ebx\n\t"
						 "call *%0\n\t"
						 "addl $12, %%esp"
						 :
						 : "r"(sysFunction));
			break;

		case 2:
			asm volatile("pop %%ebx\n\t"
						 "pop %%ecx\n\t"
						 "pop %%edx\n\t"
						 "push %%ecx\n\t"
						 "push %%ebx\n\t"
						 "call *%0\n\t"
						 "addl $8, %%esp"
						 :
						 : "r"(sysFunction));
			break;

		case 1:
			asm volatile("pop %%ebx\n\t"
						 "pop %%ecx\n\t"
						 "pop %%edx\n\t"
						 "push %%ebx\n\t"
						 "call *%0\n\t"
						 "addl $4, %%esp"
						 :
						 : "r"(sysFunction));
			break;

		case 0:
			asm volatile("pop %%ebx\n\t"
						 "pop %%ecx\n\t"
						 "pop %%edx\n\t"
						 "call *%0\n\t"
						 :
						 : "r"(sysFunction));
			break;
		}
	}
	asm("leave");
	asm("iret");
}

void DrawcallDispatcher()
{
	static int index = 0;
	// Get index into _SysCalls table from eax
	asm volatile("movl %%eax, %0"
				 : "=r"(index));

	if (index < MAX_DRAWCALL)
	{
		// Temporarily save the registers that are used to pass in the parameters
		asm volatile("push %edx\n\t"
					 "push %ecx\n\t"
					 "push %ebx\n\t");
		void *sysFunction = _DrawCalls[index].SysCall;
		int paramCount = _DrawCalls[index].ParamCount;
		// Now generate the code for the user call.  There is different
		// code depending on how many parameters are passed to the function.
		// After the call to the kernel routine, we remove the parameters from teh
		// stack by adjusting the stack pointer.  This is the standard C calling convention.
		switch (paramCount)
		{
		case 3:
			asm volatile("pop %%ebx\n\t"
						 "pop %%ecx\n\t"
						 "pop %%edx\n\t"
						 "push %%edx\n\t"
						 "push %%ecx\n\t"
						 "push %%ebx\n\t"
						 "call *%0\n\t"
						 "addl $12, %%esp"
						 :
						 : "r"(sysFunction));
			break;

		case 2:
			asm volatile("pop %%ebx\n\t"
						 "pop %%ecx\n\t"
						 "pop %%edx\n\t"
						 "push %%ecx\n\t"
						 "push %%ebx\n\t"
						 "call *%0\n\t"
						 "addl $8, %%esp"
						 :
						 : "r"(sysFunction));
			break;

		case 1:
			asm volatile("pop %%ebx\n\t"
						 "pop %%ecx\n\t"
						 "pop %%edx\n\t"
						 "push %%ebx\n\t"
						 "call *%0\n\t"
						 "addl $4, %%esp"
						 :
						 : "r"(sysFunction));
			break;

		case 0:
			asm volatile("pop %%ebx\n\t"
						 "pop %%ecx\n\t"
						 "pop %%edx\n\t"
						 "call *%0\n\t"
						 :
						 : "r"(sysFunction));
			break;
		}
	}
	asm("leave");
	asm("iret");
}

void TextCallDispatcher()
{
	static int index = 0;
	// Get index into _SysCalls table from eax
	asm volatile("movl %%eax, %0"
				 : "=r"(index));

	if (index < MAX_TEXTCALL)
	{
		// Temporarily save the registers that are used to pass in the parameters
		asm volatile("push %edx\n\t"
					 "push %ecx\n\t"
					 "push %ebx\n\t");
		void *sysFunction = _TextCalls[index].SysCall;
		int paramCount = _TextCalls[index].ParamCount;
		// Now generate the code for the user call.  There is different
		// code depending on how many parameters are passed to the function.
		// After the call to the kernel routine, we remove the parameters from teh
		// stack by adjusting the stack pointer.  This is the standard C calling convention.
		switch (paramCount)
		{
		case 3:
			asm volatile("pop %%ebx\n\t"
						 "pop %%ecx\n\t"
						 "pop %%edx\n\t"
						 "push %%edx\n\t"
						 "push %%ecx\n\t"
						 "push %%ebx\n\t"
						 "call *%0\n\t"
						 "addl $12, %%esp"
						 :
						 : "r"(sysFunction));
			break;

		case 2:
			asm volatile("pop %%ebx\n\t"
						 "pop %%ecx\n\t"
						 "pop %%edx\n\t"
						 "push %%ecx\n\t"
						 "push %%ebx\n\t"
						 "call *%0\n\t"
						 "addl $8, %%esp"
						 :
						 : "r"(sysFunction));
			break;

		case 1:
			asm volatile("pop %%ebx\n\t"
						 "pop %%ecx\n\t"
						 "pop %%edx\n\t"
						 "push %%ebx\n\t"
						 "call *%0\n\t"
						 "addl $4, %%esp"
						 :
						 : "r"(sysFunction));
			break;

		case 0:
			asm volatile("pop %%ebx\n\t"
						 "pop %%ecx\n\t"
						 "pop %%edx\n\t"
						 "call *%0\n\t"
						 :
						 : "r"(sysFunction));
			break;
		}
	}
	asm("leave");
	asm("iret");
}

#define I86_IDT_DESC_RING3 0x60

void InitialiseSysCalls()
{
	InitialiseConsoleCall(0, ConsoleWriteString, 1);
	InitialiseConsoleCall(1, ConsoleWriteCharacter, 1);
	InitialiseConsoleCall(2, ConsoleWriteInt, 2);
	InitialiseConsoleCall(3, ConsoleClearScreen, 1);
	InitialiseConsoleCall(4, ConsoleGotoXY, 2);

	//Initialise user draw calls using separate init function
	InitialiseDrawCall(0, SetPixel, 3);
	InitialiseDrawCall(1, DrawUserLine, 3);
	InitialiseDrawCall(2, ClearScreen, 1);
	InitialiseDrawCall(3, DrawUserHorizontalLine, 3);
	InitialiseDrawCall(4, DrawUserVerticalLine, 3);
	InitialiseDrawCall(5, DrawUserRectangle, 3);
	InitialiseDrawCall(6, FillUserRectangle, 3);
	InitialiseDrawCall(7, DrawUserCircle, 3);
	InitialiseDrawCall(8, FillUserCircle, 3);
	InitialiseDrawCall(9, DrawUserPolygon, 3);
	InitialiseDrawCall(10, FillUserPolygon, 3);

	//Initialise text writing calls
	InitialiseTextCall(0, WriteUserCharacter, 3);
	InitialiseTextCall(1, WriteUserText, 3);

	// Install interrupt handler!
	HAL_SetInterruptVector(0x80, ConsoleCallDispatcher, I86_IDT_DESC_RING3);
	HAL_SetInterruptVector(0x81, DrawcallDispatcher, I86_IDT_DESC_RING3);
	HAL_SetInterruptVector(0x82, TextCallDispatcher, I86_IDT_DESC_RING3); //another interrupt for text output functions

}
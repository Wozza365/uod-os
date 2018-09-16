#include <string.h>
#include <console.h>
#include <hal.h>
#include <exception.h>
#include "physicalmemorymanager.h"
#include "virtualmemorymanager.h"
#include "bootinfo.h"
#include <keyboard.h>
#include <sysapi.h>
#include <user.h>
#include <vgamodes.h>
#include <print.h>
#include <draw.h>
#include <math.h>

#define PI 3.14159265
#define PI_2 6.2831853
#define ARR 3
uint16_t arrl = 3;

// This is a dummy __main.  For some reason, gcc puts in a call to
// __main from main, so we just include a dummy.

BootInfo *_bootInfo;

void __main() {}

void InitialisePhysicalMemory()
{
	// Initialise the physical memory manager
	// We place the memory bit map in the next available block of memory after the kernel.
	// Note that all memory addresses here are physical memory addresses since we are dealing
	// with management of physical memory

	uint32_t memoryMapAddress = 0x100000 + _bootInfo->KernelSize;
	if (memoryMapAddress % PMM_GetBlockSize() != 0)
	{
		// Make sure that the memory map is going to be aligned on a block boundary
		memoryMapAddress = (memoryMapAddress / PMM_GetBlockSize() + 1) * PMM_GetBlockSize();
	}
	uint32_t sizeOfMemoryMap = PMM_Initialise(_bootInfo, memoryMapAddress);

	// We now need to mark various regions as unavailable

	// Mark first page as unavailable (so that a null pointer is invalid)
	PMM_MarkRegionAsUnavailable(0x00, PMM_GetBlockSize());

	// Mark memory used by kernel as unavailable
	PMM_MarkRegionAsUnavailable(0x100000, _bootInfo->KernelSize);

	// Mark memory used by memory map as unavailable
	PMM_MarkRegionAsUnavailable(PMM_GetMemoryMap(), sizeOfMemoryMap);

	// Reserve two blocks for the stack and make unavailable (the stack is set at 0x90000 in boot loader)
	uint32_t stackSize = PMM_GetBlockSize() * 2;
	PMM_MarkRegionAsUnavailable(_bootInfo->StackTop - stackSize, stackSize);

	// Reserve two block for the kernel stack and make unavailable
	PMM_MarkRegionAsUnavailable(0x80000 - stackSize, stackSize);
}

uint16_t ChooseResolutionWidth()
{
	ConsoleWriteString("CHOOSE A WIDTH\n");
	ConsoleWriteString("Q 256  W 320  E 360  R 376  T 400\n");

	keycode k = KeyboardGetCharacter();
	char c = KeyboardConvertKeyToASCII(k);

	//press the corresponding letter
	switch (c)
	{
	case 'q':
		return 256;
	case 'w':
		return 320;
	case 'e':
		return 360;
	case 'r':
		return 376;
	case 't':
		return 400;
	default:
		return 320;
	}
}
uint16_t ChooseResolutionHeight()
{
	ConsoleWriteString("CHOOSE A HEIGHT\n");
	ConsoleWriteString("Q 200  W 224  E 240  R 256  T 270  Y 300  U 360  I 400  O 480  P 564  A 600\n");

	keycode k = KeyboardGetCharacter();
	char c = KeyboardConvertKeyToASCII(k);

	//press the corresponding letter
	switch (c)
	{
	case 'q':
		return 200;
	case 'w':
		return 224;
	case 'e':
		return 240;
	case 'r':
		return 256;
	case 't':
		return 270;
	case 'y':
		return 300;
	case 'u':
		return 360;
	case 'i':
		return 400;
	case 'o':
		return 480;
	case 'p':
		return 564;
	case 'a':
		return 600;
	default:
		return 200;
	}
}

void CreateColourPalette() {
	//6 portions of each colour to get a fair representation
	uint8_t coloursIndex[] = {0, 12, 24, 36, 48, 60};
	uint8_t length = 6;
	int i, j, k;
	//a variety of colours
	for (i = 0; i < length; i++) {
		for (j = 0; j < length; j++) {
			for (k = 0; k < length; k++) {
				HAL_OutputByteToPort(0x3C8, (i*length*length) + (j*length) + k + 16);
				HAL_OutputByteToPort(0X3C9, coloursIndex[i]);
				HAL_OutputByteToPort(0X3C9, coloursIndex[j]);
				HAL_OutputByteToPort(0X3C9, coloursIndex[k]);
			}
		}
	}
	//white to black for remaining
	for (i = 232; i < 255; i++) {
		HAL_OutputByteToPort(0x3C8, i);
		HAL_OutputByteToPort(0X3C9, i * 3);
		HAL_OutputByteToPort(0X3C9, i * 3);
		HAL_OutputByteToPort(0X3C9, i * 3);
	}
}

void Initialise()
{
	ConsoleClearScreen(0x1F);
	ConsoleWriteString("UODOS 32-bit Kernel. Kernel size is ");
	ConsoleWriteInt(_bootInfo->KernelSize, 10);
	ConsoleWriteString(" bytes\n");
	HAL_Initialise();
	InitialisePhysicalMemory();
	VMM_Initialise();
	KeyboardInstall(33);
	InitialiseSysCalls();
}

void main(BootInfo *bootInfo)
{
	_bootInfo = bootInfo;
	Initialise();

	HAL_EnterUserMode();

	//User input for resolution
	uint16_t width = ChooseResolutionWidth();
	uint16_t height = ChooseResolutionHeight();
	screenWidth = width;
	screenHeight = height;
	chain4 = 0;
	screenColour = 238;

	VGA_SetGraphicsMode(width, height, chain4);
	//VGA_SetGraphicsMode(320, 200,1); //for testing chain 4 enabled

	CreateColourPalette();
	User_ClearScreen(screenColour);

	// //Colour palette demo
	for (int i = 0; i < 255; i++) {
		User_DrawLine(0,i, 50, i, i);
	}

	//Drawing UI borders
	User_DrawHorizontalLine(0, 260, screenWidth, 5);
	User_DrawVerticalLine(60, 0, 260, 5);
	User_DrawVerticalLine(130, 0, 260, 5);
	User_DrawVerticalLine(230, 0, 260, 5);

	//Rectangle demos
	User_DrawRectangle(70, 10, 50, 70, 25);
	User_FillRectangle(70, 90, 50, 50, 15);
	User_DrawRectangle(70, 150, 50, 30, 75);
	User_FillRectangle(70, 190, 50, 65, 165);

	//Circle demos
	User_FillCircle(160, 30, 20, 45);
	User_FillCircle(190, 45, 25, 145);
	User_DrawCircle(180, 110, 40, 77);
	User_DrawCircle(165, 160, 25, 201);

	User_WriteText("f1 to f10 to change polygon  type below", 10, 261, 5);

	//declare variables outside loop as optimization
	int i = 0;
	uint8_t polyWidth = screenWidth - 231;
	uint8_t r = 50;
	
	uint16_t polyCentreX = 315,
			 polyCentreY1 = 75,
			 polyCentreY2 = 180,
			 polySize = 2,
			 textX = 0;

	uint16_t xPoints[64];
	uint16_t yPoints1[64];
	uint16_t yPoints2[64];

	float angle = 0.0f,
    	  theta,
		  rcos,
		  rsin;

	while (1) {
		keycode k = KeyboardGetCharacter();

		if (k == KEY_F1 || k == KEY_F2 || k == KEY_F3 || k == KEY_F4 || k == KEY_F5 || 
			k == KEY_F6 || k == KEY_F7 || k == KEY_F8 || k == KEY_F9 || k == KEY_F10) {
			//manipulate the polygons in different ways
			if (k == KEY_F1 && polySize < 63) {
				polySize++; //change number of sides
			} else if (k == KEY_F2 && polySize > 3) {
				polySize--;
			} else if (k == KEY_F3) {
				polyCentreY1--; //move in Y direction
				polyCentreY2--;
			} else if (k == KEY_F4) {
				polyCentreY1++;
				polyCentreY2++;
			} else if (k == KEY_F5) {
				polyCentreX--; //move in X direction
			} else if (k == KEY_F6) {
				polyCentreX++;
			} else if (k == KEY_F7) {
				r++;//increase radius and size
			} else if (k == KEY_F8) {
				r--;
			} else if (k == KEY_F9) {
				angle += 0.314f; //~5% rotation
			} else if (k == KEY_F10) {
				angle -= 0.314f;
			}

			//Reset the polygon region
			User_FillRectangle(231, 0, polyWidth, 259, screenColour);
			//Generate the points of a regular polygon, similar to how a circle may be drawn with triangles, but much lower number of edges.
			for (i = 0; i < polySize; i++) {
				theta = (PI_2/polySize) * i;
				rcos = r * cos(theta + angle);
				rsin = r * sin(theta + angle);
				xPoints[i] = polyCentreX + rcos;
				yPoints1[i] = polyCentreY1 + rsin;
				yPoints2[i] = polyCentreY2 + rsin;
			}
			//Draw Polygons
			User_FillPolygon(xPoints, yPoints1, polySize, polySize * 4 - 3);
			User_DrawPolygon(xPoints, yPoints2, polySize, polySize * 4);
		} else {
			//Convert key and output it to screen (will display nothing if not in current character set)
			char c = KeyboardConvertKeyToASCII(k);
			User_WriteCharacter(c, textX, 279, 5);
			textX += 10;
		}
	}
}
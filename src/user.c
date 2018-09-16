#include <console.h>
#include <keyboard.h>
#include <draw.h>

void User_ConsoleWriteCharacter(unsigned char c)
{
	asm volatile("movl $1, %%eax\n\t"
				 "movzx %0, %%ebx\n\t"
				 "int $0x80\n"
				 : : "b"(c)
				);
}

void User_ConsoleWriteString(char* str)
{
	asm volatile("xor %%eax, %%eax\n\t"
				 "leal (%0), %%ebx\n\t"
				 "int $0x80\n"
				 : : "b"(str)
				);
}

void User_ConsoleWriteInt(unsigned int i, unsigned int base)
{
	asm volatile("movl $2, %%eax\n\t"
				 "movl %0, %%ecx\n\t"
				 "movl %1, %%ebx\n\t"
				 "int $0x80\n"
				 : : "c"(base), "b"(i)
				);
}

void User_ConsoleClearScreen(const uint8_t c)
{
	asm volatile("movl $3, %%eax\n\t"
				 "movzx %0, %%ebx\n\t"
				 "int $0x80\n"
				 : : "b"(c)
				);
}

void User_ConsoleGotoXY(unsigned int x, unsigned int y) 
{
	asm volatile("movl $4, %%eax\n\t"
				 "movl %0, %%ecx\n\t"
				 "movl %1, %%ebx\n\t"
				 "int $0x80\n"
				 : : "c"(y), "b"(x)
				);
}

void User_SetPixel(unsigned int x, unsigned int y, uint8_t colour) {
	asm volatile("movl $0, %%eax\n\t"
				 "movzx %0, %%edx\n\t"
				 "movl %1, %%ecx\n\t"
				 "movl %2, %%ebx\n\t"
				 "int $0x81\n"
				 : : "d"(colour), "c"(y), "b"(x)
				);
}

//squeeze 2 16bit values into a single 32 bit register using bitshifting. This is then reversed at the other end.
uint32_t MergeTwo16Bit(uint16_t a, uint16_t b) {
	uint32_t start = (uint32_t)a;
	start = start << 16;
	start += (uint32_t)b;
	return start;
}

void User_DrawLine(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY, uint8_t colour) {
	uint32_t start = MergeTwo16Bit(startX, startY);
	uint32_t end = MergeTwo16Bit(endX, endY);

	asm volatile("movl $1, %%eax\n\t"
				 "movl %0, %%ebx\n\t"
				 "movl %1, %%ecx\n\t"
				 "movzx %2, %%edx\n\t"
				 "int $0x81\n"
				 : : "b"(start), "c"(end), "d"(colour)
				);
}

void User_ClearScreen(uint8_t colour) {
	asm volatile("movl $2, %%eax\n\t"
				 "movzx %0, %%ebx\n\t"
				 "int $0x81\n"
				 : : "b"(colour)
				);
}

void User_DrawHorizontalLine(uint16_t startX, uint16_t startY, uint16_t length, uint8_t colour) {
	uint32_t start = MergeTwo16Bit(startX, startY);
	asm volatile("movl $3, %%eax\n\t"
				 "movl %0, %%ebx\n\t"
				 "movzx %1, %%ecx\n\t"
				 "movzx %2, %%edx\n\t"
				 "int $0x81\n"
				 : : "b"(start), "c"(length), "d"(colour)
				);
}

void User_DrawVerticalLine(uint16_t startX, uint16_t startY, uint16_t length, uint8_t colour) {
	uint32_t start = MergeTwo16Bit(startX, startY);
	//Vector2 s = Reverse32BitMergeVector2(start);

	// uint32_t start = (uint32_t)startX;
	// start = start << 16;
	// start += (uint32_t)startY;

	asm volatile("movl $4, %%eax\n\t"
				 "movl %0, %%ebx\n\t"
				 "movzx %1, %%ecx\n\t"
				 "movzx %2, %%edx\n\t"
				 "int $0x81\n"
				 : : "b"(start), "c"(length), "d"(colour)
				);
}

void User_DrawRectangle(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height, uint8_t colour) {
	uint32_t start = MergeTwo16Bit(startX, startY);
	uint32_t size = MergeTwo16Bit(width, height);

	asm volatile("movl $5, %%eax\n\t"
			     "movl %0, %%ebx\n\t"
				 "movl %1, %%ecx\n\t"
				 "movzx %2, %%edx\n\t"
				 "int $0x81\n"
				 : : "b"(start), "c"(size), "d"(colour)
				);
}

void User_FillRectangle(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height, uint8_t colour) {
	uint32_t start = MergeTwo16Bit(startX, startY);
	uint32_t size = MergeTwo16Bit(width, height);

	asm volatile("movl $6, %%eax\n\t"
				 "movl %0, %%ebx\n\t"
				 "movl %1, %%ecx\n\t"
				 "movzx %2, %%edx\n\t"
				 "int $0x81\n"
				 : : "b"(start), "c"(size), "d"(colour)
				);
}

void User_DrawCircle(uint16_t centreX, uint16_t centreY, uint16_t radius, uint8_t colour) {
	uint32_t centre = MergeTwo16Bit(centreX, centreY);

	asm volatile("movl $7, %%eax\n\t"
				 "movl %0, %%ebx\n\t"
				 "movzx %1, %%ecx\n\t"
				 "movzx %2, %%edx\n\t"
				 "int $0x81\n"
				 : : "b"(centre), "c"(radius), "d"(colour)
				);
}

void User_FillCircle(uint16_t centreX, uint16_t centreY, uint16_t radius, uint8_t colour) {
	uint32_t centre = MergeTwo16Bit(centreX, centreY);

	asm volatile("movl $8, %%eax\n\t"
				 "movl %0, %%ebx\n\t"
				 "movzx %1, %%ecx\n\t"
				 "movzx %2, %%edx\n\t"
				 "int $0x81\n"
				 : : "b"(centre), "c"(radius), "d"(colour)
				);
}


void User_DrawPolygon(uint16_t* xPoints, uint16_t* yPoints, uint16_t sides, uint8_t colour) {
	uint16_t colour16 = (uint16_t)colour;
	uint32_t sizeCol = MergeTwo16Bit(sides, colour16);
	asm volatile("movl $9, %%eax\n\t"
				 "movl %0, %%edx\n\t"
				 "mov %1, %%ecx\n\t"
				 "mov %2, %%ebx\n\t"
				 "int $0x81\n"
				 : : "d"(sizeCol), "c"(yPoints), "b"(xPoints)
				);
}

void User_FillPolygon(uint16_t* xPoints, uint16_t* yPoints, uint16_t sides, uint8_t colour) {
	uint16_t colour16 = (uint16_t)colour;
	uint32_t sizeCol = MergeTwo16Bit(sides, colour16);
	asm volatile("movl $10, %%eax\n\t"
				 "movl %0, %%edx\n\t"
				 "mov %1, %%ecx\n\t"
				 "mov %2, %%ebx\n\t"
				 "int $0x81\n"
				 : : "d"(sizeCol), "c"(yPoints), "b"(xPoints)
				);
}


void User_WriteCharacter(char c, uint16_t x, uint16_t y, uint8_t colour) {
	uint32_t position = MergeTwo16Bit(x, y);
	asm volatile("movl $0, %%eax\n\t"
				 "movzx %0, %%ebx\n\t"
				 "movl %1, %%ecx\n\t"
				 "movzx %2, %%edx\n\t"
				 "int $0x82\n"
				 : : "b"(c), "c"(position), "d"(colour)
				);
}

void User_WriteText(char* str, uint16_t x, uint16_t y, uint8_t colour) {
	uint32_t position = MergeTwo16Bit(x, y);
	asm volatile("movl $1, %%eax\n\t"
				 "leal (%0), %%ebx\n\t"
				 "movl %1, %%ecx\n\t"
				 "movzx %2, %%edx\n\t"
				 "int $0x82\n"
				 : : "b"(str), "c"(position), "d"(colour)
				);
}
#ifndef _USER_H
#define _USER_H
#include <keyboard.h>

void User_ConsoleWriteCharacter(unsigned char c); 
void User_ConsoleWriteString(char* str); 
void User_ConsoleWriteInt(unsigned int i, unsigned int base); 
void User_ConsoleClearScreen(const uint8_t c); 
void User_ConsoleGotoXY(unsigned int x, unsigned int y); 

void User_SetPixel(unsigned int x, unsigned int y, uint8_t colour);
void User_DrawLine(uint16_t startX, uint16_t startY, uint16_t endX, uint16_t endY, uint8_t colour);
void User_ClearScreen(uint8_t colour);
void User_DrawRectangle(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height, uint8_t colour);
void User_FillRectangle(uint16_t startX, uint16_t startY, uint16_t width, uint16_t height, uint8_t colour);
void User_DrawHorizontalLine(uint16_t startX, uint16_t startY, uint16_t length, uint8_t colour);
void User_DrawVerticalLine(uint16_t startX, uint16_t startY, uint16_t length, uint8_t colour);
void User_DrawCircle(uint16_t centreX, uint16_t centreY, uint16_t radius, uint8_t colour);
void User_FillCircle(uint16_t centreX, uint16_t centreY, uint16_t radius, uint8_t colour);
void User_DrawPolygon(uint16_t* xPoints, uint16_t* yPoints, uint16_t sides, uint8_t colour);
void User_FillPolygon(uint16_t* xPoints, uint16_t* yPoints, uint16_t sides, uint8_t colour);

void User_WriteCharacter(char c, uint16_t x, uint16_t y, uint8_t colour);
void User_WriteText(char* str, uint16_t x, uint16_t y, uint8_t colour);

#endif
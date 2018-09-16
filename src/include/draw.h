#include <stdint.h>

#define CONSOLE_HEIGHT 25
#define CONSOLE_WIDTH 80

uint16_t screenWidth;
uint16_t screenHeight;

uint8_t chain4;
uint8_t screenColour;

typedef struct {
    uint16_t x;
    uint16_t y;
} Vector2;


typedef struct {
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
} Rectangle;

void SetPixel(uint16_t x, uint16_t y, const uint8_t colour);

void DrawUserLine(uint32_t start, uint32_t end, uint8_t colour);

void DrawLine(Vector2 start, Vector2 end, uint8_t colour);

void DrawUserHorizontalLine(uint32_t start, uint16_t length, uint8_t colour);

void DrawHorizontalLine(Vector2 start, uint16_t length, uint8_t colour);

void DrawUserVerticalLine(uint32_t start, uint16_t length, uint8_t colour);

void DrawVerticalLine(Vector2 start, uint16_t length, uint8_t colour);

void DrawUserRectangle(uint32_t start, uint32_t size, uint8_t colour);

void DrawRectangle(Rectangle rect, uint8_t colour);

void FillUserRectangle(uint32_t start, uint32_t size, uint8_t colour);

void FillRectangle(Rectangle rect, uint8_t colour);

//calculated distance from each corner, weights based on distance and colour
void DrawGradientRectangle(Rectangle rect, unsigned int ColTL, unsigned int ColTR, unsigned int ColBL, unsigned int ColBR);

void ClearScreen(uint8_t colour);

void DrawUserCircle(uint32_t centre, uint16_t radius, uint8_t colour);

void DrawCircle(Vector2 centre, uint16_t radius, uint8_t colour);

void FillUserCircle(uint32_t centre, uint16_t radius, uint8_t colour);

void FillCircle(Vector2 centre, uint16_t radius, uint8_t colour);

void DrawUserPolygon(uint16_t xPoints[], uint16_t yPoints[], uint32_t sizeCol);

void DrawPolygon(uint16_t xPoints[], uint16_t yPoints[], uint16_t sides, uint8_t colour);

void FillUserPolygon(uint16_t xPoints[], uint16_t yPoints[], uint32_t sizeCol);

void FillPolygon(uint16_t xPoints[], uint16_t yPoints[], uint16_t sides, uint8_t colour);

Vector2 Reverse32BitMergeVector2(uint32_t a);
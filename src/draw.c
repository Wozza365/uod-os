#include <draw.h>
#include <stdint.h>
#include <print.h>
#include <hal.h>
#include <math.h>

uint8_t *_vgaMemory = (uint8_t *)0xA0000;

void SetPixel(uint16_t x, uint16_t y, const uint8_t colour)
{
    if (!chain4) {
        //select a plane to enable writing to based on x position
        // (x & 3) gets plane between 0-3
        // eg x = 5:
        // 101 (5) & 011 (3) = 001 (1) so second plane
        HAL_OutputByteToPort(0x3c5, 0x01 << (x & 3));
        _vgaMemory[(screenWidth * y + x)/4] = colour;
    } else {
        _vgaMemory[(screenWidth * y + x)] = colour;
    }
}

void ClearScreen(uint8_t colour)
{
    // 90 ticks (400x600)
    // int i;
    // int val = screenHeight * screenWidth;
    // for (i = 0; i < val; i++)
    // {
    //     _vgaMemory[i] = colour;
    // }

    //48 ticks (400x600)
    //We don't need to worry about planes here, as its still the same block of memory
    uint8_t* base = _vgaMemory;
    int val = screenHeight * screenWidth;
    while(val){
        base[--val] = colour;
    }
}

Vector2 Reverse32BitMergeVector2(uint32_t a) {
    //bitshifts 16 to get first value
    //then ANDs with 0x0000FFFF to get the second
    uint32_t tStart = a;
    uint32_t tStartX = tStart >> 16;
    uint16_t startX = (uint16_t)tStartX;
    uint32_t tStartY = tStart & 0x0000FFFF;
    uint16_t startY = (uint16_t)tStartY;
    Vector2 s = { .x = startX, .y = startY };
    return s;
}

void DrawUserHorizontalLine(uint32_t start, uint16_t length, uint8_t colour) {
    //Version used on the receiving end of user transfer code
    Vector2 s = Reverse32BitMergeVector2(start);
    DrawHorizontalLine(s, length, colour);
}

void DrawHorizontalLine(Vector2 start, uint16_t length, uint8_t colour) {
    if (chain4) {
        //extra optimized version for chain4mode, similar to clear screen
        uint8_t* base = _vgaMemory + (start.y * screenWidth) + start.x;
        int val = length;
        while(val){
            base[--val] = colour;
        }
    } else {
        //otherwise same implementation as vertical
        uint16_t x = start.x;
        uint16_t end = start.x + length;
        while (x < end) {
            SetPixel(x, start.y, colour);
            x++;
        }
    }
}

void DrawUserVerticalLine(uint32_t start, uint16_t length, uint8_t colour) {
    //Version used on the receiving end of user transfer code
    Vector2 s = Reverse32BitMergeVector2(start);
    DrawVerticalLine(s, length, colour);
}

void DrawVerticalLine(Vector2 start, uint16_t length, uint8_t colour) {
    //simply iterate through the positions in memory
    uint16_t y = start.y;
    uint16_t end = start.y + length;
    while (y < end) {
        SetPixel(start.x, y, colour);
        y++;
    }
}

void DrawUserLine(uint32_t start, uint32_t end, uint8_t colour) {
    //Version used on the receiving end of user transfer code
    Vector2 s = Reverse32BitMergeVector2(start);
    Vector2 e = Reverse32BitMergeVector2(end);
    DrawLine(s, e, colour);
}

void DrawLine(Vector2 start, Vector2 end, uint8_t colour) {
    // Vector2 d, pos, s1, e1;
    // int i, p;
    
    if (start.x == end.x && start.y == end.y) {
        return;
    }

    //fast bailouts for axis aligned lines
    if (start.x == end.x)
    {
        if (start.y < end.y) {
            DrawVerticalLine(start, end.y - start.y, colour);
        } else {
            Vector2 s = { .x = start.x, .y = end.y };
            DrawVerticalLine(s, start.y - end.y, colour);
        }
        
        return;
    }
    if (start.y == end.y)
    {
        if (start.x < end.x) {
            DrawHorizontalLine(start, end.x - start.x, colour);
        } else {
            Vector2 s = { .x = end.x, .y = start.y };
            DrawHorizontalLine(s, start.x - end.x, colour);
        }
        return;
    }
    
    //version of bresenham line drawing algorithm
    //works for any orientation of line
    int diffX = abs(end.x - start.x),
        diffY = abs(end.y - start.y),
        stepX = start.x < end.x ? 1 : -1,
        stepY = start.y < end.y ? 1 : -1;
    int err = (diffX>diffY ? diffX : -diffY)/2, 
        e2;
    int posX = start.x;
    int posY = start.y;
    
    for(;;){
        SetPixel(posX, posY, colour);
        //stops when we've reached the end of the line
        if (posX == end.x && posY == end.y) 
            break;
        e2 = err;

        //change positions based on how far away we are from where the line would actually be
        if (e2 > -diffX) { 
            err -= diffY; 
            posX += stepX; 
        }
        if (e2 < diffY) { 
            err += diffX; 
            posY += stepY; 
        }
    }
}

void DrawUserRectangle(uint32_t start, uint32_t size, uint8_t colour) {
    //Version used on the receiving end of user transfer code
    Vector2 s = Reverse32BitMergeVector2(start);
    Vector2 m = Reverse32BitMergeVector2(size);
    Rectangle r = { .x = s.x, .y = s.y, .width = m.x, .height = m.y };
    DrawRectangle(r, colour);
}

void DrawRectangle(Rectangle rect, uint8_t colour)
{
    //Fast method drawing a rectangle with horizontal and vertical methods
    Vector2 s = {.x = rect.x, .y = rect.y + rect.height};
    DrawHorizontalLine(s, rect.width, colour);
    s.y -= rect.height;
    DrawHorizontalLine(s, rect.width, colour);
    DrawVerticalLine(s, rect.height, colour);
    s.x += rect.width;
    DrawVerticalLine(s, rect.height, colour);
}

void FillUserRectangle(uint32_t start, uint32_t size, uint8_t colour) {
    //Version used on the receiving end of user transfer code
    Vector2 s = Reverse32BitMergeVector2(start);
    Vector2 m = Reverse32BitMergeVector2(size);
    Rectangle r = { .x = s.x, .y = s.y, .width = m.x, .height = m.y };
    FillRectangle(r, colour);
}

void FillRectangle(Rectangle rect, uint8_t colour) {
    //Draw horizontal line between minimum and maximum y
    Vector2 s = {.x = rect.x, .y = rect.y};
    for (int i = 0; i < rect.height; i++)
    {
        DrawHorizontalLine(s, rect.width, colour);
        s.y++;
    }
}

void DrawUserCircle(uint32_t centre, uint16_t radius, uint8_t colour) {
    //Version used on the receiving end of user transfer code
    Vector2 c = Reverse32BitMergeVector2(centre);
    DrawCircle(c, radius, colour);
}

void DrawCircle(Vector2 centre, uint16_t radius, uint8_t colour) {
    //Bresenham circle drawing algorithm, otherwise known as Midpoint circle algorithm
    Vector2 pos = {.x = radius, .y = 0};
    int16_t p = 1 - radius;
    uint16_t cxmpx, cxppx, cympy, cyppy, cxmpy, cxppy, cympx, cyppx;

    while (pos.x > pos.y)
    { 
        //minor optimization as these would otherwise be done twice.
        cxmpx = centre.x - pos.x;
        cxppx = centre.x + pos.x;
        cympy = centre.y - pos.y;
        cyppy = centre.y + pos.y;
        cxmpy = centre.x - pos.y;
        cxppy = centre.x + pos.y;
        cympx = centre.y - pos.x;
        cyppx = centre.y + pos.x;

        SetPixel(cxppx, cyppy, colour);
        SetPixel(cxmpx, cyppy, colour);
        SetPixel(cxppx, cympy, colour);
        SetPixel(cxmpx, cympy, colour);

        SetPixel(cxppy, cyppx, colour);
        SetPixel(cxppy, cympx, colour);
        SetPixel(cxmpy, cyppx, colour);
        SetPixel(cxmpy, cympx, colour);

        pos.y++;
        if (p <= 0)
        {
            p += (pos.y * 2) + 1;
        }
        else
        {
            pos.x--;
            p += (pos.y * 2) - (pos.x * 2) + 1;
        }
    }
}

void FillUserCircle(uint32_t centre, uint16_t radius, uint8_t colour) {
    //Version used on the receiving end of user transfer code
    Vector2 c = Reverse32BitMergeVector2(centre);
    FillCircle(c, radius, colour);
}

void FillCircle(Vector2 centre, uint16_t radius, uint8_t colour)
{
    Vector2 s = { .x = 0, .y = 0 };
    Vector2 e = { .x = 0, .y = 0 };
    Vector2 pos = {.x = radius, .y = 0};   
    int p = 1 - radius;
    uint16_t cxmpx, cxppx, cympy, cyppy, cxmpy, cxppy, cympx, cyppx;

    //From top to bottom draws a horizontal line between lowest and highest point
    while (pos.x >= pos.y)
    {
        cxmpx = centre.x - pos.x;
        cxppx = centre.x + pos.x;
        cympy = centre.y - pos.y;
        cyppy = centre.y + pos.y;
        cxmpy = centre.x - pos.y;
        cxppy = centre.x + pos.y;
        cympx = centre.y - pos.x;
        cyppx = centre.y + pos.x;

        s.x = cxppy;
        s.y = cympx;
        e.x = cxmpy;
        e.y = cympx;
        DrawLine(s, e, colour); //top

        s.x = cxppx;
        s.y = cympy;
        e.x = cxmpx;
        e.y = cympy;
        DrawLine(s, e, colour);//top-upper

        //s.x = cxppx;
        s.y = cyppy;
        //e.x = cxmpx;
        e.y = cyppy;
        DrawLine(s, e, colour); //top-lower

        s.x = cxppy;
        s.y = cyppx;
        e.x = cxmpy;
        e.y = cyppx;
        DrawLine(s, e, colour);//bottom

        pos.y++;
        if (p <= 0)
        {
            p += (pos.y * 2) + 1;
        }
        else
        {
            pos.x--;
            p += (pos.y * 2) - (pos.x * 2) + 1;
        }
    }
}

void DrawUserPolygon(uint16_t xPoints[], uint16_t yPoints[], uint32_t sizeCol) {
    //Version used on the receiving end of user transfer code
    Vector2 vSizeCol = Reverse32BitMergeVector2(sizeCol);
    uint8_t col = (uint8_t)vSizeCol.y;
    uint16_t size = vSizeCol.x;
    DrawPolygon(xPoints, yPoints, size, col);
}

void DrawPolygon(uint16_t xPoints[], uint16_t yPoints[], uint16_t sides, uint8_t colour)
{
    //Draw a line between each of the positions through the list and then last and first
    uint16_t length = sides;
    Vector2 s = {.x = 0, .y = 0};
    Vector2 e = {.x = 0, .y = 0};

    for (int i = 0; i < length - 1; i++)
    {
        s.x = xPoints[i];
        s.y = yPoints[i];
        e.x = xPoints[i + 1];
        e.y = yPoints[i + 1];
        DrawLine(s, e, colour);
    }
    s.x = xPoints[0];
    s.y = yPoints[0];
    e.x = xPoints[length-1];
    e.y = yPoints[length-1];
    DrawLine(s, e, colour);
}

uint8_t InPolygon(uint16_t x, uint16_t y, uint16_t xPoints[], uint16_t yPoints[], uint16_t length) {
    //Checks to see if a point is inside the bound of the given polygon. 
    //An adaptation of the algorithm created by Randolph Franklin in Fortran.
    uint8_t inside = false;
    int i, j;
    for (i = 0, j = length -1; i < length; j = i++) {
        if ((yPoints[i] > y) != (yPoints[j] > y) && 
            x < (xPoints[j] - xPoints[i]) * (y - yPoints[i]) / (yPoints[j] - yPoints[i]) + xPoints[i]) {
                inside = !inside;
            }
    }
    return inside;
}

void FillUserPolygon(uint16_t xPoints[], uint16_t yPoints[], uint32_t sizeCol) {
    //Version used on the receiving end of user transfer code
    Vector2 vSizeCol = Reverse32BitMergeVector2(sizeCol);
    uint8_t col = (uint8_t)vSizeCol.y;
    uint16_t size = vSizeCol.x;
    FillPolygon(xPoints, yPoints, size, col);
}

void FillPolygon(uint16_t xPoints[], uint16_t yPoints[], uint16_t sides, uint8_t colour) {
    int i, j;
    uint16_t minX = 65535;
    uint16_t minY = 65535;
    uint16_t maxX = 0;
    uint16_t maxY = 0;

    //creates bounding box around the polygon
    for (i = 0; i < sides; i++) {
        if (xPoints[i] < minX) {
            minX = xPoints[i];
        }
        if (xPoints[i] > maxX) {
            maxX = xPoints[i];
        }
        if (yPoints[i] < minY) {
            minY = yPoints[i];
        }
        if (yPoints[i] > maxY) {
            maxY = yPoints[i];
        }
    }
    //iterate through each position of the polygon to see if in polygon
    for (i = minX; i <= maxX; i++) {
        for (j = minY; j <= maxY; j++) {
            if (InPolygon(i, j, xPoints, yPoints, sides)) {
                SetPixel(i, j, colour);
            }
        }
    }
}
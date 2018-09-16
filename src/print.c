#include <print.h>
#include <stdint.h>
#include <draw.h>
#include <string.h>

#define FONT_WIDTH 9
#define FONT_HEIGHT 16
#define TOTAL_WIDTH 324

void WriteUserCharacter(char c, uint32_t position, uint8_t colour) {
    Vector2 p = Reverse32BitMergeVector2(position);
    WriteCharacter(c, p.x, p.y, colour);
}

void WriteCharacter(char c, uint16_t x, uint16_t y, uint8_t colour)
{
    uint16_t xpos = 0;
    //selection for the full bitmap, see .h for more info
    // if (c >= 32 && c <= 127) {
    //     xpos = (c - ' ') * FONT_WIDTH;
    // }

    //reduced character set A-Z 0-9
    if (c >= 97 && c <= 122)
    {
        xpos = (c - 'a') * FONT_WIDTH;
    }
    else if (c >= 65 && c <= 90)
    {
        xpos = (c - 'A') * FONT_WIDTH;
    }
    else if (c >= 48 && c <= 57)
    {
        xpos = (c - '0' + 26) * FONT_WIDTH;
    }
    else
    {
        return;
    }
    // uint8_t bitmask[8] = { 128, 64, 32, 16, 8, 4, 2, 1 };

    // for (uint16_t i = 0; i < FONT_WIDTH; i++) {
    //     for (uint16_t j = 0; j < FONT_HEIGHT; j++) {
    //         uint8_t pos = full_font[(FONT_HEIGHT * FONT_WIDTH * xpos) + j + i];
    //         for (uint16_t k = 0; k < 8; k++) {
    //             if (pos & bitmask[i] > 0) {
    //                 SetPixel(x + i, y + j + k, 5);
    //             }
    //         }
    //     }
    // }

    for (uint16_t i = 0; i < FONT_HEIGHT; i++)
    {
        for (uint16_t j = 0; j < FONT_WIDTH; j++)
        {
            //gets value at the point on the bitmap imported from GIMP
            uint8_t pixel = font[(TOTAL_WIDTH * i) + xpos + j] * colour;
            if (pixel > 0) {
                //only sets the actual character pixels, allows a transparency affect.
                SetPixel(x + j, y + i, pixel);
            }
            //upgrade idea. have a background highlighting colour as well. If 0 set to background colour value
        }
    }
}

void WriteUserText(const char* c, uint32_t position, uint8_t colour) {
    Vector2 p = Reverse32BitMergeVector2(position);
    WriteText(c, p.x, p.y, colour);
}

void WriteText(const char *text, uint16_t x, uint16_t y, uint8_t colour)
{
    //iterate through the provided string until we reach end character. Any character not recognised is ignored
    uint16_t len = strlen(text);
    for (int i = 0; i < len; i++)
    {
        //Space we just iterate, no need to print a space character
        if (text[i] != ' ')
        {
            WriteCharacter(text[i], i * FONT_WIDTH + x, y, colour);
        }

        //break early if we find a null termination, should not be needed with correct strlen implementation
        if (text[i] == '\0')
        {
            break;
        }
    }
}

char *ToStringUint16(uint16_t i, uint16_t base)
{
    //converts a uint16_t to a string
    char *buffer;
    char *buffer2;
    uint16_t temp = i;
    uint16_t j = 0;
    while (temp > 0)
    {
        //get remainder and then convert to ascii, add 7 for A-F for hex, or even higher bases
        uint16_t rem = temp % base;
        buffer[j] = (char)(rem + '0');
        if (rem > 9)
        {
            buffer[j] += 7;
        }
        temp /= base;
        j++;
    }
    uint16_t k;
    //reverse the buffer
    for (k = 0; k < j; k++)
    {
        buffer2[k] = buffer[j - k - 1];
    }
    buffer2[k] = '\0';
    return buffer2;
}
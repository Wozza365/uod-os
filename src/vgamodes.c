#include <hal.h>
#include <vgamodes.h>

//-----------------------------------------------------------------------------
// Do not change any code in this file unless you really know what you are doing
//
// VGA mode switcher based on code by Jonas Berlin -98 <jberlin@cc.hut.fi>
//

#define ARRAYSIZE(x) (sizeof(x)/sizeof(x[0]))

#define R_COM  0x63 // Common bits

#define R_W256 0x00
#define R_W320 0x00
#define R_W360 0x04
#define R_W376 0x04
#define R_W400 0x04

#define R_H200 0x00
#define R_H224 0x80
#define R_H240 0x80
#define R_H256 0x80
#define R_H270 0x80
#define R_H300 0x80
#define R_H360 0x00
#define R_H400 0x00
#define R_H480 0x80
#define R_H564 0x80
#define R_H600 0x80

static const uint8_t hor_regs [] = { 0x0,  0x1,  0x2,  0x3,  0x4, 0x5,  0x13 };
static const uint8_t width_256[] = { 0x5f, 0x3f, 0x40, 0x82, 0x4a, 0x9a, 0x20 };
static const uint8_t width_320[] = { 0x5f, 0x4f, 0x50, 0x82, 0x54, 0x80, 0x28 };
static const uint8_t width_360[] = { 0x6b, 0x59, 0x5a, 0x8e, 0x5e, 0x8a, 0x2d };
static const uint8_t width_376[] = { 0x6e, 0x5d, 0x5e, 0x91, 0x62, 0x8f, 0x2f };
static const uint8_t width_400[] = { 0x70, 0x63, 0x64, 0x92, 0x65, 0x82, 0x32 };

static const uint8_t ver_regs  [] = { 0x6,  0x7,  0x9,  0x10, 0x11, 0x12, 0x15, 0x16 };
static const uint8_t height_200[] = { 0xbf, 0x1f, 0x41, 0x9c, 0x8e, 0x8f, 0x96, 0xb9 };
static const uint8_t height_224[] = { 0x0b, 0x3e, 0x41, 0xda, 0x9c, 0xbf, 0xc7, 0x04 };
static const uint8_t height_240[] = { 0x0d, 0x3e, 0x41, 0xea, 0xac, 0xdf, 0xe7, 0x06 };
static const uint8_t height_256[] = { 0x23, 0xb2, 0x61, 0x0a, 0xac, 0xff, 0x07, 0x1a };
static const uint8_t height_270[] = { 0x30, 0xf0, 0x61, 0x20, 0xa9, 0x1b, 0x1f, 0x2f };
static const uint8_t height_300[] = { 0x70, 0xf0, 0x61, 0x5b, 0x8c, 0x57, 0x58, 0x70 };
static const uint8_t height_360[] = { 0xbf, 0x1f, 0x40, 0x88, 0x85, 0x67, 0x6d, 0xba };
static const uint8_t height_400[] = { 0xbf, 0x1f, 0x40, 0x9c, 0x8e, 0x8f, 0x96, 0xb9 };
static const uint8_t height_480[] = { 0x0d, 0x3e, 0x40, 0xea, 0xac, 0xdf, 0xe7, 0x06 };
static const uint8_t height_564[] = { 0x62, 0xf0, 0x60, 0x37, 0x89, 0x33, 0x3c, 0x5c };
static const uint8_t height_600[] = { 0x70, 0xf0, 0x60, 0x5b, 0x8c, 0x57, 0x58, 0x70 };

// Output word value to specified output port

void VGA_OutputWordToPort(uint16_t portid, uint16_t value)
{
	asm volatile ("outw %0, %1"
				  :
				  : "a"(value), "Nd"(portid));
}

uint16_t screenWidth;
uint16_t screenHeight;

// Switch VGA Mode.
//
// The chain4Mode parameter should be 1 for normal mode, but 
// only allows 320x200, 256x200, 256x240 and 256x256 resolutions
// because you can only access the first 64KB for VGA memory.

// If chain4Mode is 0, then plane mode is used and you will
// need to switch planes to access the whole screen, but
// that allows you using any VGA resolution.

// Returns 1 = Mode switched, 0 = Fail

int VGA_SetGraphicsMode(uint16_t width, uint16_t height, uint8_t chain4Mode) 
{
	const uint8_t *w;
	const uint8_t *h;
	uint8_t val;
	int i;

	switch(width) 
	{
		case 256: w=width_256; val=R_COM+R_W256; break;
		case 320: w=width_320; val=R_COM+R_W320; break;
		case 360: w=width_360; val=R_COM+R_W360; break;
		case 376: w=width_376; val=R_COM+R_W376; break;
		case 400: w=width_400; val=R_COM+R_W400; break;
		default: return 0; // fail
	}
	switch(height) 
	{
		case 200: h=height_200; val|=R_H200; break;
		case 224: h=height_224; val|=R_H224; break;
		case 240: h=height_240; val|=R_H240; break;
		case 256: h=height_256; val|=R_H256; break;
		case 270: h=height_270; val|=R_H270; break;
		case 300: h=height_300; val|=R_H300; break;
		case 360: h=height_360; val|=R_H360; break;
		case 400: h=height_400; val|=R_H400; break;
		case 480: h=height_480; val|=R_H480; break;
		case 564: h=height_564; val|=R_H564; break;
		case 600: h=height_600; val|=R_H600; break;
		default: return 0; // fail
	}

	// chain4Mode not available if mode takes over 64k
	if(chain4Mode && (long)width * (long)height > 65536L) 
	{
		return 0; 
	}	

	screenWidth = width;
	screenHeight = height;
	
	// Start mode switch
	HAL_OutputByteToPort(0x3c2,val);
	VGA_OutputWordToPort(0x3d4,0x0e11); 
   
    for (i=0; i < ARRAYSIZE(hor_regs); ++i) 
    {
		VGA_OutputWordToPort(0x3d4,(uint16_t)((w[i]<<8)+hor_regs[i]));
    }
    for(i=0; i < ARRAYSIZE (ver_regs); ++i)
	{
		VGA_OutputWordToPort(0x3d4,(uint16_t)((h[i]<<8)+ver_regs[i]));
	}
    VGA_OutputWordToPort(0x3d4,0x0008); 
    if(chain4Mode) 
	{
		VGA_OutputWordToPort(0x3d4,0x4014);
		VGA_OutputWordToPort(0x3d4,0xa317);
		VGA_OutputWordToPort(0x3c4,0x0e04);
	} 
	else 
	{
		VGA_OutputWordToPort(0x3d4,0x0014);
		VGA_OutputWordToPort(0x3d4,0xe317);
		VGA_OutputWordToPort(0x3c4,0x0604);
	}

	VGA_OutputWordToPort(0x3c4,0x0101);
	VGA_OutputWordToPort(0x3c4,0x0f02); 
	VGA_OutputWordToPort(0x3ce,0x4005); 
	VGA_OutputWordToPort(0x3ce,0x0506); 

    HAL_InputByteFromPort(0x3da);
    HAL_OutputByteToPort(0x3c0,0x30); 
	HAL_OutputByteToPort(0x3c0,0x41);
    HAL_OutputByteToPort(0x3c0,0x33); 
	HAL_OutputByteToPort(0x3c0,0x00);

    for(i=0; i<16; i++) 
	{   
		HAL_OutputByteToPort(0x3c0,(uint8_t)i); 
		HAL_OutputByteToPort(0x3c0,(uint8_t)i); 
	} 
   
	HAL_OutputByteToPort(0x3c0, 0x20); // Enable video
	return 1;
}


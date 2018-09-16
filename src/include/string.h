#ifndef _STRING_H_
#define _STRING_H_

#include <size_t.h>
#include <_null.h>
#include <errno.h>

int strcmp(const char* str1, const char* str2); 
char * strcpy(char * destination, const char * source);
errno_t strcpy_s(char *destination, size_t numberOfElements, const char *source);
size_t strlen(const char* source);
void * memcpy(void * destination, const void * source, size_t count);
errno_t memcpy_s(void *destination, size_t destinationSize, const void *source, size_t count);
void * memset(void *destination, char val, size_t count);
unsigned short * memsetw(unsigned short * destination, unsigned short val, size_t count);

#endif
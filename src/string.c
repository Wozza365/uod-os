// Implementation of string and memory functions

#include <string.h>

// Compare two strings
int strcmp(const char* str1, const char* str2) 
{
	int result = 0;
	while (!(result = *(unsigned char*)str1 - *(unsigned char*)str2) && *str2)
	{
		++str1;
		++str2;
	}
	if (result < 0)
	{
		result = -1;
	}
	if (result > 0)
	{
		result = 1;
	}
	return result;
}


// Copy string source to destination

char * strcpy(char * destination, const char * source)
{
    char * destinationTemp = destination;
    while (*destination++ = *source++);
    return destinationTemp;
}

// Safe version of strcpy

errno_t strcpy_s(char *destination, size_t numberOfElements, const char *source)
{
	if (destination == NULL || source == NULL)
	{
		return EINVAL;
	}
	if (numberOfElements == 0)
	{
		*destination = '\0';
		return ERANGE;
	}
	char * sourceTemp = (char *)source;
	char * destinationTemp = destination;
	char charSource;
	while (numberOfElements--)
	{
		charSource = *sourceTemp++;
		*destinationTemp = charSource;
		if (charSource == 0)
		{
			// Copy succeeded
			return 0;
		}
	}
	// We haven't reached the end of the string
	*destination = '\0';
	return ERANGE;
}


// Return length of string

size_t strlen(const char* source) 
{
	size_t len = 0;
	while (source[len++]);
	return len;
}

// Copy count bytes from src to dest

void * memcpy(void * destination, const void * source, size_t count)
{
    const char *sp = (const char *)source;
    char *dp = (char *)destination;
    while (count != 0)
	{
		*dp++ = *sp++;
		count--;
	}
    return destination;
}

// Safe verion of memcpy

errno_t memcpy_s(void *destination, size_t destinationSize, const void *source, size_t count)
{
	if (destination == NULL)
	{
		return EINVAL;
	}
	if (source == NULL)
	{
		memset((void *)destination, '\0', destinationSize);
		return EINVAL;
	}
	if (destinationSize < count == 0)
	{
		memset((void *)destination, '\0', destinationSize);
		return ERANGE;
	}
	char * sp = (char *)source;
	char * dp = (char *)destination;
	while (count--)
	{
		*dp++ = *sp++;
	}
	return 0;
}

// Set count bytes of destination to val

void * memset(void *destination, char val, size_t count)
{
    unsigned char * temp = (unsigned char *)destination;
	while (count != 0)
	{
		temp[--count] = val;
		
	}
	return destination;
}

// Set count bytes of destination to val

unsigned short * memsetw(unsigned short * destination, unsigned short val, size_t count)
{
    unsigned short * temp = (unsigned short *)destination;
    while(count != 0)
	{
		*temp++ = val;
		count--;
	}
    return destination;
}


#include <stdio.h>

int readInt()
{
	int a;
	scanf("%d", &a);
	return a;
}

int readChar()
{
	char c;
	scanf("%c", &c);
	return c;
}

void printStr(char *a)
{
	printf("%s\n", a);
}

void printInt(int a)

{	printf("%d\n", a);
}

void printChar(char c)
{
	printf("%c\n", c);
}

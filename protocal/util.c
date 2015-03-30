#include "util.h"

void print_block(char s[], char* buf, int len)
{
              printf("%s\n", s); 
	      int i = 0;
	      for (i = 0; i < len; i++)
	      {
		  printf("0x%02hhX,", *(buf+i));
	      }
	      printf("\n");
}

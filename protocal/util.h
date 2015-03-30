#ifndef _UTIL_H_
#define _UTIL_H_

#include "config.h"
#include <stdio.h>

void print_block(char s[], char* buf, int len);

#ifdef LINUX_BUILD
    #define PRO_PRINTFH(...) printf(__VA_ARGS__) 
    #define PRO_BLOCKPRINTFH(...) print_block(__VA_ARGS__) 
    #ifdef DEBUG_MODE
	#define PRO_PRINTF(...) printf(__VA_ARGS__)
        #define PRO_BLOCKPRINTF(...) print_block(__VA_ARGS__) 
    #else
        #define PRO_PRINTF(...)
        #define PRO_BLOCKPRINTF(...)
    #endif
#elif defined (JNI_BUILD)


#endif





























#endif

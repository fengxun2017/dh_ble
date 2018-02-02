
#include "../../include/DhGlobalHead.h"

#ifdef DEBUG_LOG


// Forward declartion of Segger vprintf.
int SEGGER_RTT_vprintf(unsigned BufferIndex, const char * sFormat, va_list * pParamList);

#define LOG_BUFF 0
#define BUFF_SIZE 1024
#define NULL 0

// Spaces and length for maximum file names
#define MAX_FILE_LEN 25 

static char buffer[BUFF_SIZE]; // The buffer

void rtt_log_general_print(int terminal, const char* color, const char *file, int line, const char * sFormat, ...) {
  static int initialized = 0;
                             //01234567890123456789012345
//static const char *spaces = "                          ";
//	const char *lastSlash = file;
//	const char *current = file;	
//	int nameLen;
	va_list ParamList;

if(!initialized) {
	SEGGER_RTT_ConfigUpBuffer(LOG_BUFF, NULL, buffer, BUFF_SIZE, SEGGER_RTT_MODE_NO_BLOCK_SKIP);
	initialized = 1;
}

	// Set Terminal
	SEGGER_RTT_SetTerminal(terminal);

	// Set Color
	SEGGER_RTT_WriteString(LOG_BUFF, color);
	
	// Print file name

//	while(*current!=0) {
//		if(*current == '\\' || *current == '/') 
//			lastSlash = current+1;
//		current++;
//	}
	// Print name/line number
//    SEGGER_RTT_printf(LOG_BUFF, "%s:%d",lastSlash,line);	
//	
//	nameLen = (current-lastSlash)+1+(line>999?4:((line>99)?3:((line>9)?2:1)));
//	
//	// Print justification spacing	
//	if(nameLen<MAX_FILE_LEN) 
//		SEGGER_RTT_WriteString(LOG_BUFF,spaces+nameLen);

	// Print actual error message stuff
    va_start(ParamList, sFormat);
	SEGGER_RTT_vprintf(LOG_BUFF, sFormat, &ParamList);

    SEGGER_RTT_WriteString(LOG_BUFF, "\n");
}


void DebugData(u1 *pData, u2 len)
{
	u2 i;
#if defined(DEBUG_LOG_USE_RTT)
	for( i = 0; i < len; i++)
	{
		SEGGER_RTT_printf(0,"%02x ",pData[i]);
	}
	SEGGER_RTT_printf(0,"\r\n");
#elif defined(DEBUG_LOG_USE_UART)
	for( i = 0; i < len; i++)
	{
		DhPrintf("%02x ",pData[i]);
	}
	DhPrintf("\r\n");
#endif
}

void DebugAscii(u1 *pData, u2 len)
{
	u2 i;
#if defined(DEBUG_LOG_USE_RTT)
	for( i = 0; i < len; i++)
	{
		SEGGER_RTT_printf(0,"%c",pData[i]);
	}
	SEGGER_RTT_printf(0,"\r\n");
#elif defined(DEBUG_LOG_USE_UART)
	for( i = 0; i < len; i++)
	{
		DhPrintf("%c",pData[i]);
	}
	DhPrintf("\r\n");
#endif	
}
#endif

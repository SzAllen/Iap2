#if CONFIG_CMDLINE

#include "CmdLine.h"

static CmdLine g_CmdLine;

int CmdLine_htoi(const char *s)
{
	int n = 0;

	if( !s ) return 0;

	if( *s == '0' )
	{
		s++;
		if( *s == 'x' || *s == 'X' )s++;
	}

	while(*s != '\0')
	{
		n <<= 4;
		if( *s <= '9' )
			n |= ( *s & 0xf );
		else
			n |= ( (*s & 0xf) + 9 );
		s++;
	}
	return n;
}

int CmdLine_Printf(const char* lpszFormat, ...)
{
	int nLen = 0;
	va_list ptr;
	char g_Pfbuffer[128];
	//LOCK();

	memset(g_Pfbuffer, 0, sizeof(g_Pfbuffer));
	va_start(ptr, lpszFormat);
	nLen = vsnprintf(g_Pfbuffer, sizeof(g_Pfbuffer), lpszFormat, ptr);
	va_end(ptr);
	
	if(g_CmdLine.printf) g_CmdLine.printf(g_Pfbuffer);
	
	//UNLOCK();

	return nLen;
}

void CmdLine_Help()
{
	int i = 0;
	const FnDef* pFnDef = g_CmdLine.m_FnArray;
	
	for(i = 0; i < g_CmdLine.m_FnCount; i++, pFnDef++)
	{		
		CmdLine_Printf("\t %s\n", pFnDef->m_Title);
	}
}

ArgType CmdLine_GetArgType(const char* argStr)
{
	int nLen = strlen(argStr);

	if(Null == argStr) return AT_NONE;

	if(*argStr == '\"')
	{
		if('\"' == argStr[nLen-1])
		{
			return AT_STR;
		}
		else
		{
			return AT_ERROR;
		}
	}
	else if(argStr[0] == '0' && (argStr[1] == 'x' || argStr[1] == 'X'))
	{
		return AT_HEX;
	}
	else
	{
		return AT_DEC;
	}
}

Bool CmdLine_Parse(char* cmdLineStr, char** pFnName, char* pArgs[], int* argCount)
{
	int maxArgCount = *argCount;
	char *token;
	char fnNameseps[]   = " (\n";
	char argSeps[]   = ", )\n";

	//Find function name
	token = strtok(cmdLineStr, fnNameseps);
	if(Null == token) return False;
	*pFnName = token;

	*argCount= 0;
	token = strtok( NULL, argSeps);
	while( token != NULL )
	{
		pArgs[(*argCount)++] = token;
		if((*argCount) > maxArgCount)
		{
			CmdLine_Printf("PF_ERROR: Arg count is too many\n");
			return False;
		}
		token = strtok( NULL, argSeps);
	}

	return True;
}

Bool CmdLine_ArgConvert(char* pArgs[], int argCount, uint32 arg[])
{
	int i = 0;
	ArgType at = AT_NONE;
	char* pChar = Null;

	for(i = 0; i < argCount; i++)
	{
		at = CmdLine_GetArgType(pArgs[i]);
		if(AT_DEC == at)
		{
			arg[i] = atoi(pArgs[i]);
		}
		else if(AT_HEX == at)
		{
			arg[i] = CmdLine_htoi(pArgs[i]);
		}
		else if(AT_STR == at)
		{
			pChar = pArgs[i];
			pChar[strlen(pChar) - 1] = 0;
			pChar++;
			arg[i] = (uint32)pChar;
		}
		else if(AT_DEC == at)
		{
			arg[i] = 0;
		}
		else
		{
			CmdLine_Printf("\tArg[%d](%s) error. \n", i+1, pArgs[i]);
			return False;
		}
	}
	return True;
}

void CmdLine_Exe(CmdLine* pCmdLine, const char* pFnName, uint32 arg[], int argCount)
{
	Bool isFind = False;
	int i = 0;
	const FnDef* pFnEntry = pCmdLine->m_FnArray;
	
	#define FUN(n, funType, args) if(n == pFnEntry->m_ArgCount)	\
		{	\
			((funType)pFnEntry->pFn) args;	\
			return;	\
		}

	for(i = 0; i < pCmdLine->m_FnCount; i++, pFnEntry++)
	{
		if(strcmp(pFnName, "?") == 0)
		{
			CmdLine_Help();
			return;
		}

		if(memcmp(pFnEntry->m_Title, pFnName, strlen(pFnName)) == 0)
		{
			isFind = True;
			break;
		}
	}

	if(!isFind)
	{
		CmdLine_Printf("No function: %s\n", pFnName);
		return;
	}

	FUN(0, FnArg0, ());
	FUN(1, FnArg01, (arg[0]));
	FUN(2, FnArg02, (arg[0], arg[1]));
	FUN(3, FnArg03, (arg[0], arg[1], arg[2]));
	FUN(4, FnArg04, (arg[0], arg[1], arg[2], arg[3]));
	FUN(5, FnArg05, (arg[0], arg[1], arg[2], arg[3], arg[4]));
}

int CmdLine_GetArgCount(const char* str)
{
	Bool bFlag = False;
	int nArgCount = 0;
	
	str	= strchr(str, '(');

	if(Null == str)
	{
		return 0;
	}
	
	while(*(++str) != '\0')
	{
		if(')' == *str)
		{
			break;
		}
		else if(!bFlag)
		{
			if(' ' != *str)
			{
				bFlag = True;
				nArgCount++;

				if(',' == *str)
				{
					nArgCount++;
				}
			}
		}
		else if(',' == *str)
		{
			nArgCount++;
		}
	}
	
	return *str == ')' ? nArgCount : -1;
}

void CmdLine_Reset(CmdLine* pCmdLine)
{
	memset(pCmdLine->m_CmdLineStr, 0, sizeof(pCmdLine->m_CmdLineStr));
	pCmdLine->m_CmdLineStrLen = 0;
}

void CmdLine_AddStr(const char* str)
{
	CmdLine_AddStrEx(str, strlen(str));
}

void CmdLine_AddStrEx(const char* str, int len)
{
	int i = 0;
	CmdLine* pCmdLine = &g_CmdLine;

	for(i = 0; i < len; i++, str++)
	{
		if(pCmdLine->m_CmdLineStrLen >= MAX_CMDLINE_LEN)
		{
			CmdLine_Reset(pCmdLine);
		}
		
		if(pCmdLine->m_isEcho && *str != KEY_ENTER)
		{
			if(0 == pCmdLine->m_CmdLineStrLen)
			{
				CmdLine_Printf("->");
			}
			CmdLine_Printf("%c", *str);
		}
		
		if(*str != KEY_ENTER)
		{
			pCmdLine->m_CmdLineStr[pCmdLine->m_CmdLineStrLen++] = *str;
		}
		if(KEY_ENTER == *str || ')' == *str)
		{
			char* pFnName = Null;
			char* argStr[MAX_ARG_COUNT] = {0};
			int argCount = MAX_ARG_COUNT;
			
			if(pCmdLine->m_isEcho) CmdLine_Printf("\n");

			if(CmdLine_Parse(pCmdLine->m_CmdLineStr, &pFnName, argStr, &argCount))
			{
				uint32 arg[MAX_ARG_COUNT] = {0};
				
				if(CmdLine_ArgConvert(argStr, argCount, arg))
				{
					CmdLine_Exe(pCmdLine, pFnName, arg, argCount);
				}
			}
			CmdLine_Reset(pCmdLine);
	   	}
	}
}

void CmdLine_Init(FnDef* pCmdTable, uint8 cmdTableCount, Bool isEcho, OutPutFun printf)
{
	int i = 0;
	FnDef* pFnEntry = pCmdTable;
	
	memset(&g_CmdLine, 0, sizeof(CmdLine));
	
	g_CmdLine.m_isEcho = isEcho;

	g_CmdLine.m_FnArray = pCmdTable;
	g_CmdLine.m_FnCount = cmdTableCount;
	g_CmdLine.printf = printf;

	for(i = 0; i < cmdTableCount; i++, pFnEntry++)
	{
		int argCount = CmdLine_GetArgCount(pFnEntry->m_Title);
		if(argCount < 0 || argCount > MAX_ARG_COUNT)
		{
			CmdLine_Printf("[%s] error, get arg count[%d] error.\n", pFnEntry->m_Title, pFnEntry->m_ArgCount);
		}
		
		pFnEntry->m_ArgCount = (int8)argCount;
	}
}

#endif


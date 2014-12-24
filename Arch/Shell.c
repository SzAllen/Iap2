#if CONFIG_CMDLINE

#include "Shell.h"
#include "Debug.h"

void Shell_Test1(uint8 arg1)
{
	int i = 0;
	Printf("%s: arg1=%d\n", _FUNC_, arg1);
}
void Shell_Test2(char arg1, uint16 arg2)
{
	Printf("%s: arg1=%d, arg2=%d\n", _FUNC_, arg1, arg2);

	//AssertTrue(arg1 == arg2);
}
void Shell_Test3(uint8 arg1, uint16 arg2, short arg3)
{
	Printf("%s: arg1=%d, arg2=%d, arg3=%d\n", _FUNC_, arg1, arg2, arg3);
}
void Shell_Test4(uint8 arg1, uint16 arg2, short arg3, uint32 arg4)
{
	Printf("%s: arg1=0x%x, arg2=0x%x, arg3=%d, arg4=0x%x\n", _FUNC_, arg1, arg2, arg3, arg4);
}
void Shell_Test5(char* arg1, uint16 arg2, char* arg3, uint32 arg4, char* arg5)
{
	Printf("%s: arg1=\"%s\", arg2=%d, arg3=\"%s\", arg4=0x%x, arg5=\"%s\"\n", _FUNC_, arg1, arg2, arg3, arg4, arg5);
}

void Shell_GetArgCount()
{
	AssertTrue(CmdLine_GetArgCount("Test()") == 0);
	AssertTrue(CmdLine_GetArgCount("Test(") == -1);
	AssertTrue(CmdLine_GetArgCount("Test(  )") == 0);
	AssertTrue(CmdLine_GetArgCount("Test(,,,)") == 4);
	AssertTrue(CmdLine_GetArgCount("Test(uint8 arg1)") == 1);
	AssertTrue(CmdLine_GetArgCount("Test(char arg1, uint16 arg2)") == 2);
	AssertTrue(CmdLine_GetArgCount("Test(uint8 arg1, uint16 arg2, short arg3)") == 3);
	AssertTrue(CmdLine_GetArgCount("Test(uint8 arg1, uint16 arg2, short arg3, uint32 arg4)") == 4);
	AssertTrue(CmdLine_GetArgCount("Test(char* arg1, uint16 arg2, char* arg3, uint32 arg4, char* arg5)") == 5);
}

///////////////////////////////////////////////////////

void Shell_Init()
{
	static FnDef g_CmdTable[] = 
	{
		  {Shell_Test1, "Test1(uint8 arg1)"}
		 ,{Shell_Test2, "Test2(char arg1, uint16 arg2)"}
		 ,{Shell_Test3, "Test3(uint8 arg1, uint16 arg2, short arg3)"}
		 ,{Shell_Test4, "Test4(uint8 arg1, uint16 arg2, short arg3, uint32 arg4)"}
		 ,{Shell_Test5, "Test5(char* arg1, uint16 arg2, char* arg3, uint32 arg4, char* arg5)"}
		 
		 //,{"GetArgCount", Shell_GetArgCount, "GetArgCount()"}
	};
	
	CmdLine_Init(g_CmdTable, sizeof(g_CmdTable) / sizeof(FnDef), True, (OutPutFun)Printf);
}

#endif
